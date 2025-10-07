#include "userlog.h"

#include <QLocale>
#include <QDebug>

#include "ip_locator.h"

//=======================================================================================
UserLog::UserLog( Style s,
                  const QByteArray &id,
                  const QByteArray &key,
                  const QDateTime &from )
    : style( s )
    , id   ( id )
    , key  ( key )
    , from ( from )
{}
//=======================================================================================
void UserLog::update(QDateTime ts, qulonglong sent, qulonglong received)
{

    stamps << TimePoint{ts, sent, received};
    if ( stamps.size() > max_points )
    {
        stamps.removeFirst();
    }
}
//=======================================================================================
static QString key_id( UserLog::Style style, QByteArray key, QByteArray id )
{
    auto color = style == UserLog::Style::OpenVPN ? "#003399" : "#005522";
    return QString(R"(<div style="font-weight:bold; font-size:14px;"> %1[%2] </div>)")
            .arg(key.data()).arg(id.data());
}
//---------------------------------------------------------------------------------------
static QString key( UserLog::Style style, QByteArray id )
{
    auto color = style == UserLog::Style::OpenVPN ? "#003399" : "#005522";
    return QString(R"(<div style="font-weight:bold; font-size:15px;
                           color:%1">key: %2</div>)")
                    .arg(color)
                    .arg(id.data());
}
//---------------------------------------------------------------------------------------
static QString locate( UserLog::Style style, QByteArray id )
{
    auto ip = id.split(':').at(0);
    auto loc = Ip_Locator::info(ip);

    auto color = style == UserLog::Style::OpenVPN ? "#003399" : "#005522";
    return QString(R"(<div style="font-weight:bold; font-size:13px;
                           color:%1">loc: %2</div>)")
                    .arg(color)
                    .arg(loc.data());
}
//---------------------------------------------------------------------------------------
static QString speed(double spd)
{
    if ( spd != spd ) return "------ B/s"; // nan

    QString unit = "B/s";

    auto sp = "&nbsp;";

    if ( spd < 1'000. ) {
        auto res = QString("[%1 B/s]").arg(spd, 6, 'f', 2);
        return res.replace(' ', sp);
    }

    if ( spd < 1'000'000. ) {
        spd /= 1000;
        auto res = QString("[%1 K/s]").arg(spd, 6, 'f', 2);
        return res.replace(' ', sp);
    }

    if ( spd < 1'000'000'000. ) {
        spd /= 1000'000;
        auto res = QString("[%1 M/s]").arg(spd, 6, 'f', 2);
        return res.replace(' ', sp);
    }

    qDebug() << "Speed: " << spd;
    return QString("%1 B/s").arg(spd);
}
//---------------------------------------------------------------------------------------
static QString sent( UserLog::Style style, double _spd, quint64 total )
{
    static QLocale en(QLocale::English);
    auto tot = en.toString(total); // 123,456,789
    auto spd = speed(_spd);
    return QString(R"(<div><b>sent:</b> %1 / %2</div>)")
            .arg(spd).arg(tot);
}
//---------------------------------------------------------------------------------------
static QString recv( UserLog::Style style, double _spd, quint64 total )
{
    static QLocale en(QLocale::English);
    auto tot = en.toString(total); // 123,456,789
    auto spd = speed(_spd);
    return QString(R"(<div><b>recv:</b> %1 / %2</div>)")
            .arg(spd).arg(tot);
}
//---------------------------------------------------------------------------------------
static QString from( UserLog::Style style, QDateTime from )
{
    return QString(R"(<div><b>from:</b> %1 </div>)")
            .arg(from.toString(Qt::ISODate)).replace('T', ' ');
}
//---------------------------------------------------------------------------------------
static auto constexpr tmpl2_openvpn =
R"(<div style="border:2px solid #ccc; margin:0px; padding:0px; font-family:monospace;
        font-size:12px; color:#003399;">
  %1  %2  %3  %4  %5
</div>
)";
//---------------------------------------------------------------------------------------
static auto constexpr tmpl2_ipsec =
R"(<div style="border:1px solid #ccc; margin:4px; padding:4px; font-family:monospace;
        font-size:14px; color:#006600;">
  %1  %2  %3  %4  %5  %6
</div>
)";
//---------------------------------------------------------------------------------------
QString UserLog::text() const
{
    if ( stamps.isEmpty() ) return {};

    auto first = stamps.first();
    auto last = stamps.last();
    double d_sent = last.sent - first.sent;
    double d_recv = last.received - first.received;

    auto d_ms = last.stamp.toMSecsSinceEpoch() - first.stamp.toMSecsSinceEpoch();
    auto d_sec = d_ms / 1000.;

    auto spd_sent = d_sent / d_sec;
    auto spd_recv = d_recv / d_sec;

    auto tmpl = style == Style::OpenVPN ? tmpl2_openvpn : tmpl2_ipsec;
    return QString(tmpl)
            .arg( ::key_id(style,key,id)                    )
            //.arg( ::key(style,key)                          )
            .arg( ::locate(style,id)                        )
            .arg( ::sent(style, spd_sent, last.sent)        )
            .arg( ::recv(style, spd_recv, last.received)    )
            .arg( ::from(style,from)                        );
}
//=======================================================================================
