#include "huffman_node.cpp"

#include <vector>
#include <queue>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

typedef std::istream DATA;
typedef boost::tuple<LETTER, int> DISTR_SAMPLE;
typedef std::vector<DISTR_SAMPLE> DISTRIBUTION;

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

    bool operator >(const HuffmanQueuedTree &hqt) const {
		return frequency < hqt.frequency 
            || (frequency == hqt.frequency && time_created < hqt.time_created);
	}
    bool operator <(const HuffmanQueuedTree &hqt) const {
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

class CombinedLettersDistribution : public Distribution {
private:
    void gen_samples(
        const DISTRIBUTION &basic_distr, 
        // TODO: moga byc potrzebne jakies bignumy
        long long freq,
        LETTER l, 
        int k) {
        if (k == 0) {
            distr.push_back(boost::make_tuple(l, freq));
            return;
        }
        for (int i = 0; i < basic_distr.size(); i++) {
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

        int time = 0;
        std::priority_queue<HuffmanQueuedTree> q;
        for (const DISTR_SAMPLE &l : d.get_distribution()) {
            HuffmanQueuedTree leaf(l, time);
            q.push(leaf);
        }
        while (q.size() > 1) {
            time++;
            HuffmanQueuedTree t1 = q.top();
            q.pop();
            HuffmanQueuedTree t2 = q.top();
            q.pop();
            q.push(HuffmanQueuedTree::merge(t1, t2, time));
        }
        return q.top().getTree();
    }

public:
    HuffmanCodes(DATA& data, int k, bool cl) 
    : k(k), combine_letters(cl), tree(buildTree(data, k, cl)) {
        letter_codes = tree->getCodes();
    }
    ~HuffmanCodes() {
        HuffmanNode::destroyNode(tree);
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
        return boost::make_tuple(code, length * letter_codes.size());
    }
};

