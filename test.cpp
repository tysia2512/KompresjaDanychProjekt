#include "huffman.cpp"

#include <fstream>
#include <chrono>


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
            const std::string file = "testy/" + filename;
            if (k == 1 && combine == true) {
                continue;
            }
            std::cout << "Test: " << file 
                << ", długość bloku: " << k 
                << ", łączone litery: " << combine 
                << ", tylko istniejące ciągi: " << existing_only << "\n";
            
            std::chrono::steady_clock::time_point begin = 
                std::chrono::steady_clock::now();
            
            std::ifstream dataCodes;
            dataCodes.open(file);
            HuffmanCodes hc(dataCodes, k, combine, existing_only);
            dataCodes.close();

            std::ifstream data;
            data.open(file);
            boost::tuple<CODE, long long> encoded = hc.encode(data);
            data.close();

            std::chrono::steady_clock::time_point end = 
                std::chrono::steady_clock::now();

            long long encoded_l = encoded.get<0>().size();
            long long estimated_original_l = encoded.get<1>();
            std::cout << "Rozmiar alfabetu: " << hc.size() << "\n";
            std::cout << "Długość zakodowanego tekstu: " << encoded_l << "\nSzacowana długość oryginału: " << estimated_original_l << "\n";
            double ratio = (double)encoded_l / (double)estimated_original_l;
            std::cout << "Stopień kompresji: " << ratio << "\n";
            std::cout << "Czas wykonania: " 
                << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0 
                << "s \n";
            std::cout << "==================================================================\n";
        }
    }


};

