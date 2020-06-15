#include <iostream>
#include <algorithm>
#include <unordered_map>

#include <boost/dynamic_bitset.hpp> 
#include <boost/optional.hpp>

typedef std::string LETTER;
typedef boost::dynamic_bitset<> CODE;

class HuffmanNode {
private:
    const boost::optional<LETTER> letter;
    HuffmanNode *child0, *child1;

    void getCodes(std::unordered_map<LETTER, CODE> &codes, CODE &prefix) const {
        if (child0 != NULL && child1 != NULL) {
            prefix.push_back(0);
            child0->getCodes(codes, prefix);
            prefix.pop_back();

            prefix.push_back(1);
            child1->getCodes(codes, prefix);
            prefix.pop_back();
        } else {
            CODE c = prefix;
            codes[letter.value()] = c;
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
            delete node;
        }
    }

    std::unordered_map<LETTER, CODE> getCodes() const {
        std::unordered_map<LETTER, CODE> codes;
        CODE prefix;
        getCodes(codes, prefix);
        return codes;
    }
};
