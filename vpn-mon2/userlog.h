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

    UserLog( Style s, const QByteArray& id, const QByteArray& key, const QDateTime& from );

//    void set( const QByteArray& id, const QByteArray& key, const QDateTime& from,
//              QColor bg_color, QColor fg_color );

    void update( QDateTime ts, qulonglong sent, qulonglong received );

    QString text() const;

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
