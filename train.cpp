#include "minbpe/basic.hpp"

int main() {
    BasicTokenizer tokenizer;

    string filename = "tests/test.hi";
    ifstream file(filename);

    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    int vocab_size = 1256;

    tokenizer.train(text, vocab_size, false);

    tokenizer.save("models/hindi/hindi_1256");
    cout << "Tokenizer trained and saved successfully." << endl;

    return 0;
}