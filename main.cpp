#include "view/playwidget.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>
#include <QStyleFactory>
#include "view/myview.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    PlayWidget w;
//    w.show();
    a.setStyle(QStyleFactory::create("Fusion"));
    MyView w;
    w.show();
//    SetView w;
//    w.show();
/*    LoginWidget w;
    w.show()*/;
    return a.exec();
}
