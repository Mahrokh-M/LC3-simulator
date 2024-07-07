
#include "Logic.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QIcon icon(":/new/prefix1/icon.png"); // Replace with actual path
    app.setWindowIcon(icon);
    Logic w;
    w.show();
    return app.exec();
}
