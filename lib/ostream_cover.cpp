//
// Created by Елена on 03.10.18.
//

#include <array>
#include "ostream_cover.h"


ostream_cover::ostream_cover(std::ostream &ostream) : ostream(ostream) {}

void ostream_cover::flush_buffer() {
    static_assert(sizeof(char) == sizeof(byte_t));
    ostream.write(reinterpret_cast<const char *>(buffer.data()), size + (bit_pos != 0));
    size = bit_pos = 0;
    buffer.fill(0);
}

void ostream_cover::write_bits(code_t bits, uint8_t count) {
    //assert(count <= bits_in_code);
    bits <<= bits_in_code - count;
    for (uint8_t i = 0; i < count; ++i) {
        decltype(bits) mask = (code_t(1) << (bits_in_code - 1 - i));
        buffer[size] |= (byte_t((bits & mask) != 0)) << (8 - 1 - bit_pos);
        ++bit_pos;
        if (bit_pos == 8) {
            bit_pos = 0;
            ++size;
            if (size == buffer.size()) {
                flush_buffer();
            }
        }
    }
}

ostream_cover::~ostream_cover() {
    flush_buffer();
}

