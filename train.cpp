#include "minbpe/basic.hpp"

int main() {
    BasicTokenizer tokenizer;

    string filename = "tests/test.hi";
    ifstream file(filename);
    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    // target vocab_size
    int vocab_size = 1256;

    // train
    // training text, vocabulary size, verbose, save_every, save_path
    tokenizer.train(text, vocab_size, false, 100, "models/hindi/hindi_1256");

    tokenizer.save("models/hindi/hindi_1256");
    cout << "Tokenizer trained and saved successfully." << endl;

    return 0;
}