
#include <array>
#include "istream_cover.h"


typedef unsigned char byte_t;


istream_cover::istream_cover(std::istream &istream) : istream(istream) {
    check_buffer();
}

void istream_cover::check_buffer() {
    size_t shift = size - pos;
    if (shift < 2) {
        for (size_t i = 0; i < shift; ++i) {
            buffer[i] = buffer[pos + i];
        }
        //static_assert(sizeof(char) == sizeof(byte_t));
        istream.read(reinterpret_cast<char *>(buffer.data() + shift), buffer.size() - shift);
        size = shift + istream.gcount();
        pos = 0;
    }
}

bool istream_cover::read_bit() {
    //assert(pos < size);
    bool bit = (buffer[pos] & (1 << (8 - 1 - bit_pos))) != 0;
    ++bit_pos;
    if (bit_pos == 8) {
        bit_pos = 0;
        ++pos;
        check_buffer();
    }
    return bit;
};

uint8_t istream_cover::read_8_bits() {
    //assert(pos < size);
    uint8_t bits = (buffer[pos] << bit_pos) | (buffer[pos + 1] >> (8 - bit_pos));
    ++pos;
    check_buffer();
    return bits;
}

bool istream_cover::has_more() {
    return !istream.fail() || pos < size;
}

void istream_cover::rewind() {
    istream.clear();
    istream.seekg(std::ios::beg);
    pos = size = bit_pos = 0;
    check_buffer();
}
