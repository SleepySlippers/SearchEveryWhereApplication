//
// Created by Arseny's on 13.04.2021.
//

#ifndef SEARCHEVERYWHEREAPPLICATION_EVALUATORINTERFACE_H
#define SEARCHEVERYWHEREAPPLICATION_EVALUATORINTERFACE_H

#include <memory>
#include <optional>
#include <mutex>
#include <deque>
#include <functional>
#include <utility>
#include <thread>

class EvaluatorInterface {
    class StopEvaluationException : public std::exception {
    };

protected:
    EvaluatorInterface() : evaluation_thread_([this] {
        try {
            Evaluate();
        } catch (const StopEvaluationException &) {}
        evaluated_ = true;
    }) {}

    void AddString(const std::string &s) {
        if (!need_to_evaluate) {
            throw StopEvaluationException();
        }
        std::lock_guard lock(mutex_);
        que.push_back(s);
    }

public:

    EvaluatorInterface(const EvaluatorInterface &) = delete;

    EvaluatorInterface(EvaluatorInterface &&) = delete;

    EvaluatorInterface &operator=(const EvaluatorInterface &) = delete;

    EvaluatorInterface &operator=(EvaluatorInterface &&) = delete;

    void StopEvaluationGently() {
        need_to_evaluate = false;
        if (evaluation_thread_.joinable()) {
            evaluation_thread_.join();
        }
    }

    virtual std::optional<std::string> GetNextString() {
        std::lock_guard lock(mutex_);
        std::optional<std::string> ans;
        if (!que.empty()) {
            ans = que.front();
            que.pop_front();
        } else {
            if (evaluated_ && que.empty()) {
                done_ = true;
            }
        }
        return ans;
    }

    virtual bool IsDrained() const {
        return done_;
    }

    ~EvaluatorInterface() {
        StopEvaluationGently();
    }

private:

    virtual void Evaluate() = 0;

    bool need_to_evaluate = true;

    std::thread evaluation_thread_;
    std::mutex mutex_;
    std::deque<std::string> que; // Guarded by mutex_
    bool evaluated_ = false;
    bool done_ = false;
};

#endif //SEARCHEVERYWHEREAPPLICATION_EVALUATORINTERFACE_H
