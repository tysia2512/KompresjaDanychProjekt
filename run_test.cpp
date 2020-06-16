#include "test.cpp"

const std::vector<Test> all_tests{
    // Test("king_lear", 5), 
    // Test("king_lear", 3, true), 
    // Test("king_lear_lc", 6), 
    // Test("king_lear_lc", 4, true), 
    // Test("strona1", 5),
    // Test("strona1", 2, true),
    // Test("strona1", 2, true, true),
    Test("pies.jpeg", 5),
    Test("pies.jpeg", 5, true, true),
    Test("pies.jpeg", 3, true),
};

const std::vector<Test> chart1{
    Test("pies.jpeg", 6),
    Test("pies.jpeg", 6, true, true),
    Test("pies.jpeg", 3, true),
};

const std::vector<Test> tests{
    Test("king_lear", 10, true, true),
    Test("king_lear_lc", 10, true, true),
    Test("pies.jpeg", 10, true, true),
    Test("skos", 10, true, true),
};

void run_all_tests() {
    for (const Test &test : tests) {
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