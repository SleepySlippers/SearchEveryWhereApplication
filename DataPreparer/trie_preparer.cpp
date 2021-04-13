//
// Created by Arseny's on 13.04.2021.
//

#include "preparer.h"
#include <fstream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <iostream>

class Trie {
public:
    using PrimeSize = size_t;

    explicit Trie(std::unordered_map<char, size_t> alphabet) : alphabet_map_(std::move(alphabet)),
                                                               alphabet_sz_(alphabet_map_.size()) {
        raw_data.resize((alphabet_sz_ + 1));
        memset(&raw_data[0], 0, (alphabet_sz_ + 1) * sizeof(PrimeSize));
    }

    explicit Trie(const std::string &alphabet) : alphabet_map_(BuildAlphabetMap(alphabet)),
                                                 alphabet_sz_(alphabet_map_.size()) {
        raw_data.resize((alphabet_sz_ + 1));
        memset(&raw_data[0], 0, (alphabet_sz_ + 1) * sizeof(PrimeSize));
    }

    PrimeSize AddNode(PrimeSize parent_begin, PrimeSize letter_num) {
        ++node_numb;
        PrimeSize prev_size = raw_data.size();
        raw_data[parent_begin + letter_num] = prev_size;
        raw_data.resize(prev_size + (alphabet_sz_ + 1));
        memset(&raw_data[prev_size], 0, (alphabet_sz_ + 1) * sizeof(PrimeSize));
        return prev_size;
    }

    void AddNumb(PrimeSize parent, PrimeSize val) {
        ++num_num;
        PrimeSize prev = raw_data[parent + alphabet_sz_];
        raw_data[parent + alphabet_sz_] = raw_data.size();
        raw_data.push_back(val);
        raw_data.push_back(prev);
    }

    void AddString(const std::string &s, PrimeSize num) {
        PrimeSize cur = 0;
        for (char c: s) {
            if (!alphabet_map_.count(c)) {
                throw std::runtime_error("trying to add string with char out of alphabet");
            }
            if (raw_data[cur + alphabet_map_[c]] == 0) {
                cur = AddNode(cur, alphabet_map_[c]);
            } else {
                cur = raw_data[cur + alphabet_map_[c]];
            }
        }
        AddNumb(cur, num);
    }

    static std::unordered_map<char, size_t> BuildAlphabetMap(const std::string &s) {
        std::unordered_map<char, size_t> ans;
        for (char c: s) {
            if (!ans.count(c)) {
                ans[c] = ans.size();
            }
        }
        return ans;
    }

    const char *GetRawData() const {
        return reinterpret_cast<const char *>(&raw_data[0]);
    }

    PrimeSize GetRawDataSize() const {
        return raw_data.size() * sizeof(PrimeSize);
    }

    size_t node_numb = 0;
    size_t num_num = 0;

private:
    std::unordered_map<char, size_t> alphabet_map_;
    const size_t alphabet_sz_;
    std::vector<PrimeSize> raw_data;
};

void PrepareData(const std::string &inName, const std::string &outName) {
    std::unordered_map<char, size_t> alphabet;
    { // find alphabet
        std::ifstream in(inName);
        if (!in.is_open()) {
            throw std::runtime_error("Couldn't find file " + inName);
        }
        std::string s;
        while (in >> s) {
            for (char c: s) {
                c = static_cast<char>(std::tolower(c));
                if (!alphabet.count(c)) {
                    alphabet[c] = alphabet.size();
                }
            }
        }
    }
    Trie suf_trie(alphabet);
    //std::ofstream out(outName);
    std::ifstream in(inName);
    if (!in.is_open()) {
        throw std::runtime_error("Couldn't find file " + inName);
    }
    std::string s;
    auto cur_pos = in.tellg();
    while (in >> s) {
        for (char &c: s) {
            c = static_cast<char>(std::tolower(c));
        }
        for (size_t i = 0; i < s.size(); ++i) {
            suf_trie.AddString(s.substr(i), cur_pos);
        }
        cur_pos = in.tellg();
    }
    std::cout << "Nodes: " << suf_trie.node_numb << " Nums: " << suf_trie.num_num << std::endl;
    std::ofstream out(outName, std::ios::binary);
    out.write(suf_trie.GetRawData(), static_cast<long long>(suf_trie.GetRawDataSize()));
}