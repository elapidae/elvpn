#ifndef USERLOG_H
#define USERLOG_H

#include <QObject>
#include <QDateTime>
#include <QColor>

class UserLog
{
    static constexpr auto max_points = 60;

public:
    enum class Style { OpenVPN, IPSec };

    UserLog() {}
    UserLog( Style s, const QByteArray& id, const QByteArray& key, const QDateTime& from );

//    void set( const QByteArray& id, const QByteArray& key, const QDateTime& from,
//              QColor bg_color, QColor fg_color );

    void update( QDateTime ts, qulonglong sent, qulonglong received );
    void shift( QDateTime ts );

    QString text() const;

    QString get_key() const;
    QString get_ip() const;
    QString get_loc() const;
    QString get_from() const;
    QString get_recv() const;
    QString get_recv_spd() const;
    QString get_sent() const;
    QString get_sent_spd() const;

private:
    Style style;
    QByteArray id, key;
    QColor bg_color, fg_color;
    QDateTime from;

    struct TimePoint {
        QDateTime stamp;
        qulonglong sent = 0;
        qulonglong received = 0;
    };
    QList<TimePoint> stamps;
};

#endif // USERLOG_H
