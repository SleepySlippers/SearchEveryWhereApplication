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

class LightTrie;

class SubSeqNFA {
    class Node;

    friend class SufTrie;

public:

    explicit SubSeqNFA(const std::string &s) : root_(new Node()) {
        for (size_t i = 0; i < s.size(); ++i) {
            nodes_.push_back(new Node());
            root_->links.emplace_back(s[i], nodes_[i]);
        }
        for (size_t i = 0; i < s.size(); ++i) {
            for (size_t j = i + 1; j < s.size(); ++j) {
                nodes_[i]->links.emplace_back(s[j], nodes_[j]);
            }
        }
    }

private:
    class Node {
    public:
        std::vector<std::pair<char, Node *>> links;
    };

    Node *root_;
    std::vector<Node *> nodes_;
};

class SufTrie {
    class Node;

    friend class LightTrie;

public:

    SufTrie(const Node &) = delete;

    SufTrie &operator=(const Node &) = delete;

    explicit SufTrie(const std::string &s) : root_(new Node()) {
        for (size_t i = 0; i < s.size(); ++i) {
            AddString(s.substr(i));
        }
    }

    explicit SufTrie(const SubSeqNFA &nfa) : root_(new Node()) {
        CopySubSeqNFANode(root_, nfa.root_);
    }

    ~SufTrie() {
        delete root_;
    }

private:

    void AddString(const std::string &s) {
        Node *cur = root_;
        for (char c: s) {
            if (!cur->links.count(c)) {
                cur->links.emplace(c, new Node());
            }
            cur = cur->links.at(c);
        }
    }

    void CopySubSeqNFANode(Node *my, SubSeqNFA::Node *others) {
        for (auto it: others->links) {
            if (!my->links.count(it.first)) {
                my->links.emplace(it.first, new Node());
            }
            CopySubSeqNFANode(my->links.at(it.first), it.second);
        }
    }

    class Node {
    public:
        std::unordered_map<char, Node *> links;

        ~Node() {
            for (auto &lnk: links) {
                delete lnk.second;
            }
        }
    };

    Node *root_;
};

class LightTrie {
    class Node;

public:

    LightTrie() : root_(new Node()) {}

    void AddString(const std::string &s, PrimeSize num);

    void AddSufTrie(SufTrie &&suf_trie, const PrimeSize &num) {
        CopySufTrieNode(root_, suf_trie.root_, num);
    }

    size_t node_numb = 0;
    size_t num_num = 0;

    ~LightTrie();

    void Print(std::ofstream &out) const;

    Node *root_;

private:

    void CopySufTrieNode(Node *my, SufTrie::Node *others, const PrimeSize &num) {
        if (others->links.empty()) {
            ++num_num;
            AddNumb(my, num);
            //my->numbs.push_back(num);
            return;
        }
        for (auto it: others->links) {
            if (!my->links.count(it.first)) {
                ++node_numb;
                AddNode(my, it.first);
                //my->links.emplace(it.first, new Node());
            }
            CopySufTrieNode(my->links.at(it.first), it.second, num);
        }
    }

    static void AddNode(Node *parent, char c);

    static void AddNumb(Node *node, PrimeSize num);

    class Node {
    public:
        std::unordered_map<char, Node *> links;
        std::vector<PrimeSize> numbs;

        PrimeSize Print(std::ofstream &out);

        ~Node();
    };
};

void PrepareData(const std::string &inName, const std::string &outName) {
    LightTrie suf_trie;
    std::ifstream in(inName);
    if (!in.is_open()) {
        throw std::runtime_error("Couldn't find file " + inName);
    }
    char buf[100];
    auto cur_pos = in.tellg();
    while (in.getline(buf, sizeof(buf))) {
        std::string s(buf);
        /*for (char &c: s) {
            c = static_cast<char>(std::tolower(c));
        }
        for (size_t i = 0; i < s.size(); ++i) {
            suf_trie.AddString(s.substr(i), cur_pos);
        }*/
        auto tmp = SufTrie(s);
        suf_trie.AddSufTrie(std::move(tmp), cur_pos);
        cur_pos = in.tellg();
    }
    std::cout << "Nodes: " << suf_trie.node_numb << " Nums: " << suf_trie.num_num << std::endl;
    std::ofstream out(outName, std::ios::binary);
    suf_trie.Print(out);
}

PrimeSize LightTrie::Node::Print(std::ofstream &out) {
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

LightTrie::Node::~Node() {
    for (auto it: links) {
        delete it.second;
    }
}

void LightTrie::AddString(const std::string &s, PrimeSize num) {
    Node *cur = root_;
    for (char c: s) {
        if (!cur->links.count(c)) {
            ++node_numb;
            AddNode(cur, c);
        }
        cur = cur->links[c];
    }
    ++num_num;
    AddNumb(cur, num);
}

void LightTrie::Print(std::ofstream &out) const {
    root_->Print(out);
}

void LightTrie::AddNode(LightTrie::Node *parent, char c) {
    if (!parent->links.count(c))
        parent->links.emplace(c, new Node());
}

void LightTrie::AddNumb(LightTrie::Node *node, PrimeSize num) {
    node->numbs.push_back(num);
}

LightTrie::~LightTrie() {
    delete root_;
}