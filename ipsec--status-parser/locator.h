#ifndef LOCATOR_H
#define LOCATOR_H

#include <QJsonDocument>

class locator
{
public:
    struct Info
    {
        QString ip, city, region, _org, postal;
        QString owner;

        QString org() const;
        QString str(int fsize = 47) const;
    };

    locator();

    static QByteArray curl(QByteArray addr, QByteArray ip);
    static QJsonDocument locate(QByteArray addr, QByteArray ip);

    static Info ipinfo_io(QByteArray ip);
};

#endif // LOCATOR_H
