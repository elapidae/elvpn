#include "locator.h"

#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>


//=======================================================================================
static void err(QString s)
{
    qDebug() << s;
    exit(2);
}
//=======================================================================================
QByteArray locator::curl(QByteArray addr, QByteArray ip)
{
    auto cmd = QString("curl %1/%2 >delme-locate-ip 2>/dev/null")
            .arg(addr.data())
            .arg(ip.data());

    auto rcode = system(cmd.toStdString().c_str());
    if (rcode != 0) err("Cannot curl");

    QFile f("delme-locate-ip");
    if (!f.open(QIODevice::ReadOnly)) err("Cannot open");

    return f.readAll();
}
//=======================================================================================
QJsonDocument locator::locate(QByteArray addr, QByteArray ip)
{
    auto data = curl("ipinfo.io", ip);
    QJsonParseError jerr;
    auto res = QJsonDocument::fromJson(data,&jerr);
    if (jerr.error != QJsonParseError::NoError) err(jerr.errorString());
    return res;
}
//=======================================================================================
locator::Info locator::ipinfo_io(QByteArray ip)
{
    auto doc = locate("ipinfo.io", ip);
    auto obj = doc.object();

    Info res;
    res.ip     = obj["ip"].toString();
    res.city   = obj["city"].toString();
    res.region = obj["region"].toString();
    res._org   = obj["org"].toString();
    res.postal = obj["postal"].toString();

    return res;
}
//=======================================================================================


//=======================================================================================
QString locator::Info::org() const
{
    QString res = _org;
    if (res.startsWith("AS"))
    {
        while (res.at(0) != " ") res.remove(0, 1);
    }
    res = res.trimmed();

    if (res.startsWith("PJSC"))
    {
        while (res.at(0) != " ") res.remove(0, 1);
    }
    res = res.trimmed();

    return res;
}
//=======================================================================================
QString locator::Info::str(int fsize) const
{
    if (!owner.isEmpty())
    {
        auto text = QString("%1: %2 (%3)").arg(ip).arg(owner).arg(city);
        return QString("%1").arg(text, fsize);
    }

    auto text = QString("%1: %2 (%3)").arg(ip).arg(city).arg(org());
    return QString("%1").arg(text, fsize);
}
//=======================================================================================
