#include "huffman.cpp"
#include <fstream>

struct Test {
    std::string filename;
    int maxk;
    bool combine;
    bool existing_only;

    Test(const std::string& fn, int maxk, bool cmb=false, bool eo=false) 
    : filename(fn), maxk(maxk), combine(cmb), existing_only(eo) {}
    
};

const std::vector<Test> all_tests{
    Test("king_lear", 5), 
    Test("king_lear", 3, true), 
    Test("king_lear_lc", 6), 
    Test("king_lear_lc", 3, true), 
    Test("strona1", 5),
    Test("strona1", 2, true),
    Test("download.jpeg", 5),
    Test("download.jpeg", 2, true),
};

void run_test(const Test& test) {
    for (int k = 1; k <= test.maxk; k++) {
        const std::string file = test.filename;
        if (k == 1 && test.combine == true) {
            continue;
        }
        std::cout << "Test: " << file 
            << ", długość bloku: " << k 
            << ", łączone litery: " << test.combine 
            << ", tylko istniejące ciągi: " << test.existing_only << "\n";
        
        std::ifstream dataCodes;
        dataCodes.open(file);
        //dodatkowy argument
        HuffmanCodes hc(dataCodes, k, test.combine, test.existing_only);
        dataCodes.close();

        std::ifstream data;
        data.open(file);
        boost::tuple<CODE, long long> encoded = hc.encode(data);

        long long encoded_l = encoded.get<0>().length();
        long long estimated_original_l = encoded.get<1>();
        std::cout << "Długość zakodowanego tekstu: " << encoded_l << "\nSzacowana długość oryginału: " << estimated_original_l << "\n";
        double ratio = (double)encoded_l / (double)estimated_original_l;
        std::cout << "Stosunek: " << ratio << "\n";
        data.close();
    }
}

void run_all_tests() {
    for (const Test &test : all_tests) {
       run_test(test);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        run_all_tests();
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
        run_test(Test(filename, k, cmb, eo));
    }
}