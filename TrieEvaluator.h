//
// Created by Arseny's on 13.04.2021.
//

#ifndef SEARCHEVERYWHEREAPPLICATION_TRIEEVALUATOR_H
#define SEARCHEVERYWHEREAPPLICATION_TRIEEVALUATOR_H

#include "EvaluatorInterface.h"

#include "preparer.h"
#include <thread>
#include <deque>
#include <utility>
#include <fstream>
#include <optional>
//#include <QStringAlgorithms>
#include <memory>
#include <mutex>

class TrieEvaluator : public EvaluatorInterface {
public:

    static std::shared_ptr<TrieEvaluator> MakeSharedForEvaluatorPtr(const std::string &s);

private:
    class InstanceCreateDefender {
        friend std::shared_ptr<TrieEvaluator> TrieEvaluator::MakeSharedForEvaluatorPtr(const std::string &s);

        InstanceCreateDefender() = default;
    };

public:

    explicit TrieEvaluator(std::string s, const InstanceCreateDefender &);

private:

    void Evaluate() override;

    void TrieTraverse(PrimeSize cur, std::ifstream &prepared_data, std::ifstream &dict);

    std::string s_;
};


#endif //SEARCHEVERYWHEREAPPLICATION_TRIEEVALUATOR_H
