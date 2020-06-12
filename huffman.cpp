// #include "huffman.h"
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>

#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
//DEBUG
#include <boost/tuple/tuple_io.hpp>
#include <boost/optional/optional_io.hpp>

// #include <fstream>

typedef std::string LETTER;
typedef std::string CODE;
typedef std::istream DATA;
typedef boost::tuple<LETTER, int> DISTR_SAMPLE;
typedef std::vector<DISTR_SAMPLE> DISTRIBUTION;

class HuffmanNode {
private:
    LETTER letter;
    const boost::optional<const HuffmanNode&> child0, child1;

    void getCodes(std::unordered_map<LETTER, CODE> &codes, const CODE &prefix) const {
        // std::cout << (bool)child0 << " " << (bool)child1 << "\n";
        if (child0 && child1) {
            child0->getCodes(codes, prefix + "0");
            child1->getCodes(codes, prefix + "1");
        } else {
            codes[letter] = prefix;
        }
    }

public:
    HuffmanNode() {}
    HuffmanNode(const LETTER &letter) 
    : letter(letter) ,child0(boost::none), child1(boost::none) 
    { std::cout << (bool)child0 <<" " << (bool)child1 << "\n"; }
    HuffmanNode(const HuffmanNode &child0, const HuffmanNode &child1)
    : child0(child0), child1(child1) {}

    std::unordered_map<LETTER, CODE> getCodes() const {
        std::unordered_map<LETTER, CODE> codes;
        getCodes(codes, "");
        return codes;
    }
};

class HuffmanQueuedTree {
private:
    HuffmanNode tree;
    int frequency;
    int time_created;
public:
    HuffmanQueuedTree(const LETTER &l, int f, int tc) 
        : tree(HuffmanNode(l)), frequency(f), time_created(tc) {}
    HuffmanQueuedTree(const LETTER &l, int f) 
        : tree(HuffmanNode(l)), frequency(f) {}
    HuffmanQueuedTree(const DISTR_SAMPLE &s, int tc) 
        : tree(HuffmanNode(s.get<0>())), frequency(s.get<1>()), time_created(tc) 
        {}
    HuffmanQueuedTree(
        const HuffmanNode &tree, 
        const int frequency, 
        const int time_created)
        : tree(tree), frequency(frequency), time_created(time_created) {}

    bool operator <(const HuffmanQueuedTree &hqt) const {
		return frequency < hqt.frequency 
            || (frequency == hqt.frequency && time_created < hqt.time_created);
	}
    bool operator >(const HuffmanQueuedTree &hqt) const {
        return frequency > hqt.frequency 
            || (frequency == hqt.frequency && time_created > hqt.time_created);
    }

    const HuffmanNode getTree() const {
        return tree;
    }

    static HuffmanQueuedTree merge (
        const HuffmanQueuedTree &t1, 
        const HuffmanQueuedTree &t2, 
        int time) {

        HuffmanNode tree(t1.tree, t2.tree);

        HuffmanQueuedTree res(tree, t1.frequency + t2.frequency, time);
        return res;
    }
};

class Distribution {
protected:
    DISTRIBUTION distr;
public:
    DISTRIBUTION get_distribution() const {
        return distr;
    }
    
    Distribution() {}
    // O(k*n) - erase chyba jest liniowe
    Distribution(DATA &data, int k) {
        std::unordered_map<LETTER, int> freq;
        LETTER letter = "";
        for (int i = 0; i < k; i++) {
            if (data) {
                letter += char(data.get());
            }
        }
        freq[letter]++;
        while(data) {
            letter += char(data.get());
            letter.erase(0, 1);
            // DEBUG
            // std::cout << letter << "\n";
            freq[letter]++;
        }
        for(std::unordered_map<LETTER,int>::iterator it = freq.begin(); it != freq.end(); ++it) {
            distr.push_back(boost::make_tuple(it->first, it->second));
        }
        // DEBUG
        // for (const auto &d : distr) {
        //     std::cout << d << "\n";
        // }
    }
};

// O(A^k)
class CombinedLettersDistribution : public Distribution {
private:
    void gen_samples(
        const DISTRIBUTION &basic_distr, 
        // TODO: moga byc potrzebne jakies bignumy
        long long freq,
        LETTER l, 
        int k) {
        if (k == 0)
            distr.push_back(boost::make_tuple(l, freq));
        for (int i = 0; i < basic_distr.size(); i++ ) {
            gen_samples(
                basic_distr, 
                freq *= basic_distr[i].get<1>(), 
                l + basic_distr[i].get<0>(), k-1);
        }
    }
public:
    CombinedLettersDistribution(DATA &data, int k) {
        Distribution simple_distr = Distribution(data, 1);
        gen_samples(simple_distr.get_distribution(), 1, "", k);
    }
};

class HuffmanCodes {
private:
    const HuffmanNode tree;
    std::unordered_map<LETTER, CODE> letter_codes;
    const int k;
    const bool combine_letters;

    static const HuffmanNode buildTree (DATA& data, int k, bool cl) {
        Distribution d;
        if (cl) {
            d = CombinedLettersDistribution(data, k);
        } else {
            d = Distribution(data, k);
        }
        // HuffmanNode tree = HuffmanNode();

        int time = 0;
        // TODO: komparator chyba w zla strone
        std::priority_queue<HuffmanQueuedTree> q;
        for (const DISTR_SAMPLE &l : d.get_distribution()) {
            HuffmanQueuedTree leaf(l, time);
            q.push(leaf);
        }
        // TODO: co jesli zaczynamy od jednego wierzcholka
        while (q.size() > 1) {
            time++;
            HuffmanQueuedTree t1 = q.top();
            q.pop();
            HuffmanQueuedTree t2 = q.top();
            q.pop();
            q.push(HuffmanQueuedTree::merge(t1, t2, time));
            // DEBUG
            // std::cout << q.size() << "\n";
        }
        return q.top().getTree();
    }

public:
    HuffmanCodes(DATA& data, int k, bool cl) 
    : k(k), combine_letters(cl), tree(buildTree(data, k, cl)) {
        letter_codes = tree.getCodes();
        std::cout << "codes done\n";
    }

    boost::tuple<CODE, int> encode(DATA& data) const {
        int length = 0;
        CODE code = "";
        while (data) {
            LETTER l = "";
            for (int i = 0; i < k; i++) {
                if (data) {
                    l += char(data.get());
                } else {
                    break;
                }
            }
            if (l.size() == k) {
                code += letter_codes.find(l)->second;
                length += k;
            }
        }
        return boost::make_tuple(code, length);
    }
};

// const std::vector<std::string> filenames{"text1"};
// const std::vector<int> ks{1, 2, 3, 4, 5, 6, 7};
// const std::vector<bool> combined{false, true};

// int main() {
//     for (const std::string &file : filenames) {
//         for (const int &k : ks) {
//             for (const bool &cmb : combined) {
//                 if (k == 1 && cmb == true) {
//                     continue;
//                 }
                
//                 std::ifstream data;
//                 data.open(file);
//                 HuffmanCodes h(data, k, cmb);
//                 data.close();
//                 data.open(file);
//                 boost::tuple<CODE, int> encoded = h.encode(data);
//                 std::cout << encoded.get<0>().length() << " " << encoded.get<1>() << "\n";
//                 data.close();
//             }
//         }
//     }
// }