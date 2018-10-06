
#include <array>
#include "istream_cover.h"


typedef unsigned char byte_t;


istream_cover::istream_cover(std::istream &istream) : istream(istream) {
    check_buffer();
}

void istream_cover::check_buffer() {
    size_t shift = size - pos;
    if (shift < 2) {
        for (size_t i = 0; i < shift; i++) {
            buffer[i] = buffer[pos + i];
        }
        istream.read(reinterpret_cast<char *>(buffer.data() + shift), buffer.size() - shift);
        size = shift + istream.gcount();
        pos = 0;
    }
}

bool istream_cover::read_bit() {
    bool bit = (buffer[pos] & (1 << (8 - 1 - pos_in_byte))) != 0;
    pos_in_byte++;
    if (pos_in_byte == 8) {
        pos_in_byte = 0;
        pos++;
        check_buffer();
    }
    return bit;
};

uint8_t istream_cover::read_8_bits() {
    uint8_t bits = (buffer[pos] << pos_in_byte) | (buffer[pos + 1] >> (8 - pos_in_byte));
    pos++;
    check_buffer();
    return bits;
}

bool istream_cover::has_more() {
    //return !istream.fail() || pos < size;
    return (pos < size);
}

void istream_cover::restart() {
    istream.clear();
    istream.seekg(std::ios::beg);
    pos = size = pos_in_byte = 0;
    check_buffer();
}
