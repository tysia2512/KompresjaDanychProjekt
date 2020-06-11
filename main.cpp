#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
// #include <boost/tuple/tuple_io.hpp>

typedef std::string LETTER;
typedef std::string CODE;
typedef std::istream DATA;
typedef boost::tuple<LETTER, int> DISTR_SAMPLE;
typedef std::vector<DISTR_SAMPLE> DISTRIBUTION;

class HuffmanNode {
private:
    LETTER letter;
    boost::optional<HuffmanNode&> child0, child1;

public:
    HuffmanNode();
    HuffmanNode(const LETTER &letter);
    HuffmanNode(const HuffmanNode &child0, const HuffmanNode &child1);
};

class HuffmanQueuedTree {
private:
    const HuffmanNode tree;
    int frequency;
    int time_created;
public:
    HuffmanQueuedTree(const LETTER &l, int f, int tc) 
        : tree(HuffmanNode(l)), frequency(f), time_created(tc) {}
    HuffmanQueuedTree(const LETTER &l, int f) 
        : tree(HuffmanNode(l)), frequency(f) {}
    HuffmanQueuedTree(
        const HuffmanNode &tree, 
        const int frequency, 
        const int time_created);

    bool operator <(const HuffmanQueuedTree &hqt) const {
		return frequency < hqt.frequency 
            || (frequency == hqt.frequency && time_created < hqt.time_created);
	}
    bool operator >(const HuffmanQueuedTree &hqt) const {
        return frequency > hqt.frequency 
            || (frequency == hqt.frequency && time_created > hqt.time_created);
    }

    static HuffmanQueuedTree merge (
        const HuffmanQueuedTree &t1, 
        const HuffmanQueuedTree &t2, 
        int time) {

        HuffmanNode tree = HuffmanNode(t1.tree, t2.tree);

        return HuffmanQueuedTree(tree, t1.frequency + t2.frequency, time);
    }
};

class Distribution {
protected:
    DISTRIBUTION distr;
public:
    DISTRIBUTION get_distribution() const {
        return distr;
    }
    
    Distribution();
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
            freq[letter]++;
        }
        for(std::unordered_map<LETTER,int>::iterator it = freq.begin(); it != freq.end(); ++it) {
            distr.push_back(boost::make_tuple(it->first, it->second));
        }
    }
};

// O(A^k)
class CombinedLettersDistribution : Distribution {
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
    HuffmanNode tree;
    std::unordered_map<LETTER, CODE> letter_codes;

public:
    HuffmanCodes(const Distribution &d)
    {
        HuffmanNode tree = HuffmanNode();

        // auto cmp = [](boost::tuple<LETTER, int, int> left, boost::tuple<LETTER, int, int> right) { 
        //     return (left ^ 1) < (right ^ 1); 
        // };
        std::priority_queue<boost::tuple<LETTER, int, int>> q;
        for (const DISTR_SAMPLE &l : d.get_distribution()) {
        }
    }
};

class Huffman {
};

int main() {
}