//
// Created by Arseny's on 12.04.2021.
//

#ifndef SEARCHEVERYWHEREAPPLICATION_UTILS_H
#define SEARCHEVERYWHEREAPPLICATION_UTILS_H

#include <fstream>

void PreparedDataCheck();

std::ifstream::pos_type GetFileSize(const char *filename);

bool StartsWith(const char *pre, const char *str);

#endif //SEARCHEVERYWHEREAPPLICATION_UTILS_H
