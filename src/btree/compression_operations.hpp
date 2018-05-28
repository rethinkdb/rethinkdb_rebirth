// Copyright 2010-2015 RethinkDB, all rights reserved.
#ifndef BTREE_COMPRESSION_OPERATIONS_HPP_
#define BTREE_COMPRESSION_OPERATIONS_HPP_

#include <map>
#include <string>
#include <vector>

#include "btree/keys.hpp"
#include "buffer_cache/types.hpp"
#include "containers/archive/archive.hpp"
#include "containers/uuid.hpp"
#include "rpc/serialize_macros.hpp"
#include "serializer/types.hpp"

/* This file contains code for working with the compression block */

class buf_lock_t;

struct compression_t {
    compression_t()
        : superblock(NULL_BLOCK_ID) { }

    /* A virtual superblock. */
    block_id_t superblock;

    std::map<std::pair<std::uint16_t, char>, std::uint16_t> *dictionary;
};

RDB_DECLARE_SERIALIZABLE(compression_t);

void initialize_compression_dictionary(buf_lock_t *superblock, compression_t &compression);

void get_compression_dictionary(buf_lock_t *compression_dict_block,
                         std::map<std::pair<std::uint16_t, char>, std::uint16_t> *dictionary_out);

void get_compression_dictionary_internal(buf_lock_t *compression_dict_block,
                                compression_t *compression_out);

/* Overwrites existing values with the same id. */
void set_compression_dictionary_value(buf_lock_t *compression_dict_block,
                         	compression_t &compression);

void migrate_compression_dict_block(buf_lock_t *compression_block);

#endif /* BTREE_COMPRESSION_OPERATIONS_HPP_ */
