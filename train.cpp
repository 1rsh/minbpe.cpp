#include <bits/stdc++.h>
#include "minbpe/base.hpp"
#include "minbpe/progressbar.hpp"
#include "minbpe/basic.hpp"
#include "minbpe/regex.hpp"

int main() {
    BasicTokenizer tokenizer;

    string filename = "tests/test.hi";
    ifstream file(filename);
    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    // target vocab_size
    int vocab_size = 50256;
    string save_path = "models/hindi/hindi_" + to_string(vocab_size);
    bool verbose = (vocab_size <= 1256);

    // train
    // training text, vocabulary size, verbose, save_every, save_path
    tokenizer.train(text, vocab_size, verbose, 100, save_path);

    tokenizer.save(save_path);
    cout << "Tokenizer trained and saved successfully." << endl;

    return 0;
}