#include "huffman.cpp"
#include <fstream>

struct Test {
private:
    std::string filename;
    int maxk;
    bool combine;
    bool existing_only;

public:
    Test(const std::string& fn, int maxk, bool cmb=false, bool eo=false) 
    : filename(fn), maxk(maxk), combine(cmb), existing_only(eo) {}

    void run() const {
        for (int k = 1; k <= maxk; k++) {
            const std::string file = filename;
            if (k == 1 && combine == true) {
                continue;
            }
            std::cout << "Test: " << file 
                << ", długość bloku: " << k 
                << ", łączone litery: " << combine 
                << ", tylko istniejące ciągi: " << existing_only << "\n";
            
            std::ifstream dataCodes;
            dataCodes.open(file);
            //dodatkowy argument
            HuffmanCodes hc(dataCodes, k, combine, existing_only);
            dataCodes.close();

            std::ifstream data;
            data.open(file);
            boost::tuple<CODE, long long> encoded = hc.encode(data);

            long long encoded_l = encoded.get<0>().size();
            long long estimated_original_l = encoded.get<1>();
            std::cout << "Długość zakodowanego tekstu: " << encoded_l << "\nSzacowana długość oryginału: " << estimated_original_l << "\n";
            double ratio = (double)encoded_l / (double)estimated_original_l;
            std::cout << "Stosunek: " << ratio << "\n";
            data.close();
        }
    }


};

