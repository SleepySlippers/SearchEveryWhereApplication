#include <QApplication>
#include <QLineEdit>
#include <QBoxLayout>

#include "Evaluator.h"
#include "WordsPrinter.h"

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
