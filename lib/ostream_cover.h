
#ifndef HUFFMAN_OSTREAM_COVER_H
#define HUFFMAN_OSTREAM_COVER_H

#include <cstdint>
#include <iostream>
#include <limits>

struct ostream_cover {
    typedef unsigned char byte_t;
    typedef uint64_t code_t;
    const uint8_t bits_in_code = 64;

    explicit ostream_cover(std::ostream &ostream);

    void flush_buffer();

    void write_bits(code_t bits, uint8_t count);

    ~ostream_cover();

private:

    std::array<byte_t, 1u << 16u> buffer{};
    uint8_t pos_in_byte{};
    size_t size{};
    std::ostream &ostream;

};


#endif //HUFFMAN_OSTREAM_COVER_H
