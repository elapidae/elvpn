#ifndef OPENVPN_H
#define OPENVPN_H

#include <QObject>


#include "userlog.h"


class OpenVPN : public QObject
{
    Q_OBJECT
public:
    explicit OpenVPN(QObject *parent = nullptr);

    void process(QList<QByteArray> lines);

    std::map<QByteArray, UserLog> users;

    void shift( const QDateTime& ts );

signals:
    //void upd_user(QByteArray id, UserLog user);
    //void rm_user(QByteArray id);


private:
    //QByteArray updated;
};

#endif // OPENVPN_H
