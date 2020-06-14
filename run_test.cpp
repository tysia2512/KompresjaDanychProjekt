#include "test.cpp"

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

void run_all_tests() {
    for (const Test &test : all_tests) {
       test.run();
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
        Test test(filename, k, cmb, eo);
        test.run();
    }
}