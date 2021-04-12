//
// Created by Arseny's_ on 11.04.2021.
//

#include "preparer.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

void PrepareData(std::string inName, std::string outName) {
    std::ifstream in(inName);
    if (!in.is_open()){
        throw std::runtime_error("Couldn't find file " + inName);
    }
    std::ofstream out(outName);
    std::string s;
    std::vector < std::string > suffixes;
    while (in >> s){
        for (size_t i = 0; i < s.size(); ++i) {
            suffixes.push_back(s.substr(i) + " " + s.substr(0, i));
        }
    }
    std::cout << "Start sorting" << std::endl;
    std::sort(suffixes.begin(), suffixes.end());
    std::cout << "Sort ended\nPrinting" << std::endl;
    out << "\n";
    for (const auto &s: suffixes){
        out << s << "\n";
    }
}

