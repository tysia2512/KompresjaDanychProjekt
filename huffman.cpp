#include "huffman_node.cpp"

#include <vector>
#include <queue>
#include <algorithm>

#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/optional/optional_io.hpp>

// #include <fstream>

// typedef std::string LETTER;
// typedef std::string CODE;
typedef std::istream DATA;
typedef boost::tuple<LETTER, int> DISTR_SAMPLE;
typedef std::vector<DISTR_SAMPLE> DISTRIBUTION;

// class HuffmanNode {
// private:
//     LETTER letter;
//     HuffmanNode *child0, *child1;

//     void getCodes(std::unordered_map<LETTER, CODE> &codes, const CODE &prefix) const {
//         // std::cout << (bool)child0 << " " << (bool)child1 << "letter: " << letter  << "\n";
//         // std::cout << child0 << " " << child1 << "\n";
//         std::cout << prefix << "\n";
//         // std::cout << child0.get() << " " << child1.get() << "\n";
//         if (child0 && child1) {
//             std::cout << child0 << " " << child1 << "\n";
//             // std::cout << (int)(child0.value()) << " " << (int)(child1.value()) << "\n";
//             child0->getCodes(codes, prefix + "0");
//             child1->getCodes(codes, prefix + "1");
//         } else {
//             // std::cout << "leaf with " << letter;
//             std::cout << "has code " << prefix << "\n";
//             // codes[letter] = prefix;
//         }
//     }

// public:
//     HuffmanNode() {}
//     HuffmanNode(const LETTER &letter) 
//     : letter(letter) ,child0(nullptr), child1(nullptr) {}
//     // { std::cout << (bool)child0 <<" " << (bool)child1 << "\n"; }
//     HuffmanNode(const HuffmanNode &child0, const HuffmanNode &child1)
//     : child0(&child0), child1(&child1), letter("") {}

//     std::unordered_map<LETTER, CODE> getCodes() const {
//         std::unordered_map<LETTER, CODE> codes;
//         getCodes(codes, "");
//         std::cout << "codes done\n";
//         return codes;
//     }

//     void debug() {
//         std::cout << "TREE: " << (bool)(child0) << " " << (bool)child1 << " " << letter << "\n";
//     }
// };

class HuffmanQueuedTree {
private:
    HuffmanNode* tree;
    int frequency;
    int time_created;
public:
    HuffmanQueuedTree(const LETTER &l, int f, int tc) 
        : tree(new HuffmanNode(l)), frequency(f), time_created(tc) {}
    HuffmanQueuedTree(const LETTER &l, int f) 
        : tree(new HuffmanNode(l)), frequency(f) {}
    HuffmanQueuedTree(const DISTR_SAMPLE &s, int tc) 
        : tree(new HuffmanNode(s.get<0>())), frequency(s.get<1>()), time_created(tc) 
        {}
    HuffmanQueuedTree(
        HuffmanNode *tree, 
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

    HuffmanNode* getTree() const {
        return tree;
    }

    static HuffmanQueuedTree merge (
        const HuffmanQueuedTree &t1, 
        const HuffmanQueuedTree &t2, 
        int time) {

        HuffmanNode* tree = new HuffmanNode(t1.tree, t2.tree);

        return HuffmanQueuedTree(tree, t1.frequency + t2.frequency, time);
        // return res;
    }

    void debug() {
        std::cout << frequency << " " << time_created << "\n";
        tree->debug();
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
    HuffmanNode* tree;
    std::unordered_map<LETTER, CODE> letter_codes;
    const int k;
    const bool combine_letters;

    static HuffmanNode* buildTree (DATA& data, int k, bool cl) {
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
            t1.debug(); t2.debug();
            q.push(HuffmanQueuedTree::merge(t1, t2, time));
            // DEBUG
            // std::cout << q.size() << "\n";
        }
        return q.top().getTree();
    }

public:
    HuffmanCodes(DATA& data, int k, bool cl) 
    : k(k), combine_letters(cl), tree(buildTree(data, k, cl)) {
        letter_codes = tree->getCodes();
        std::cout << "codes done\n";
    }
    ~HuffmanCodes() {
        delete tree;
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

// int main() {}
