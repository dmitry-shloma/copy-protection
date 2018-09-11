#include <QCoreApplication>
#include <QObject>

#include "sampleapp.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    SampleApp sampleApp;
    QObject::connect(&sampleApp, SIGNAL(done()), &app, SLOT(quit()));
    return app.exec();
}
