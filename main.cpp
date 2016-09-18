#include <QApplication>
#include "huffman.h"
#include "mainui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainUi *window = new MainUi();

    window->setFixedSize(500, 150);

    window->show();

    return app.exec();
}
