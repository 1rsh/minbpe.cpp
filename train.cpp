#include <bits/stdc++.h>
#include "minbpe/base.hpp"
#include "minbpe/progressbar.hpp"
#include "minbpe/basic.hpp"
#include "minbpe/regex.hpp"

int main() {
    RegexTokenizer tokenizer; // change tokenizer class here

    string filename = "tests/taylorswift.txt";
    ifstream file(filename);
    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    // target vocab_size
    int vocab_size = 512;
    string save_path = "models/taylor/taylor_" + to_string(vocab_size);
    bool verbose = (vocab_size <= 1256);

    // train
    // training text, vocabulary size, verbose, save_every, save_path, pattern
    tokenizer.train(text, vocab_size, verbose, 100, save_path, GPT4_PATTERN);

    tokenizer.save(save_path);
    cout << "Tokenizer trained and saved successfully." << endl;

    return 0;
}