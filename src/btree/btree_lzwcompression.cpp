#include "btree/btree_lzwcompression.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <map>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace globals {

/// Dictionary Maximum Size (when reached, the dictionary will be reset)
const std::uint16_t dms {std::numeric_limits<std::uint16_t>::max()};

} // namespace globals

void compress(std::istream &is, std::ostream &os, std::map<std::pair<std::uint16_t, char>, std::uint16_t>& dictionary)
{
    // "named" lambda function, used to reset the dictionary to its initial contents
    const auto reset_dictionary = [&dictionary] {
        dictionary.clear();

        const long int minc = std::numeric_limits<char>::min();
        const long int maxc = std::numeric_limits<char>::max();

        for (long int c = minc; c <= maxc; ++c)
        {
            // to prevent Undefined Behavior, resulting from reading and modifying
            // the dictionary object at the same time
            const std::uint16_t dictionary_size = dictionary.size();

            dictionary[{globals::dms, static_cast<char> (c)}] = dictionary_size;
        }
    };

    reset_dictionary();

    std::uint16_t i {globals::dms}; // Index
    char c;

    while (is.get(c))
    {
        // dictionary's maximum size was reached
        if (dictionary.size() == globals::dms)
            reset_dictionary();

        if (dictionary.count({i, c}) == 0)
        {
            // to prevent Undefined Behavior, resulting from reading and modifying
            // the dictionary object at the same time
            const std::uint16_t dictionary_size = dictionary.size();

            dictionary[{i, c}] = dictionary_size;
            os.write(reinterpret_cast<const char *> (&i), sizeof (std::uint16_t));
            i = dictionary.at({globals::dms, c});
        }
        else
            i = dictionary.at({i, c});
    }

    if (i != globals::dms)
        os.write(reinterpret_cast<const char *> (&i), sizeof (std::uint16_t));
}

void decompress(std::istream &is, std::ostream &os)
{
    std::map<std::pair<std::uint16_t, char>> dictionary;

    const auto rebuild_string = [&dictionary](std::uint16_t k) -> std::vector<char> {
        std::vector<char> s; // String

        while (k != globals::dms)
        {
            s.push_back(dictionary[k].second);
            k = dictionary[k].first;
        }

        std::reverse(s.begin(), s.end());
        return s;
    };

    std::uint16_t i {globals::dms}; // Index
    std::uint16_t k; // Key

    while (is.read(reinterpret_cast<char *> (&k), sizeof (std::uint16_t)))
    {
        if (k > dictionary.size())
            throw std::runtime_error("invalid compressed code");

        std::vector<char> s; // String

        s = rebuild_string(k);

        os.write(&s.front(), s.size());
        i = k;
    }

    if (!is.eof() || is.gcount() != 0)
        throw std::runtime_error("corrupted compressed file");
}