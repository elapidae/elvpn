#ifndef IPSEC
#define IPSEC

#include <QObject>
#include <QMap>

#include "userlog.h"


class IPSec : public QObject
{
     Q_OBJECT
public:
    explicit IPSec(QObject *parent = nullptr);

    void process(QList<QByteArray> lines);

    QMap<QByteArray, UserLog> users;
signals:

private:
    QByteArray parse_line(QByteArray *line);
};

#endif // IPSEC
