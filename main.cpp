#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

#include "lib/huffman.h"

int main(int argc, char *argv[]) {
    if (argc != 4 || (strcmp(argv[1], "-c") != 0 && strcmp(argv[1], "-d") != 0)) {
        std::cout << "\nwrite -c/-d and names of files\n";
        return 0;
    }
    std::ifstream fin(argv[2], std::ios::binary);
    if (!fin) {
        std::cout << "\nCan't open inFile\n";
        return -1;
    }
    std::ofstream fout(argv[3], std::ios::binary);
    if (!fout) {
        std::cout << "\nCan't open outFile\n";
        return -1;
    }
    bool encode = std::strcmp(argv[1], "-c") == 0;
    clock_t time = clock();
    if (encode) {
        compress(fin, fout);
    } else {
        try {
            decompress(fin, fout);
        } catch (...) {
            std::cout << "\nsmth wrong in inFile\n";
            return -1;
        }
    }
    double elapsed = static_cast<double>(clock() - time) / CLOCKS_PER_SEC;
    std::cout<< "\nWork is done!\n";
    std::cerr << "Time : " << std::setprecision(3) << elapsed << " seconds" << std::endl;
    return 0;
}