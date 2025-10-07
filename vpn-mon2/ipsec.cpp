#include "ipsec.h"

#include <QDebug>

//=======================================================================================
IPSec::IPSec(QObject *parent)
    : QObject{parent}
{}
//=======================================================================================
void IPSec::process(QList<QByteArray> lines)
{
    QSet<QByteArray> cur_users;
    for ( auto &&ll: lines )
    {
        ll = ll.trimmed();
        if (ll.isEmpty()) continue;
        auto ip = parse_line( &ll );
        cur_users.insert( ip );
    }

    auto last_keys = users.keys();
    for ( auto && used: cur_users )
    {
        last_keys.removeOne(used);
    }

    for ( auto && hidden: last_keys )
    {
        users.remove( hidden );
    }
}
//=======================================================================================
static int parse_conn_id(QByteArray *s);
static std::tuple<QByteArray,int> parse_name_and_id(QByteArray *s);
static QByteArray parse_ip(QByteArray *s);
static QByteArray parse_type(QByteArray *s);
static QDateTime parse_add_time(QByteArray *s);
static size_t parse_inOutBytes( QByteArray *s, const char *io_label );
static QByteArray take_until(QByteArray *s, char until);
static QByteArray parse_id( QByteArray *s );
static QByteArray parse_lease( QByteArray *s );

static QByteArray parse_conn_name_id( QByteArray *line )
{
    auto conn_id = parse_conn_id( line );
    auto [name, name_id] = parse_name_and_id( line );
    return QString("%1/%2/%3")
            .arg(conn_id).arg(name.data()).arg(name_id)
            .toLatin1();
}
//---
QByteArray IPSec::parse_line( QByteArray *line )
{
    auto key = parse_conn_name_id( line );

    auto ip = parse_ip( line );
    auto type = parse_type( line );
    auto atime = parse_add_time( line );
    auto received = parse_inOutBytes( line, "inBytes" );
    auto sent = parse_inOutBytes( line, "outBytes" );
    //auto maxBytes = take_until( line, ',' );
    //auto id = parse_id( line );
    //auto lease = parse_lease( line );
    //---------------------------------------------------
    auto user_it = users.find(ip);
    if ( user_it == users.end() ) {
        users.insert(ip, UserLog( UserLog::Style::IPSec, ip, key, atime) );
    }
    auto now = QDateTime::currentDateTimeUtc();
    users.find(ip).value().update(now, sent, received);

    return ip;
}
//=======================================================================================


//=======================================================================================
static void err(QString s)
{
    qDebug() << s;
    exit(1);
}
//=======================================================================================
static long long parse_num(QByteArray *s)
{
    QByteArray n;
    while (!s->isEmpty())
    {
        auto ch = s->at(0);
        auto isch = ch >= '0' && ch <= '9';
        if (!isch) break;
        n.push_back(ch);
        s->remove(0, 1);
    }
    return n.toLongLong();
}
//=======================================================================================
static void trim(QByteArray *s)
{
    *s = s->trimmed();
}
//=======================================================================================
static void take( QByteArray *a, char ch )
{
    if (a->isEmpty())
        err("exp nonempty");
    if (a->at(0) != ch) err(QString("exp ") + ch);
    a->remove(0, 1);
    trim(a);
}
//=======================================================================================
static QByteArray parse_quotes(QByteArray *s, char quote)
{
    QByteArray res;
    take(s, quote);
    while (s->at(0) != quote)
    {
        res += s->at(0);
        s->remove(0, 1);
    }
    take(s, quote);
    return res;
}
//=======================================================================================
static QByteArray take_until(QByteArray *s, char until)
{
    QByteArray res;
    while (!s->isEmpty())
    {
        auto ch = s->at(0);
        if (ch == until) break;
        res += ch;
        s->remove(0, 1);
    }
    take(s, until);
    trim(s);
    return res;
}
//=======================================================================================
static int parse_conn_id(QByteArray *s)
{
    take(s, '#');
    auto res = parse_num(s);
    take(s, ':');
    trim(s);
    return res;
}
//=======================================================================================
static std::tuple<QByteArray,int> parse_name_and_id(QByteArray *s)
{
    auto name = parse_quotes(s, '"');
    take(s, '[');
    auto id = parse_num(s);
    take(s, ']');
    trim(s);
    return {name, id};
}
//=======================================================================================
static QByteArray parse_ip(QByteArray *s)
{
    return take_until(s, ',');
}
//=======================================================================================
static QByteArray parse_type(QByteArray *s)
{
    auto res = take_until(s, ',');
    trim(s);
    return res;
}
//=======================================================================================
static QDateTime parse_add_time(QByteArray *s)
{
    auto f = take_until(s, ',');
    auto lv = f.split('=');
    return QDateTime::fromSecsSinceEpoch(lv.at(1).toULongLong(), Qt::UTC);
}
//=======================================================================================
static size_t parse_inOutBytes( QByteArray *s, const char *io_label )
{
    auto f = take_until(s, ',');
    auto lv = f.split('=');

    if (lv.at(0) != io_label) err("not some bytes");
    return lv.at(1).toULongLong();
}
//=======================================================================================
static QByteArray parse_id( QByteArray *s )
{
    if ( !s->startsWith("id=") ) err("not id=");
    s->remove(0, 3);
    auto res = parse_quotes( s, '\'' );

    if (!s->isEmpty() && s->at(0) == ',')
        take(s, ',');

    return res;
}
//=======================================================================================
static QByteArray parse_lease( QByteArray *s )
{
    if (s->isEmpty()) return {};

    if (!s->startsWith("lease=")) err("not lease");
    s->remove(0, 6);

    auto res = *s;
    s->clear();
    return res;
}
//=======================================================================================


