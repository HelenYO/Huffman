#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <queue>
#include <functional>
#include <memory>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cstring>

#include "huffman.h"
#include "istream_cover.h"
#include "ostream_cover.h"


typedef unsigned char byte_t;
typedef uint64_t code_t;

constexpr size_t ALPHABET_SIZE = 256;

struct symbol {
    byte_t byte;
    size_t count;
    code_t code;
    uint8_t bits;

    symbol(byte_t byte, size_t count, code_t code, uint8_t bits) : byte(byte), count(count), code(code),
                                                                   bits(bits) {}
};

std::vector<symbol> build_symbols_info(istream_cover &istream) {
    std::vector<symbol> symbols_info;
    symbols_info.reserve(ALPHABET_SIZE);
    for (size_t i = 0; i < ALPHABET_SIZE; ++i) {
        symbols_info.emplace_back(i, 0, 0, 0);
    }
    while (istream.has_more()) {
        ++symbols_info[istream.read_8_bits()].count;
    }
    istream.rewind();
    return symbols_info;
}

struct nodeEn {
    byte_t byte = 0;
    size_t count = 0;
    std::shared_ptr<nodeEn> left = nullptr;
    std::shared_ptr<nodeEn> right = nullptr;

    nodeEn() = default;

    nodeEn(byte_t byte, size_t count) : byte(byte), count(count) {}

    nodeEn(nodeEn left, nodeEn right) : count(left.count + right.count),
                                        left(std::make_shared<nodeEn>(left)),
                                        right(std::make_shared<nodeEn>(right)) {}
};

void write_tree(nodeEn const &node, code_t code, uint8_t bits, std::vector<symbol> &symbols_info,
                ostream_cover &ostream) {
    if (!node.left && !node.right) {
        ostream.write_bits(1, 1);
        ostream.write_bits(node.byte, 8);
        symbols_info[node.byte].code = code;
        symbols_info[node.byte].bits = bits;
        return;
    } else {
        ostream.write_bits(0, 1);
    }
    ++bits;
    write_tree(*node.left, (code << 1) | 0, bits, symbols_info, ostream);
    write_tree(*node.right, (code << 1) | 1, bits, symbols_info, ostream);
}

void make_tree(std::vector<symbol> &symbols_info, ostream_cover &ostream) {
    std::function<bool(nodeEn, nodeEn)> comparator = [](nodeEn const &lhs, nodeEn const &rhs) {
        return lhs.count > rhs.count;
    };
    std::priority_queue<nodeEn, std::vector<nodeEn>, decltype(comparator)> queue(comparator);
    for (auto const &symbol : symbols_info) {
        if (symbol.count > 0) {
            queue.emplace(symbol.byte, symbol.count);
        }
    }
    while (queue.size() != 1) {
        auto lhs = queue.top();
        queue.pop();
        auto rhs = queue.top();
        queue.pop();
        queue.emplace(lhs, rhs);
    }
    auto root = queue.top();
    write_tree(root, 0, !root.left && !root.right, symbols_info, ostream);
}

struct nodeDe {
    byte_t byte;
    std::shared_ptr<nodeDe> left;
    std::shared_ptr<nodeDe> right;

    explicit nodeDe(byte_t byte) : byte(byte), left(nullptr), right(nullptr) {}

    nodeDe(nodeDe left, nodeDe right) : byte(0),
                                        left(std::make_shared<nodeDe>(left)),
                                        right(std::make_shared<nodeDe>(right)) {}
};

nodeDe build_again_tree(istream_cover &istream) {
    if (istream.read_bit()) {
        return nodeDe(istream.read_8_bits());
    }
    nodeDe left = build_again_tree(istream);
    nodeDe right = build_again_tree(istream);
    return nodeDe(left, right);
}

void compress(std::istream &in, std::ostream &out) {
    istream_cover istream(in);
    auto symbols_info = build_symbols_info(istream);
    size_t file_size = 0;
    for (auto const &symbol : symbols_info) {
        file_size += symbol.count;
    }
    if (file_size == 0) {
        return;
    }
    char *tmpbytes = reinterpret_cast<char *>(&file_size);
    char bytes[sizeof(size_t)];
    std::strcpy(bytes, tmpbytes);
    for (char byte : bytes) {
        out.put(byte);
    }
    ostream_cover ostream(out);
    make_tree(symbols_info, ostream);
    while (istream.has_more()) {
        byte_t byte = istream.read_8_bits();
        ostream.write_bits(symbols_info[byte].code, symbols_info[byte].bits);
    }
}

void decompress(std::istream &in, std::ostream &out) {
    size_t file_size = 0;
    auto *tmpbytes = reinterpret_cast<char *>(&file_size);
    char bytes[sizeof(size_t)];
    std::strcpy(bytes, tmpbytes);
    for (char &byte : bytes) {
        int tmp = in.get();
        //if (tmp == std::char_traits<char>::eof()) {
        if (tmp == std::istream::traits_type::eof()) {
            return;
        } else {
            byte = static_cast<char>(tmp);
        }
    }
    std::strcpy(tmpbytes, bytes);
    if (file_size == 0) {
        return;
    }
    istream_cover istream(in);
    auto root = build_again_tree(istream);
    for (size_t i = 0; i < file_size; ++i) {
        auto *node = &root;
        while (!(!node->left && !node->right)) {
            if (!istream.read_bit()) {
                node = node->left.get();
            } else {
                node = node->right.get();
            }
        }
        out.put(node->byte);
    }
}