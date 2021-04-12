//
// Created by Arseny's_ on 11.04.2021.
//

#include <cassert>
#include "preparer.h"

int main(int argc, char *argv[]){
    assert(argc == 2);
    PrepareData(argv[1], "prepared.txt");
}
