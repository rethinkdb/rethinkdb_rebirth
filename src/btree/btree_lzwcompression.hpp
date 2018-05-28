// Copyright 2010-2015 RethinkDB, all rights reserved.
#ifndef BTREE_LZW_COMPRESSION_HPP_
#define BTREE_LZW_COMPRESSION_HPP_

#include <map>
#include <string>
#include <vector>

#include "btree/keys.hpp"
#include "buffer_cache/types.hpp"
#include "containers/archive/archive.hpp"
#include "containers/uuid.hpp"
#include "rpc/serialize_macros.hpp"
#include "serializer/types.hpp"

namespace globals {

/// Dictionary Maximum Size (when reached, the dictionary will be reset)
const std::uint16_t dms {std::numeric_limits<std::uint16_t>::max()};

} // namespace globals

void compress(std::istream &is, std::ostream &os, std::map<std::pair<std::uint16_t, char>, std::uint16_t>& dictionary);

void decompress(std::istream &is, std::ostream &os);