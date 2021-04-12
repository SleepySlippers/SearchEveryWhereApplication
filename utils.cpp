//
// Created by Arseny's on 12.04.2021.
//

#include "DataPreparer/preparer.h"
#include "utils.h"
#include <cstring>

bool StartsWith(const char *pre, const char *str) {
    size_t len_pre = strlen(pre),
            len_str = strlen(str);
    return len_str >= len_pre && memcmp(pre, str, len_pre) == 0;
}

void PreparedDataCheck() {
    std::ifstream prepared_data("DataPreparer/prepared.txt");
    if (!prepared_data.is_open()) {
        std::ifstream tmp("words.txt");
        if (!tmp.is_open()) {
            throw std::runtime_error("No dictionary file : words.txt");
        }
        PrepareData("words.txt", "DataPreparer/prepared.txt");
    }
}

std::ifstream::pos_type GetFileSize(const char *filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}