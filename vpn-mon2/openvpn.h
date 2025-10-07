#ifndef OPENVPN_H
#define OPENVPN_H

#include <QObject>
#include <QMap>

#include "userlog.h"


class OpenVPN : public QObject
{
    Q_OBJECT
public:
    explicit OpenVPN(QObject *parent = nullptr);

    void process(QList<QByteArray> lines);

    QMap<QByteArray, UserLog> users;


signals:
    //void upd_user(QByteArray id, UserLog user);
    //void rm_user(QByteArray id);


private:
    //QByteArray updated;
};

#endif // OPENVPN_H
