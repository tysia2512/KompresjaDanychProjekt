#include "huffman_node.cpp"

#include <vector>
#include <queue>
#include <cmath>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

typedef std::istream DATA;
typedef boost::tuple<LETTER, long long> DISTR_SAMPLE;
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
    std::unordered_map<LETTER, long long> singleton_frequency;
public:
    DISTRIBUTION getDistribution() const {
        return distr;
    }

    std::unordered_map<LETTER, long long> getFrequency() const {
        return singleton_frequency;
    }
    
    Distribution() {}
    Distribution(DATA &data, int k) {
        std::unordered_map<LETTER, long long> freq;
        LETTER letter = "";
        for (int i = 0; i < k; i++) {
            if (!data.eof()) {
                letter += char(data.get());
                singleton_frequency["" + letter[letter.size() - 1]]++;
            }
        }
        freq[letter]++;
        while(!data.eof()) {
            letter += char(data.get());
            singleton_frequency["" + letter[letter.size() - 1]]++;
            letter.erase(0, 1);
            freq[letter]++;
        }
        long long max_freq = 0;
        for(std::unordered_map<LETTER, long long>::iterator it = freq.begin(); it != freq.end(); it++) {
            distr.push_back(boost::make_tuple(it->first, it->second));
            if (it->second > max_freq)
                max_freq = it->second;
        }
    }
};

class CombinedLettersDistribution : public Distribution {
private:
    void gen(
        const DISTRIBUTION &basic_distr, 
        long long freq,
        LETTER l, 
        int k) {
        if (k == 0) {
            distr.push_back(boost::make_tuple(l, freq));
            return;
        }
        for (int i = 0; i < basic_distr.size(); i++) {
            gen(
                basic_distr, 
                freq * basic_distr[i].get<1>(), 
                l + basic_distr[i].get<0>(), k-1);
        }
    }

    void genExistingOnly(const Distribution& existing) {
        std::unordered_map<LETTER, long long> frequencies = 
            existing.getFrequency();
        for (const DISTR_SAMPLE &letter : 
            existing.getDistribution()) {
            long long freq = 1;
            for (int i = 0; i < letter.get<0>().size(); i++) {
                freq *= frequencies["" + letter.get<0>()[i]];
            }
            distr.push_back(boost::make_tuple(letter.get<0>(), freq));
        }
    }
public:
    CombinedLettersDistribution(DATA &data, int k, bool only_existing = false) {
        if (only_existing) {
            Distribution d = Distribution(data, k);
            genExistingOnly(d);
        } else {
            Distribution d = Distribution(data, 1);
            gen(d.getDistribution(), 1, "", k);
        }
    }
};

class HuffmanCodes {
private:
    HuffmanNode* tree;
    std::unordered_map<LETTER, CODE> letter_codes;
    const int k;
    const bool combine_letters;
    const bool existing_only;

    static HuffmanNode* buildTree (DATA& data, int k, bool cl, bool eo) {
        Distribution d;
        if (cl) {
            d = CombinedLettersDistribution(data, k, eo);
        } else {
            d = Distribution(data, k);
        }

        int time = 0;
        std::priority_queue<HuffmanQueuedTree> q;
        for (const DISTR_SAMPLE &l : d.getDistribution()) {
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
    HuffmanCodes(DATA& data, int k, bool cl, bool existing_only = false) 
    : k(k), 
    combine_letters(cl), 
    tree(buildTree(data, k, cl, existing_only)), 
    existing_only(existing_only) {
        letter_codes = tree->getCodes();
    }
    ~HuffmanCodes() {
        HuffmanNode::destroyNode(tree);
    }

    boost::tuple<CODE, long long> encode(DATA& data) const {
        long long length = 0;
        CODE code;
        while (!data.eof()) {
            LETTER l = "";
            for (int i = 0; i < k; i++) {
                if (!data.eof()) {
                    l += char(data.get());
                } else {
                    break;
                }
            }
            if (l.size() == k) {
                for (int i = 0; i < letter_codes.find(l)->second.size(); i++) {
                    code.push_back(letter_codes.find(l)->second[i]);
                }
                length += k;
            }
        }
        double estimated_length = length * log2(letter_codes.size());
        return boost::make_tuple(code, (long long)estimated_length);
    }

    int size() {
        return letter_codes.size();
    }
};