//=======================================================================================
//LineData parser::parse(QByteArray line)
//{
//    LineData res {};

//    res.conn_id = parse_conn_id( &line );
//    parse_name_and_id(&line, &res);
//    res.ip_ = ip(&line);
//    res.type = parse_type( &line );
//    res.add_time = parse_add_time( &line );

//    res.in_bytes = parse_inOutBytes( &line,  "in"  );
//    res.out_bytes = parse_inOutBytes( &line, "out" );

//    auto maxBytes = take_until( &line, ',' );

//    res.id = parse_id( &line );
//    res.lease = parse_lease( &line );

//    return res;
//}
//=======================================================================================
//LineData::vector parser::parse_lines(const QByteArray& lines)
//{
//    LineData::vector res;
//    auto nlines = lines.split('\n');
//    for (auto &&ll: nlines)
//    {
//        ll = ll.trimmed();
//        if (ll.isEmpty()) continue;
//        res.push_back( parse(ll) );
//    }
//    return res;
//}
//=======================================================================================
static QString to_str(QDateTime dt)
{
    return dt.toString("yy-MM-dd_hh:mm:ss");
}
//=======================================================================================
//QString LineData::asLine() const
//{
//    auto now = QDateTime::currentDateTimeUtc();

//    return QString("%1 | %2 | %3 | %4 | %5 | %6 | id=%7")
//            .arg(to_str(now))
//            .arg(cnn())
//            .arg(ip())
//            .arg(time())
//            .arg(inCount())
//            .arg(outCount())
//            .arg(id.data())
//            ;
//}
//=======================================================================================
//QString LineData::asLineLocate() const
//{
//    auto now = QDateTime::currentDateTimeUtc();

//    auto lip = locator::ipinfo_io(ip_).str();
//    return QString("%1 | %2 | %3 | %4 | %5 | %6 | id=%7")
//            .arg(to_str(now))
//            .arg(cnn())
//            .arg(lip)
//            .arg(time())
//            .arg(inCount())
//            .arg(outCount())
//            .arg(id.data())
//            ;
//}
//=======================================================================================
//QString LineData::cnn(int len) const
//{
//    auto res = QString("%1/%2/%3").arg(conn_id).arg(name.data()).arg(name_id);
//    return QString("%1").arg(res, len);
//}
//=======================================================================================
//QString LineData::ip(int len) const
//{
//    return QString("%1").arg(ip_.data(), len);
//}
//=======================================================================================
//QString LineData::time(int len) const
//{
//    auto dt = to_str(add_time);
//    return QString("%1").arg(dt, len);
//}
//=======================================================================================
static QString as_sample(long long val, int len = 15)
{
    auto bytes = val % 1000;
    val /= 1000;

    auto kb = val % 1000;
    val /= 1000;

    auto mb = val % 1000;
    val /= 1000;

    auto gb = val % 1000;
    val /= 1000;
    if (val != 0) err("val is too big");


    QString res = QString("%1").arg(bytes,3,10,QChar('0'));
    if (kb) {
        res = QString("%1'").arg(kb,3,10,QChar('0')) + res;
    }
    if (mb) {
        res = QString("%1'").arg(mb,3,10,QChar('0')) + res;
    }
    if (gb) {
        res = QString("%1'").arg(gb,3,10,QChar('0')) + res;
    }

    while (!res.isEmpty() && res.at(0) == '0')
    {
        res.remove(0,1);
    }

    while (res.size() < len)
    {
        res = ' ' + res;
    }

    return res;
}
//=======================================================================================
//QString LineData::inCount(int len) const
//{
//    return QString("in:%1").arg(as_sample(in_bytes, len-3), len);
//}
//=======================================================================================
//QString LineData::outCount(int len) const
//{
//    return QString("out:%1").arg(as_sample(out_bytes, len-4), len);
//}
//=======================================================================================
