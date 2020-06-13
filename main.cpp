#include "huffman.cpp"
#include <fstream>

const std::vector<std::string> filenames{"text1"};
const std::vector<int> ks{1, 2, 3, 4, 5, 6, 7};
const std::vector<bool> combined{false, true};

int main() {
    for (const std::string &file : filenames) {
        for (const int &k : ks) {
            for (const bool &cmb : combined) {
                std::cout << file << " " << k << " " << cmb << "\n";
                if (k == 1 && cmb == true) {
                    continue;
                }
                
                std::ifstream dataCodes;
                dataCodes.open(file);
                HuffmanCodes hc(dataCodes, k, cmb);
                // hc.debugCodes();
                dataCodes.close();

                std::ifstream data;
                data.open(file);
                boost::tuple<CODE, int> encoded = hc.encode(data);
                std::cout << encoded.get<0>().length() << " " << encoded.get<1>() << "\n";
                data.close();
                std::cout << "data closed\n";
            }
        }
    }
}