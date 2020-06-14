#include "huffman.cpp"
#include <fstream>

struct Test {
    std::string filename;
    int maxk;
    const std::vector<bool> combined{false, true};

    Test(const std::string& fn, int maxk) : filename(fn), maxk(maxk) {}
};

const std::vector<Test> tests{
    Test("text1", 2), 
    Test("text2", 2),
    Test("strona1", 2),
    Test("download.jpeg", 3)
};

int main() {
    for (const Test &test : tests) {
        for (int k = 1; k <= test.maxk; k++) {
            for (const bool &cmb : test.combined) {
                const std::string file = test.filename;
                if (k == 1 && cmb == true) {
                    continue;
                }
                std::cout << "Test: " << file << ", k: " << k << ", cmb: " << cmb << "\n";
                
                std::ifstream dataCodes;
                dataCodes.open(file);
                HuffmanCodes hc(dataCodes, k, cmb);
                dataCodes.close();

                std::ifstream data;
                data.open(file);
                boost::tuple<CODE, long long> encoded = hc.encode(data);

                long long encoded_l = encoded.get<0>().length();
                long long estimated_original_l = encoded.get<1>();
                std::cout << "Encoded: " << encoded_l << " vs. estimated original length: " << estimated_original_l << "\n";
                double ratio = (double)encoded_l / (double)estimated_original_l;
                std::cout << "Ratio: " << ratio << "\n";
                data.close();
            }
        }
    }
}