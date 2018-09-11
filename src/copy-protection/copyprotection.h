#ifndef COPYPROTECTION_H
#define COPYPROTECTION_H

#include <QObject>

class CopyProtection : public QObject
{
    Q_OBJECT
public:
    explicit CopyProtection(QObject *parent = 0);

signals:

public slots:

};

#endif // COPYPROTECTION_H
