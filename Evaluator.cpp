//
// Created by Arseny's on 12.04.2021.
//

#include "Evaluator.h"
#include "utils.h"
#include <cstring>

std::optional<std::string> Evaluator::GetNextString() {
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

bool Evaluator::IsDrained() const {
    return done_;
}

Evaluator::Evaluator(std::string s, const InstanceCreateDefender&) : s_(std::move(s)) {
    if (s_.empty()) {
        return;
    }
    evaluation_thread_ = std::thread([this] { Evaluate(); });
}

std::string Evaluator::GlueString(char *buf) {
    std::string tail;
    int i = 0;
    while (buf[i] != ' ') {
        tail.push_back(buf[i]);
        ++i;
    }
    ++i;
    std::string begin;
    while (buf[i] != '\n' && buf[i] != '\0') {
        begin.push_back(buf[i]);
        ++i;
    }
    return begin + tail;
}

void Evaluator::Evaluate() {
    auto first = FindFirstMatching();
    if (!need_to_evaluate)
        return;
    std::ifstream prepared_data("DataPreparer/prepared.txt");
    prepared_data.seekg(first);
    char buf[100];
    //for (size_t k = 0; k < 30; ++k) {
    while (true){
        if (!need_to_evaluate)
            return;
        prepared_data.getline(buf, sizeof(buf));
        //if (!QtPrivate::startsWith(s_, buf)) {
        if (!StartsWith(s_.c_str(), buf)) {
            break;
        }
        {
            std::lock_guard lock(mutex_);
            que.push_back(GlueString(buf));
        }
    }
    evaluated_ = true;
}

uintmax_t Evaluator::FindFirstMatching() const {
    PreparedDataCheck();
    std::ifstream prepared_data("DataPreparer/prepared.txt");
    auto f_size = GetFileSize("DataPreparer/prepared.txt");
    uintmax_t l = 0;
    uintmax_t r = f_size;
    const char *s_copy = s_.c_str();
    const int BUF_SZ = 100;
    char buf[BUF_SZ];
    while (r - l > 1) {
        if (!need_to_evaluate)
            return -1;
        uintmax_t m = (r + l) >> 1;
        prepared_data.seekg(m);
        prepared_data.getline(buf, BUF_SZ);
        prepared_data.getline(buf, BUF_SZ);
        int res = memcmp(buf, s_copy, s_.size());
        if (res < 0) {
            l = m;
        } else {
            r = m;
        }
    }
    prepared_data.seekg(r);
    prepared_data.getline(buf, BUF_SZ);
    r += strlen(buf);
    r += 2;
    return r;
}

Evaluator::~Evaluator() {
    need_to_evaluate = false;
    if (evaluation_thread_.joinable())
        evaluation_thread_.join();
}

std::shared_ptr<Evaluator> Evaluator::MakeSharedForEvaluatorPtr(std::string s) {
    return std::make_shared<Evaluator>(s, Evaluator::InstanceCreateDefender());
}
