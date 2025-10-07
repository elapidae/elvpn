#ifndef IP_LOCATOR_H
#define IP_LOCATOR_H

#include <QSqlDatabase>

class Ip_Locator
{
public:
    static void need_path(QString);
    static QByteArray info(QByteArray ip);


    Ip_Locator(QString path = ".");
    virtual ~Ip_Locator();

    QByteArray locate(const QByteArray& ip);


private:

    bool has(const QByteArray &ip);
    QByteArray from_base(const QByteArray &ip);
    void add_to_base(const QByteArray &ip);

    QSqlDatabase db;
    QString _db_name;
};

#endif // IP_LOCATOR_H
