#include "base.hpp"
#include "progressbar.hpp"
using namespace std;

regex GPT2_PATTERN(R"('(?:[sdmt]|ll|ve|re)| ?[a-zA-Z]+| ?[0-9]+| ?[^\s\w]+|\s+(?!\S)|\s+)");

class RegexTokenizer: public Tokenizer {
    public:
        virtual void train(const string& text, int vocab_size, bool verbose = false, int save_every = -1, string save_dir = "", regex pattern = GPT2_PATTERN){
            assert(vocab_size >= 256);
            int num_merges = vocab_size - this->vocab.size();
            vector<string> text_chunks = split_string(text, pattern);

            map<pair<int, int>, int> merges; // (int, int) -> int
            unordered_map<int, string> vocab; // int -> bytes
            for (int idx = 0; idx < 256; ++idx) {
                vocab[idx] = string(1, static_cast<char>(idx)); // initializing the vocabulary with single byte tokens
            }
            int old_vocab_size = this->vocab.size();

            vector<vector<uint8_t> > text_chunk_bytes;
            vector<vector<int> > text_chunk_ids;

            // encoded in utf-8
            for(auto chunk: text_chunks){
                vector<uint8_t> chunk_bytes(chunk.begin(), chunk.end());
                text_chunk_bytes.push_back(chunk_bytes);

                vector<int> ids(chunk_bytes.begin(), chunk_bytes.end()); 
                text_chunk_ids.push_back(ids);
            }

            int num_chunks = text_chunks.size();

            auto start_time = steady_clock::now();
            for (int i = 0; i < num_merges; ++i) {
                if(!verbose) print_progress_bar(i, num_merges, start_time);

                map<pair<int, int>, int> pre_stats;
                for (int j = 0; j < num_chunks; ++j){
                    auto chunk_stats = get_stats(text_chunk_ids[j]);
                    for (auto s: chunk_stats){
                        pre_stats[s.first] += s.second;
                    }
                }
                auto stats = sort_stats(pre_stats);

                auto max_pair = stats[0];
                pair<int, int> pair_to_merge = stats[0].first;
                
                int new_token_idx = old_vocab_size + i;

                vector<vector<int> > new_text_chunk_ids;

                for(auto chunk_ids: text_chunk_ids){
                    vector<int> ids = merge(chunk_ids, pair_to_merge, new_token_idx);
                    new_text_chunk_ids.push_back(ids);
                }

                text_chunk_ids = new_text_chunk_ids;

                // Save the merge
                merges[pair_to_merge] = new_token_idx;
                vocab[new_token_idx] = vocab[pair_to_merge.first] + vocab[pair_to_merge.second];
                
                // Checkpoint
                if(save_every != -1 && (i+1)%save_every==0){
                    this->merges.insert(merges.begin(), merges.end());
                    this->vocab.insert(vocab.begin(), vocab.end());
                    save(save_dir);
                }

                if (verbose) {
                    cout << "merge " << (i + 1) << "/" << num_merges 
                        << ": (" << pair_to_merge.first << ", " << pair_to_merge.second 
                        << ") -> " << new_token_idx 
                        << " (" << vocab[new_token_idx] << ") had " 
                        << max_pair.second << " occurrences" << endl << flush;
                }
            }
            this->merges.insert(merges.begin(), merges.end());
            this->vocab.insert(vocab.begin(), vocab.end());
        }

        virtual vector<int> encode_chunk(const string& chunk) {
            vector<uint8_t> chunk_bytes(chunk.begin(), chunk.end()); 
            vector<int> ids(chunk_bytes.begin(), chunk_bytes.end());

            while (ids.size() >= 2) {
                auto stats = get_stats(ids);
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

        virtual vector<int> encode(const string& text, regex pattern = GPT2_PATTERN) {
            vector<string> chunks = split_string(text, pattern);
            vector<int> ids;
            for (auto chunk: chunks){
                auto v = encode_chunk(chunk);
                ids.reserve(ids.size() + distance(v.begin(),v.end()));
                ids.insert(ids.end(), v.begin(), v.end());

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

        vector<string> split_string(string input, regex pattern){
            vector<string> matches;

            // Find all matches using sregex_iterator
            auto words_begin = sregex_iterator(input.begin(), input.end(), pattern);
            auto words_end = sregex_iterator();

            for (sregex_iterator i = words_begin; i != words_end; ++i) {
                smatch match = *i;
                matches.push_back(match.str());
            }

            return matches;
        }
};
