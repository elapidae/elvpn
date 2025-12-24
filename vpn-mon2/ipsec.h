#ifndef IPSEC
#define IPSEC

#include <QObject>

#include "userlog.h"


class IPSec : public QObject
{
     Q_OBJECT
public:
    explicit IPSec(QObject *parent = nullptr);

    void process( QList<QByteArray> lines);

    void shift( const QDateTime& ts );

    std::map<QByteArray, UserLog> users;

signals:
    void update(const UserLog&);
    void remove(const QByteArray& key);

private:
    //QByteArray parse_line(QByteArray *line);
};

#endif // IPSEC
