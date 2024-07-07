#include "LC3.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon icon(":/new/prefix1/icon.png"); // Replace with actual path
    a.setWindowIcon(icon);
    lc3 w;
    w.show();
    return a.exec();
}
