#ifndef BASE_HPP   // Start of include guard
#define BASE_HPP

#include <bits/stdc++.h>
#include <string>
using namespace std;

map<pair<int, int>, int> get_stats(
    const vector<int>& tokens, 
    map<pair<int, int>, int> counts = map<pair<int, int>, int>()
    ) {
    
    for (size_t i = 0; i < tokens.size() - 1; ++i) {
        pair<int, int> p = make_pair(tokens[i], tokens[i + 1]);
        counts[p]++;
    }
    return counts;
}

bool sortByVal(const pair<pair<int, int>, int> &a, const pair<pair<int, int>, int> &b) { 
    return (a.second > b.second); 
}

vector<pair<pair<int, int>, int> > sort_stats(map<pair<int, int>, int> stats) {
    vector<pair<pair<int, int>, int> > sorted_stats;

    map<pair<int, int>, int>::iterator it;
    for (it = stats.begin(); it != stats.end(); ++it) {
        sorted_stats.push_back(make_pair(it->first, it->second));
    }

    sort(sorted_stats.begin(), sorted_stats.end(), sortByVal);

    return sorted_stats;
}


vector<int> merge(
    vector<int>& tokens, 
    pair<int, int> pair, 
    int idx
    ) {
    vector<int> new_tokens;
    size_t i = 0;
    
    while (i < tokens.size()) {
        if (i < tokens.size() - 1 && tokens[i] == pair.first && tokens[i + 1] == pair.second) {
            new_tokens.push_back(idx);
            i += 2;
        } else {
            new_tokens.push_back(tokens[i]);
            i++;
        }
    }
    
    return new_tokens;
}

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        // Hash the first element
        size_t hash1 = hash<T1>{}(p.first);
        // Hash the second element
        size_t hash2 = hash<T2>{}(p.second);
        // Combine the two hash values
        return hash1
               ^ (hash2 + 0x9e3779b9 + (hash1 << 6)
                  + (hash1 >> 2));
    }
};


class Tokenizer {
    public:
        Tokenizer() {
            merges = map<pair<int, int>, int>();
            pattern = "";
            special_tokens = unordered_map<string, int>();
            vocab = _build_vocab();
        }

        virtual void train(const string& text, int vocab_size, bool verbose = false){
            throw runtime_error("Not Implemented");
        }

        virtual vector<int> encode(const string& text) {
            throw runtime_error("Not Implemented");
        }

        virtual string decode(const vector<int>& ids) {
            throw runtime_error("Not Implemented");
        }

        void save(const string& file_prefix) {
            string model_file = file_prefix + ".model";
            ofstream model_out(model_file);
            model_out << "minbpe v1\n";
            model_out << pattern << "\n";

            model_out << special_tokens.size() << "\n";
            for (const auto& st : special_tokens) {
                model_out << st.first << " " << st.second << "\n";
            }

            for (const auto& m : merges) {
                model_out << m.first.first << " " << m.first.second << " " << m.second << "\n";
            }
            model_out.close();

            string vocab_file = file_prefix + ".vocab";
            unordered_map<int, pair<int, int>> inverted_merges;
            for (const auto& m : merges) {
                inverted_merges[m.second] = m.first;
            }
            ofstream vocab_out(vocab_file);
            for (const auto& v : vocab) {
                string s = render_token(v.second);
                if (inverted_merges.count(v.first)) {
                    int idx0 = inverted_merges[v.first].first;
                    int idx1 = inverted_merges[v.first].second;
                    string s0 = render_token(vocab[idx0]);
                    string s1 = render_token(vocab[idx1]);
                    vocab_out << "[" << s0 << "][" << s1 << "] -> [" << s << "] " << v.first << "\n";
                } else {
                    vocab_out << "[" << s << "] " << v.first << "\n";
                }
            }
            vocab_out.close();
        }

        void load(const string& model_file) {
            assert(model_file.substr(model_file.find_last_of(".") + 1) == "model");
            map<pair<int, int>, int> merges;
            unordered_map<string, int> special_tokens;
            int idx = this->vocab.size();

            ifstream model_in(model_file);
            if (!model_in.is_open()) {
                std::cerr << "Error opening file." << std::endl;
                throw runtime_error("Error opening file.");
            }

            string version, line;
            getline(model_in, version);
            assert(version == "minbpe v1");
            getline(model_in, pattern);

            int num_special;
            model_in >> num_special;
            model_in.ignore();
            for (int i = 0; i < num_special; ++i) {
                string special;
                int special_idx;
                model_in >> special >> special_idx;
                special_tokens[special] = special_idx;
            }

            while (getline(model_in, line)) {
                istringstream iss(line);
                int idx1, idx2, idx3;
                iss >> idx1 >> idx2 >> idx3;
                merges[make_pair(idx1, idx2)] = idx3;
            }
            model_in.close();

            this->merges = merges;
            this->special_tokens = special_tokens;
            this->vocab = _build_vocab();
        }

        map<pair<int, int>, int> merges;
        string pattern;
        unordered_map<string, int> special_tokens;
        unordered_map<int, string> vocab;

        unordered_map<int, string> _build_vocab() {
            unordered_map<int, string> vocab;
            for (int idx = 0; idx < 256; ++idx) {
                vocab[idx] = string(1, static_cast<char>(idx)); // all byte values
            }

            vector<pair<pair<int, int>, int> > merge_vector;
            for (const auto& m : merges) {
                merge_vector.push_back(make_pair(m.first, m.second));
            }
            sort(merge_vector.begin(), merge_vector.end(), 
                [](const pair<pair<int, int>, int>& a, const pair<pair<int, int>, int>& b) {
                    return a.second < b.second;
            });

            for (const auto& m : merge_vector) {
                vocab[m.second] = vocab[m.first.first] + vocab[m.first.second];
            }
            for (const auto& st : special_tokens) {
                vocab[st.second] = st.first;
            }
            return vocab;
        }

        string render_token(const string& token) {
            string result;
            for (char ch : token) {
                if (iscntrl(ch)) {
                    char buffer[7];
                    snprintf(buffer, sizeof(buffer), "\\u%04x", ch);
                    result.append(buffer);
                } else {
                    result.push_back(ch);
                }
            }
            return result;
        }
};
#endif