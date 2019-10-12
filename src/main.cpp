#include "arduinodata.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ArduinoData w;
    w.show();

    return a.exec();
}
