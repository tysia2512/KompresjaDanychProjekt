#include "huffman.cpp"
#include <fstream>

CODE encode(const std::string& filename, int k, bool cmb, bool eo) {
    std::ifstream dataCodes;
    dataCodes.open(filename);
    //dodatkowy argument
    HuffmanCodes hc(dataCodes, k, cmb, eo);
    dataCodes.close();

    std::ifstream data;
    data.open(filename);
    boost::tuple<CODE, long long> encoded = hc.encode(data);
    return encoded.get<0>();
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "Należy podać parametry\n";
    } else {
        std::string filename = argv[1];
        int k = 1;
        bool cmb = false;
        bool eo = false;
        if (argc >= 2) {
            k = atoi(argv[2]);
        }
        if (argc >= 3) {
            cmb = atoi(argv[3]);
        }
        if (argc >= 4) {
            eo = atoi(argv[4]);
        }
        std::cout << encode("testy/" + filename, k, cmb, eo);
        std::cout << "\n";
    }
}