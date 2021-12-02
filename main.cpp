#include "game.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Game w;
    w.setWindowIcon(QIcon(":/images/icon.png"));
    w.show();
    return a.exec();
}
