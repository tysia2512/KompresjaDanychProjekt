#include <iostream>
#include <algorithm>
#include <unordered_map>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

typedef std::string LETTER;
typedef std::string CODE;

class HuffmanNode {
private:
    const boost::optional<LETTER> letter;
    HuffmanNode *child0, *child1;

    void getCodes(std::unordered_map<LETTER, CODE> &codes, const CODE &prefix) const {
        // Zawsze jest dwoje lub 0 dzieci
        if (child0 != NULL && child1 != NULL) {
            child0->getCodes(codes, prefix + "0");
            child1->getCodes(codes, prefix + "1");
        } else {
            codes[letter.value()] = prefix;
        }
    }
public:
    HuffmanNode() {}
    HuffmanNode(const LETTER &letter) 
    : letter(letter) ,child0(nullptr), child1(nullptr) {}
    HuffmanNode(HuffmanNode *child0, HuffmanNode *child1) 
    : child0(child0), child1(child1), letter(boost::none) {}

    static void destroyNode(HuffmanNode* node) {
        if (node != NULL) {
            destroyNode(node->child0);
            destroyNode(node->child1);
            // std::cout << node << "\n";
            delete node;
        }
    }

    // ~HuffmanNode() {
    //     if (this != NULL) {
    //         destroyNode(child0);
    //         destroyNode(child1);
    //     }
    // }

    std::unordered_map<LETTER, CODE> getCodes() const {
        std::unordered_map<LETTER, CODE> codes;
        getCodes(codes, "");
        std::cout << "codes done\n";
        return codes;
    }

    void debug() {
        std::cout << "TREE: " << child0 << " " << child1 << " " << letter << "\n";
    }
};
