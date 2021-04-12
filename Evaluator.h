//
// Created by Arseny's on 12.04.2021.
//

#ifndef SEARCHEVERYWHEREAPPLICATION_EVALUATOR_H
#define SEARCHEVERYWHEREAPPLICATION_EVALUATOR_H

#include "preparer.h"
#include <thread>
#include <deque>
#include <utility>
#include <fstream>
#include <optional>
#include <QStringAlgorithms>
#include <memory>
#include <mutex>

// Only MakeSharedForEvaluatorPtr can create instance of Evaluator
// Only one consumer supported

class Evaluator : public std::enable_shared_from_this<Evaluator> {
public:

    static std::shared_ptr<Evaluator> MakeSharedForEvaluatorPtr(std::string s);

    class InstanceCreateDefender {
        friend std::shared_ptr<Evaluator> Evaluator::MakeSharedForEvaluatorPtr(std::string s);

        InstanceCreateDefender() = default;
    };

    explicit Evaluator(std::string s, const InstanceCreateDefender &);

    Evaluator(const Evaluator &) = delete;

    Evaluator(Evaluator &&) = delete;

    Evaluator &operator=(const Evaluator &) = delete;

    Evaluator &operator=(Evaluator &&) = delete;

    std::optional<std::string> GetNextString();

    bool IsDrained() const;

    ~Evaluator();

private:

    static std::string GlueString(char *buf);

    void Evaluate();

    uintmax_t FindFirstMatching() const;

    std::thread evaluation_thread_;
    std::mutex mutex_;
    std::deque<std::string> que; // Guarded by mutex_

    std::string s_;
    bool evaluated_ = false;
    bool done_ = false;
    bool need_to_evaluate = true;
};

#endif //SEARCHEVERYWHEREAPPLICATION_EVALUATOR_H
