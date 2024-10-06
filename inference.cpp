#include "minbpe/basic.hpp"

int main() {
    BasicTokenizer tokenizer;
    string text = "मेरा नाम इर्श है।";

    cout << "Original String: " << text << endl;
    
    tokenizer.load("models/hindi/hindi_1256.model");

    cout << "Tokens: ";
    vector<int> encoded = tokenizer.encode(text);
    for (int id : encoded) {
        cout << id << " "; // Print encoded ids
    }
    cout << endl;

    string decoded = tokenizer.decode(encoded);
    cout << "Decoded String: " << decoded << endl; // Print the decoded text

    cout << "Check: " << (strcmp(text.c_str(), decoded.c_str()) == 0) << endl;

    return 0;
}