//
// Created by Arseny's on 13.04.2021.
//

#include "TrieEvaluator.h"


std::shared_ptr<TrieEvaluator> TrieEvaluator::MakeSharedForEvaluatorPtr(const std::string &s) {
    return std::make_shared<TrieEvaluator>(s, TrieEvaluator::InstanceCreateDefender());
}

TrieEvaluator::TrieEvaluator(std::string s, const TrieEvaluator::InstanceCreateDefender &) : s_(std::move(s)) {

}

template<typename T>
void ReadRawData(std::ifstream &in, T &x) {
    in.read(reinterpret_cast<char *>(&x), sizeof(x));
}

void TrieEvaluator::Evaluate() {
    if (s_.empty())
        return;
    std::ifstream prepared_data("DataPreparer/prepared.txt", std::ios::binary);
    for (char &c: s_) {
        c = static_cast<char>(std::tolower(c));
    }
    for (char c: s_) {
        PrimeSize numbs_size;
        //prepared_data.read(reinterpret_cast<char*>(&numbs_size), sizeof(numbs_size));
        PrimeSize cur = prepared_data.tellg();
        ReadRawData(prepared_data, numbs_size);
        cur = prepared_data.tellg();
        prepared_data.seekg(cur + numbs_size * sizeof(PrimeSize));
        cur = prepared_data.tellg();
        PrimeSize links_size;
        //prepared_data.read(reinterpret_cast<char*>(&links_size), sizeof(numbs_size));
        ReadRawData(prepared_data, links_size);
        cur = prepared_data.tellg();
        for (size_t i = 0; i < links_size; ++i) {
            char link_let;
            ReadRawData(prepared_data, link_let);
            cur = prepared_data.tellg();
            PrimeSize link_dst;
            ReadRawData(prepared_data, link_dst);
            cur = prepared_data.tellg();
            if (link_let == c) {
                cur = link_dst;
                prepared_data.seekg(cur);
                cur = prepared_data.tellg();
                break;
            }
        }
    }
    std::ifstream dict("words.txt");
    TrieTraverse(prepared_data.tellg(), prepared_data, dict);
}

void TrieEvaluator::TrieTraverse(PrimeSize cur, std::ifstream &prepared_data, std::ifstream &dict) {
    prepared_data.seekg(cur);
    PrimeSize numbs_size;
    //prepared_data.read(reinterpret_cast<char*>(&numbs_size), sizeof(numbs_size));
    {
        ReadRawData(prepared_data, numbs_size);
        std::vector<PrimeSize> numbs;
        numbs.resize(numbs_size);
        prepared_data.read(reinterpret_cast<char *>(&numbs[0]), numbs_size * sizeof(PrimeSize));
        for (auto num: numbs) {
            dict.seekg(num);
            char buf[100];
            dict.getline(buf, sizeof(buf));
            AddString(buf);
        }
    }
    prepared_data.seekg(cur + sizeof(numbs_size) + numbs_size * sizeof(PrimeSize));
    PrimeSize links_size;
    std::vector<PrimeSize> link_dsts;
    ReadRawData(prepared_data, links_size);
    for (size_t i = 0; i < links_size; ++i) {
        char link_let;
        ReadRawData(prepared_data, link_let);
        cur = prepared_data.tellg();
        link_dsts.push_back(0);
        ReadRawData(prepared_data, link_dsts.back());
    }
    for (size_t i = 0; i < links_size; ++i) {
        TrieTraverse(link_dsts[i], prepared_data, dict);
    }
}
