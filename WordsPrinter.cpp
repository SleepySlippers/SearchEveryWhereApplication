//
// Created by Arseny's on 12.04.2021.
//

#include "WordsPrinter.h"
#include <QCoreApplication>
#include <QFuture>
#include <QThread>

WordsPrinter::WordsPrinter(QLayout *layout) {
    layout->addWidget(&status_);
    layout->addWidget(&results_);
    UpdateStatus();
}

void WordsPrinter::RequestChanged(const QString &s) {
    need_to_stop_printing_ = true;
    if (printer_thread_.joinable())
        printer_thread_.join();
    results_num_ = 0;
    results_.setText("");
    if (s.toStdString().empty()) {
        printing_done_ = true;
        UpdateStatus();
        return;
    }
    printer_thread_ = std::thread([this, s] {
        used_.clear();
        need_to_stop_printing_ = false;
        printing_done_ = false;
        current_evaluator_ = Evaluator::MakeSharedForEvaluatorPtr(s.toStdString());
        while (!current_evaluator_->IsDrained()) {
            std::string ans;
            for (size_t i = 0; i < 100 && !current_evaluator_->IsDrained(); ++i) {
                if (need_to_stop_printing_)
                    return;
                auto tmp = current_evaluator_->GetNextString();
                if (tmp.has_value() && !used_.count(tmp.value())) {
                    used_.insert(tmp.value());
                    ans += tmp.value() + "\n";
                    ++results_num_;
                }
            }
            if (!ans.empty()){
                ans.pop_back();
            }
            QMetaObject::invokeMethod(this, "Append",
                                       Q_ARG(const QString&, QString::fromStdString(ans)));
            QMetaObject::invokeMethod(this, "UpdateStatus");
            using namespace std::chrono_literals;
            if (need_to_stop_printing_)
                return;
            std::this_thread::sleep_for(2ms);
        }
        PrintingDone();
        QMetaObject::invokeMethod(this, "UpdateStatus");
    });
}
