//
// Created by Arseny's on 12.04.2021.
//

#ifndef SEARCHEVERYWHEREAPPLICATION_WORDSPRINTER_H
#define SEARCHEVERYWHEREAPPLICATION_WORDSPRINTER_H

#include "Evaluator.h"
#include <chrono>
#include <thread>
#include <QObject>
#include <QTextEdit>
#include <QLayout>
#include <QLabel>
#include <unordered_set>

class WordsPrinter : public QObject {
Q_OBJECT
public:
    explicit WordsPrinter(QLayout *layout);

    WordsPrinter(WordsPrinter&&) = delete;
    WordsPrinter(const WordsPrinter&) = delete;
    WordsPrinter& operator=(const WordsPrinter&) = delete;
    WordsPrinter& operator=(WordsPrinter&&) = delete;

    ~WordsPrinter(){
        need_to_stop_printing_ = true;
        if (printer_thread_.joinable()){
            printer_thread_.join();
        }
    }

public slots:

    void RequestChanged(const QString &s);

    void Append(const QString &s) {
        results_.append(s);
        results_.moveCursor(QTextCursor::Start);
    }

    void UpdateStatus() {
        std::string stat = "Status: ";
        if (!printing_done_) {
            stat += "Searching... ";
        } else {
            stat += "Search done. ";
        }
        stat += std::to_string(results_num_) + " found.";
        status_.setText(QString::fromStdString(stat));
    }

private:

    void PrintingDone() {
        printing_done_ = true;
    }

    std::shared_ptr<Evaluator> current_evaluator_;
    QLabel status_;
    QTextEdit results_;
    std::unordered_set<std::string> used_;
    std::thread printer_thread_;
    bool need_to_stop_printing_ = false;
    bool printing_done_ = true;
    long long results_num_ = 0;
};

#endif //SEARCHEVERYWHEREAPPLICATION_WORDSPRINTER_H
