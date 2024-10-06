#include "../minbpe/basic.hpp"

int main() {
    BasicTokenizer tokenizer;
    string text = "aaabdaaabac";
    tokenizer.train(text, 256 + 3, true); // 256 byte tokens, then do 3 merges

    vector<int> encoded = tokenizer.encode(text);
    for (int id : encoded) {
        cout << id << " "; // Print encoded ids
    }
    cout << endl;

    string decoded = tokenizer.decode(encoded);
    cout << decoded << endl; // Print the decoded text

    tokenizer.save("toy"); // Save the tokenizer

    return 0;
}
