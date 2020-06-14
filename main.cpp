#include "huffman.cpp"
#include <fstream>

struct Test {
    std::string filename;
    int maxk;
    bool combine;

    Test(const std::string& fn, int maxk, bool cmb) : filename(fn), maxk(maxk), combine(cmb) {}
        
    Test(const std::string& fn, int maxk) : filename(fn), maxk(maxk), combine(false) {}
};

const std::vector<Test> tests{
    Test("king_lear", 5), 
    Test("king_lear", 3, true), 
    Test("king_lear_lc", 6), 
    Test("king_lear_lc", 3, true), 
    Test("strona1", 5),
    Test("strona1", 2, true),
    Test("download.jpeg", 5),
    Test("download.jpeg", 2, true),
};

int main() {
    for (const Test &test : tests) {
        for (int k = 1; k <= test.maxk; k++) {

            const std::string file = test.filename;
            if (k == 1 && test.combine == true) {
                continue;
            }
            std::cout << "Test: " << file << ", k: " << k << ", cmb: " << test.combine << "\n";
            
            std::ifstream dataCodes;
            dataCodes.open(file);
            HuffmanCodes hc(dataCodes, k, test.combine);
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