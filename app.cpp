#include <QApplication>
#include <QTextBlock>
#include <QLineEdit>
#include <QLabel>
#include <QBoxLayout>
#include <fstream>
#include <QTextEdit>
#include <QThread>

#include "preparer.h"

class WordsPrinter : public QObject {
public:
    explicit WordsPrinter(QLayout* layout) {
        layout->addWidget(&label);
    }

public slots:
    void RequestChanged(const QString& s){
        label.setText("");
        if (s.toStdString().empty())
            return;
        auto first = FindFirstMatching(s.toStdString());
        std::ifstream prepared_data("DataPreparer/prepared.txt");
        prepared_data.seekg(first);
        std::string ans;
        for (size_t k = 0; k < 30; ++k){
            char buf[100];
            prepared_data.getline(buf, sizeof(buf));
            if (!StartsWith(s.toStdString().c_str(), buf)){
                break;
            }
            std::string tail;
            int i = 0;
            while (buf[i] != ' '){
                tail.push_back(buf[i]);
                ++i;
            }
            ++i;
            std::string begin;
            while (buf[i] != '\n' && buf[i] != '\0'){
                begin.push_back(buf[i]);
                ++i;
            }
            ans += begin + tail + "\n";
            //label.setText(QString::fromStdString(ans));
            label.append(QString::fromStdString(begin + tail));
        }
    }

private:
    static bool StartsWith(const char *pre, const char *str)
    {
        size_t len_pre = strlen(pre),
                len_str = strlen(str);
        return len_str >= len_pre && memcmp(pre, str, len_pre) == 0;
    }

    static std::ifstream::pos_type filesize(const char* filename)
    {
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        return in.tellg();
    }

    static uintmax_t FindFirstMatching(const std::string& s){
        PreparedDataCheck();
        std::ifstream prepared_data("DataPreparer/prepared.txt");
        auto f_size = filesize("DataPreparer/prepared.txt");
        uintmax_t l = 0;
        uintmax_t r = f_size;
        const char *s_copy = s.c_str();
        char* buf;
        const int BUF_SZ = 100;
        buf = new char[BUF_SZ];
        while (r - l > 1){
            uintmax_t m = r + l >> 1;
            prepared_data.seekg(m);
            prepared_data.getline(buf, BUF_SZ);
            prepared_data.getline(buf, BUF_SZ);
            int res = memcmp(buf, s_copy, s.size());
            if (res < 0){
                l = m;
            } else {
                r = m;
            }
        }
        prepared_data.seekg(r);
        prepared_data.getline(buf, BUF_SZ);
        r += strlen(buf);
        delete buf;
        ++r;
        ++r;
        return r;
    }

    static void PreparedDataCheck(){
        std::ifstream prepared_data("DataPreparer/prepared.txt");
        if (prepared_data.bad()){
            std::ifstream tmp("words.txt");
            if (tmp.bad()){
                throw std::runtime_error("No dictionary file : words.txt");
            }
            PrepareData("../words.txt", "prepared.txt");
        }
    }

    QTextEdit label;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QWidget widget;
    QVBoxLayout layout(&widget);
    QLineEdit edit;
    layout.addWidget(&edit);
    WordsPrinter wordsPrinter(&layout);
    QObject::connect(&edit, &QLineEdit::textChanged, &wordsPrinter, &WordsPrinter::RequestChanged);
    widget.show();
    return QApplication::exec();
}
