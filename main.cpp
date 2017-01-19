#include <QApplication>
#include "DeformDialog.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    DeformDialog deformDialog;
    deformDialog.show();
    return app.exec();
}
