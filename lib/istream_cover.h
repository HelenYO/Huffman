//
// Created by Елена on 03.10.18.
//

#ifndef HUFFMAN_ISTREAM_COVER_H
#define HUFFMAN_ISTREAM_COVER_H


#include <istream>
#include <limits>

struct istream_cover {
    typedef unsigned char byte_t;

    explicit istream_cover(std::istream &istream);

    void check_buffer();

    bool read_bit();

    uint8_t read_8_bits();

    bool has_more();

    void rewind();

private:
    std::array<byte_t, 1u << 16u> buffer{};
    uint8_t bit_pos = 0;
    size_t pos = 0;
    size_t size = 0;
    std::istream &istream;
};


#endif //HUFFMAN_ISTREAM_COVER_H
