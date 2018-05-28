// Copyright 2010-2014 RethinkDB, all rights reserved.
#include "btree/compression_operations.hpp"

#include "btree/operations.hpp"
#include "buffer_cache/alt.hpp"
#include "buffer_cache/blob.hpp"
#include "buffer_cache/serialize_onto_blob.hpp"
#include "containers/archive/vector_stream.hpp"
#include "containers/archive/versioned.hpp"

ATTR_PACKED(struct btree_compression_block_t {
    static const int COMPRESSION_BLOB_MAXREFLEN = 4076;

    block_magic_t magic;
    char compression_blob[COMPRESSION_BLOB_MAXREFLEN];
});

template <cluster_version_t W>
struct btree_compression_block_magic_t {
    static const block_magic_t value;
};

block_magic_t v2_5_compress_block_magic = { { 'c', 'd', 'a', 'd' } };

template <>
const block_magic_t
btree_compression_block_magic_t<cluster_version_t::v2_5_is_latest>::value
    = { { 'c', 'd', 'a', 'e' } };

template <cluster_version_t W>
void serialize(write_message_t *wm, const compression_t &compression) {
    serialize_varint_uint64(wm, compression.superblock);

    serialize_varint_uint64(wm, compression.dictionary->size());
    for (auto it = compression.dictionary->begin(), e = compression.dictionary->end(); it != e; ++it) {
        serialize<W>(wm, it->first);
        serialize<W>(wm, it->second);
    }
}
INSTANTIATE_SERIALIZE_FOR_CLUSTER(compression_t);

template <cluster_version_t W>
archive_result_t deserialize(read_stream_t *s, compression_t *compression) {
    archive_result_t res = deserialize_varint_uint64(s, &(compression->superblock));
    if (bad(res)) { return res; }

    uint64_t sz;
    res = deserialize_varint_uint64(s, &sz);
    if (bad(res)) { return res; }

    if (sz > std::numeric_limits<size_t>::max()) {
        return archive_result_t::RANGE_ERROR;
    }

    std::map<std::pair<std::uint16_t, char>, std::uint16_t> *m = new std::map<std::pair<std::uint16_t, char>, std::uint16_t>();

    for (uint64_t i = 0; i < sz; ++i) {
        std::pair<std::uint16_t, char> p;
        res = deserialize<W>(s, &p);
        if (bad(res)) { return res; }

        char val;
        res = deserialize<W>(s, &val);
        if (bad(res)) { return res; }

        m->insert(std::pair<std::pair<std::uint16_t, char>, std::uint16_t>(p, val));
    }

    compression->dictionary = m;

    return archive_result_t::SUCCESS;
}
INSTANTIATE_DESERIALIZE_FOR_CLUSTER(compression_t);

cluster_version_t compression_block_version(const btree_compression_block_t *data) {
    if (data->magic
               != btree_compression_block_magic_t<cluster_version_t::v2_5_is_latest>::value) {
        fail_due_to_user_error(
            "Data compression is not supported below version 2.5");
    } else if (data->magic
               == btree_compression_block_magic_t<
                   cluster_version_t::v2_5_is_latest>::value)  {
        return cluster_version_t::v2_5_is_latest;
    } else {
        crash("Unexpected magic in btree_compression_block_t.");
    }
}

void get_compression_dictionary(buf_lock_t *compression_dict_block,
                         std::map<std::pair<std::uint16_t, char>, std::uint16_t> *dictionary_out) {
    compression_t compression;
    get_compression_dictionary_internal(compression_dict_block, &compression);

    dictionary_out = compression.dictionary;
}

void get_compression_dictionary_internal(buf_lock_t *compression_dict_block,
                                compression_t *compression_out) {
    buf_read_t read(compression_dict_block);
    const btree_compression_block_t *data
        = static_cast<const btree_compression_block_t *>(read.get_data_read());

    blob_t compression_blob(compression_dict_block->cache()->max_block_size(),
                       const_cast<char *>(data->compression_blob),
                       btree_compression_block_t::COMPRESSION_BLOB_MAXREFLEN);
    deserialize_for_version_from_blob(compression_block_version(data),
                                      buf_parent_t(compression_dict_block), &compression_blob, compression_out);    
}

void set_compression_dictionary_value(buf_lock_t *compression_dict_block,
                            compression_t &compression) {
    buf_write_t write(compression_dict_block);
    btree_compression_block_t *data
        = static_cast<btree_compression_block_t *>(write.get_data_write());

    blob_t compression_blob(compression_dict_block->cache()->max_block_size(),
                       data->compression_blob,
                       btree_compression_block_t::COMPRESSION_BLOB_MAXREFLEN);

    data->magic = btree_compression_block_magic_t<cluster_version_t::LATEST_DISK>::value;
    serialize_onto_blob<cluster_version_t::LATEST_DISK>(
            buf_parent_t(compression_dict_block), &compression_blob, compression);
}

void initialize_compression_dictionary(buf_lock_t *compression_dict_block,
        compression_t &compression) {
    buf_write_t write(compression_dict_block);
    btree_compression_block_t *data
        = static_cast<btree_compression_block_t *>(write.get_data_write());
    data->magic = btree_compression_block_magic_t<cluster_version_t::LATEST_DISK>::value;
    memset(data->compression_blob, 0, btree_compression_block_t::COMPRESSION_BLOB_MAXREFLEN);

    set_compression_dictionary_value(compression_dict_block,
                                   compression);
}

void migrate_compression_dict_block(buf_lock_t *compression_block) {
    cluster_version_t block_version;
    {
        buf_read_t read(compression_block);
        const btree_compression_block_t *data
            = static_cast<const btree_compression_block_t *>(read.get_data_read());
        block_version = compression_block_version(data);
    }

    compression_t compression;
    get_compression_dictionary_internal(compression_block, &compression);
    if (block_version != cluster_version_t::LATEST_DISK) {
        set_compression_dictionary_value(compression_block, compression);
    }
}

