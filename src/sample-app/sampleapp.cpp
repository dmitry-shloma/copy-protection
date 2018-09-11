#include "sampleapp.h"

SampleApp::SampleApp(QObject *parent) :
    QObject(parent)
{
    std::cout << "SampleApp is started..." << std::endl;
}

SampleApp::~SampleApp()
{
    std::cout << "SampleApp is finished..." << std::endl;
}
