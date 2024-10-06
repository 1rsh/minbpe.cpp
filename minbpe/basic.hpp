#include <bits/stdc++.h>
#include "base.hpp"
#include "progressbar.hpp"

using namespace std;

class BasicTokenizer: public Tokenizer {
    public:
        virtual void train(const string& text, int vocab_size, bool verbose = false){
            assert(vocab_size >= 256);
            int num_merges = vocab_size - 256;

            // encoded in utf-8
            vector<uint8_t> text_bytes(text.begin(), text.end());
            vector<int> ids(text_bytes.begin(), text_bytes.end()); 

            map<pair<int, int>, int> merges; // (int, int) -> int
            unordered_map<int, string> vocab; // int -> bytes
            for (int idx = 0; idx < 256; ++idx) {
                vocab[idx] = string(1, static_cast<char>(idx)); // initializing the vocabulary with single byte tokens
            }

            for (int i = 0; i < num_merges; ++i) {
                if(!verbose) print_progress_bar(i, num_merges);

                auto stats = sort_stats(get_stats(ids));

                auto max_pair = stats[0];
                pair<int, int> pair_to_merge = stats[0].first;
                
                int new_token_idx = 256 + i;

                ids = merge(ids, pair_to_merge, new_token_idx);

                // Save the merge
                merges[pair_to_merge] = new_token_idx;
                vocab[new_token_idx] = vocab[pair_to_merge.first] + vocab[pair_to_merge.second];

                if (verbose) {
                    cout << "merge " << (i + 1) << "/" << num_merges 
                        << ": (" << pair_to_merge.first << ", " << pair_to_merge.second 
                        << ") -> " << new_token_idx 
                        << " (" << vocab[new_token_idx] << ") had " 
                        << max_pair.second << " occurrences" << endl << flush;
                }
            }

            this->merges = merges;
            this->vocab = vocab;
        }

        virtual vector<int> encode(const string& text) {
            vector<uint8_t> text_bytes(text.begin(), text.end()); 
            vector<int> ids(text_bytes.begin(), text_bytes.end());

            while (ids.size() >= 2) {
                auto stats = get_stats(ids);
                
                // Find the pair with the lowest merge index
                pair<int, int> pair_to_merge;
                int min_index = INT_MAX; // Initialize with a large value
                bool found = false; // Flag to check if a pair is found

                for (const auto& stat : stats) {
                    auto it = merges.find(stat.first);
                    if (it != merges.end() && it->second < min_index) {
                        min_index = it->second;
                        pair_to_merge = stat.first;
                        found = true; // Found a valid pair to merge
                    }
                }

                // If no more merges are available, break the loop
                if (!found) {
                    break; // Nothing else can be merged anymore
                }

                // Otherwise, let's merge the best pair (lowest merge index)
                int idx = merges[pair_to_merge];
                ids = merge(ids, pair_to_merge, idx); // Perform the merge
            }
            
            return ids;
        }


        virtual string decode(const vector<int>& ids) {
            string text;
            for (int idx : ids) {
                text += vocab[idx];
            }
            return text;
        }
};
