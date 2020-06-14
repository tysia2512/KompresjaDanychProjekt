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
public:
    DISTRIBUTION get_distribution() const {
        return distr;
    }
    
    Distribution() {}
    Distribution(DATA &data, int k) {
        std::unordered_map<LETTER, long long> freq;
        LETTER letter = "";
        for (int i = 0; i < k; i++) {
            if (!data.eof()) {
                letter += char(data.get());
            }
        }
        freq[letter]++;
        while(!data.eof()) {
            letter += char(data.get());
            letter.erase(0, 1);
            freq[letter]++;
        }
        long long max_freq = 0;
        for(std::unordered_map<LETTER, long long>::iterator it = freq.begin(); it != freq.end(); it++) {
            distr.push_back(boost::make_tuple(it->first, it->second));
            if (it->second > max_freq)
                max_freq = it->second;
        }
        std::cout << distr.size() << " letters in simple distribution\n";
        std::cout << "Most often appearing letter appeared " << max_freq << " times\n";
    }

    void debug() {
        std::cout << "DISTRIBUTION DEBUG\n";
        for (int i = 0; i < distr.size(); i++) {
            std::cout << distr[i].get<0>() << " " << distr[i].get<1>() << "\n";
        }
    }
};

class CombinedLettersDistribution : public Distribution {
private:
    void gen_samples(
        const DISTRIBUTION &basic_distr, 
        long long freq,
        LETTER l, 
        int k) {
        if (freq <= 0)
            std::cout << "PODEJRZANE!: " << l << " " << freq << "\n";
        if (k == 0) {
            distr.push_back(boost::make_tuple(l, freq));
            return;
        }
        for (int i = 0; i < basic_distr.size(); i++) {
            gen_samples(
                basic_distr, 
                freq * basic_distr[i].get<1>(), 
                l + basic_distr[i].get<0>(), k-1);
        }
    }
public:
    CombinedLettersDistribution(DATA &data, int k) {
        Distribution simple_distr = Distribution(data, 1);
        std::cout << "Combining " << simple_distr.get_distribution().size() << " letters\n";
        gen_samples(simple_distr.get_distribution(), 1, "", k);
        std::cout << "Resulted in " << distr.size() << " letters\n";
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

    boost::tuple<CODE, long long> encode(DATA& data) const {
        long long length = 0;
        CODE code = "";
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
                if (letter_codes.find(l) == letter_codes.end()) {
                    std::cout << "Letter " << l << " not found\n";
                    std::cout << (int)l[0] << "\n";
                    return boost::make_tuple("", 0);
                }
                code += letter_codes.find(l)->second;
                length += k;
            }
        }
        std::cout << "Letter codes' number: " << letter_codes.size() << "\n";
        double estimated_length = length * log2(letter_codes.size());
        return boost::make_tuple(code, (long long)estimated_length);
    }
};

