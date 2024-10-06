# minbpe.cpp

Inspired by [Andrej Karpathy](https://github.com/karpathy)'s tutorial on [GPT2's BPE Tokenizer](https://github.com/karpathy/minbpe/tree/master) in C++.

Minimal, clean code for the (byte-level) Byte Pair Encoding (BPE) algorithm commonly used in LLM tokenization. The BPE algorithm is "byte-level" because it runs on UTF-8 encoded strings.

This algorithm was popularized for LLMs by the [GPT-2 paper](https://d4mucfpksywv.cloudfront.net/better-language-models/language_models_are_unsupervised_multitask_learners.pdf) and the associated GPT-2 [code release](https://github.com/openai/gpt-2) from OpenAI. [Sennrich et al. 2015](https://arxiv.org/abs/1508.07909) is cited as the original reference for the use of BPE in NLP applications. Today, all modern LLMs (e.g. GPT, Llama, Mistral) use this algorithm to train their tokenizers.

This repository is a work in progress with the following functionalities:
1. [minbpe/base.hpp](minbpe/base.hpp): Implements the `Tokenizer` class, which is the base class. It contains the `train`, `encode`, and `decode` stubs, save/load functionality, and there are also a few common utility functions. This class is not meant to be used directly, but rather to be inherited from.
2. [minbpe/basic.cpp](minbpe/basic.cpp): Implements the `BasicTokenizer`, the simplest implementation of the BPE algorithm that runs directly on text.

Finally, The script [train.cpp](train.cpp) trains the tokenizer on the input text [tests/test.hi](tests/test.hi) (IIT Bombay's English-Hindi Corpus) and saves the vocab to disk for visualization. This script runs in about 48 seconds on my (M3 Max) MacBook.

All of the files above are very short and thoroughly commented, and also contain a usage example on the bottom of the file.

## quick start

As the simplest example, we can reproduce the [Wikipedia article on BPE](https://en.wikipedia.org/wiki/Byte_pair_encoding) using [examples/toy_example.cpp](examples/toy_example.cpp):

```cpp
#include "../minbpe/basic.hpp"

int main() {
    BasicTokenizer tokenizer;
    string text = "aaabdaaabac";
    tokenizer.train(text, 256 + 3, true); // 256 byte tokens, then do 3 merges

    vector<int> encoded = tokenizer.encode(text);
    for (int id : encoded) cout << id << " "; // Print encoded ids
    cout << endl;

    string decoded = tokenizer.decode(encoded);
    cout << decoded << endl; // Print the decoded text

    tokenizer.save("toy"); // Save the tokenizer

    return 0;
}
```

According to Wikipedia, running bpe on the input string: "aaabdaaabac" for 3 merges results in the string: "XdXac" where  X=ZY, Y=ab, and Z=aa. The tricky thing to note is that minbpe always allocates the 256 individual bytes as tokens, and then merges bytes as needed from there. So for us a=97, b=98, c=99, d=100 (their [ASCII](https://www.asciitable.com) values). Then when (a,a) is merged to Z, Z will become 256. Likewise Y will become 257 and X 258. So we start with the 256 bytes, and do 3 merges to get to the result above, with the expected output of [258, 100, 258, 97, 99].

## todos
- write RegexTokenizer in C++
- write GPT4Tokenizer in C++ 
- write LlamaTokenizer in C++?

## License

MIT