//
// Created by Arseny's on 13.04.2021.
//

#include "preparer.h"
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <iostream>
#include <algorithm>

#pragma GCC optimize("Ofast")

size_t printed = 0;
size_t printing = 0;

class LightTrie {
private:
    class Node;

public:

    LightTrie() : root_(new Node()) {}

    void AddString(const std::string &s, PrimeSize num) {
        Node *cur = root_;
        for (char c: s) {
            if (cur->links.count(c)) {
                cur = cur->links[c];
            } else {
                ++node_numb;
                cur = AddNode(cur, c);
            }
        }
        ++num_num;
        AddNumb(cur, num);
    }

    size_t node_numb = 0;
    size_t num_num = 0;

    ~LightTrie() {
        delete root_;
    }

    void Print(std::ofstream &out) const {
        root_->Print(out);
    }


    Node *root_;

private:
    static Node *AddNode(Node *parent, char c) {
        return parent->links[c] = new Node();
    }

    static void AddNumb(Node *node, PrimeSize num) {
        node->numbs.push_back(num);
    }

    class Node {
    public:
        std::unordered_map<char, Node *> links;
        std::vector<PrimeSize> numbs;

        PrimeSize Print(std::ofstream &out) {
            ++printing;
            if (printing % 100 == 0)
                std::cout << "Printing: " << printing << std::endl;

            PrimeSize ans = out.tellp();
            PrimeSize tmp = numbs.size();
            out.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            out.write(reinterpret_cast<char *>(&numbs[0]), tmp * sizeof(PrimeSize));
            tmp = links.size();
            out.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            std::vector<std::pair<char, PrimeSize>> vec;
            for (auto it: links) {
                vec.emplace_back(it.first, 0);
            }
            PrimeSize links_beg = out.tellp();
            std::sort(vec.begin(), vec.end());
            const size_t SIZEOF_PAIR = sizeof(char) + sizeof(PrimeSize);
            //out.seekp(links_beg + vec.size() * SIZEOF_PAIR);
            char *raw = new char[(SIZEOF_PAIR) * vec.size()];
            memset(raw, 0, (SIZEOF_PAIR) * vec.size());
            out.write(raw, vec.size() * SIZEOF_PAIR);
            for (auto &i : vec) {
                out.seekp(0, std::ios::end);
                i.second = links[i.first]->Print(out);
            }
            for (size_t i = 0; i < vec.size(); ++i) {
                raw[i * SIZEOF_PAIR] = vec[i].first;
                *reinterpret_cast<PrimeSize *>(&raw[i * SIZEOF_PAIR + sizeof(char)]) = vec[i].second;
            }
            out.seekp(links_beg);
            out.write(raw, vec.size() * SIZEOF_PAIR);
            delete[] raw;
            ++printed;
            if (printed % 100 == 0)
                std::cout << "Printed: " << printed << std::endl;
            return ans;
        }

        ~Node() {
            for (auto it: links) {
                delete it.second;
            }
        }
    };
};

void PrepareData(const std::string &inName, const std::string &outName) {
    /*std::vector<PrimeSize> suf = {4, 6, 2, 5, 3, 5, 3};
    {
        std::ofstream out("test.txt", std::ios::binary);
        out.write(reinterpret_cast<char*>(&suf[0]), suf.size() * sizeof(PrimeSize));
    }
    size_t sz = suf.size();
    suf.clear();
    suf.resize(sz);
    {
        std::ifstream in("test.txt", std::ios::binary);
        in.read(reinterpret_cast<char*>(&suf[0]), sz * sizeof(PrimeSize));
    }
    for (auto it: suf){
        std::cout << it << " ";
    }
    return;*/
    LightTrie suf_trie;
    std::ifstream in(inName);
    if (!in.is_open()) {
        throw std::runtime_error("Couldn't find file " + inName);
    }
    char buf[100];
    auto cur_pos = in.tellg();
    while (in.getline(buf, sizeof(buf))) {
        std::string s(buf);
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
    suf_trie.Print(out);
}