#ifndef SAMPLEAPP_H
#define SAMPLEAPP_H

#include <QObject>
#include <iostream>

class SampleApp : public QObject
{
    Q_OBJECT
public:
    explicit SampleApp(QObject *parent = 0);
    ~SampleApp();

signals:
    void done();
};

#endif // SAMPLEAPP_H
