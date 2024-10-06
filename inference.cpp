#include "minbpe/basic.hpp"

int main() {
    BasicTokenizer tokenizer;
    string text = "मेरा नाम इर्श है।";
    
    tokenizer.load("models/hindi/hindi_50256.model");

    vector<int> encoded = tokenizer.encode(text);
    for (int id : encoded) {
        cout << id << " "; // Print encoded ids
    }
    cout << endl;

    string decoded = tokenizer.decode(encoded);
    cout << decoded << endl; // Print the decoded text

    return 0;
}