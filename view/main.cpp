#include "View.h"
#include <QApplication>

View *v;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    v = new View();
    v->show();
	v->display_menu();

    return app.exec();
}
