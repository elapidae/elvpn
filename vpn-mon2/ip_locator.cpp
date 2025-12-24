#include "ip_locator.h"

#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlRecord>

#define qdeb qDebug() << __LINE__ << ": "

//=======================================================================================
static Ip_Locator& singleton()
{
    static auto res = Ip_Locator();
    return res;
}

//=======================================================================================
QByteArray Ip_Locator::info(QByteArray ip, const QByteArray& key)
{
    return singleton().locate(ip, key);
}
//=======================================================================================


//=======================================================================================
Ip_Locator::Ip_Locator( QString path )
{
    db = QSqlDatabase::addDatabase( "QSQLITE", "ip-locator" );

    if ( path.isEmpty() ) path = "./";
    db.setDatabaseName( path + "/ipinfo-io.sqlite" );

    if ( !db.open() ) {
        throw 42;
    }

    auto q = db.exec("CREATE TABLE IF NOT EXISTS ipinfo "
                     "(key,stamp,ip,city,region,org,postal)");
}
//=======================================================================================
Ip_Locator::~Ip_Locator()
{
    db.close();
    QSqlDatabase::removeDatabase( "ip-locator" );
}
//=======================================================================================
static QByteArray curl(QByteArray addr, QByteArray ip);
static void err(QString s);
//------------------------------------------------------------------------
QByteArray Ip_Locator::locate(const QByteArray &ip, const QByteArray &key)
{
    if ( !has(ip) )
    {
        add_to_base( ip, key );
    }

    return from_base( ip );
}
//=======================================================================================
static void err(QString s)
{
    qdeb << s;
    exit(1);
}
//---------------------------------------------------------------------------------------
static QByteArray curl(QByteArray addr, QByteArray ip)
{
    auto cmd = QString("curl %1/%2 >/tmp/delme-locate-ip 2>/dev/null")
            .arg(addr.data())
            .arg(ip.data());

    auto rcode = system(cmd.toStdString().c_str());
    if (rcode != 0) err("Cannot curl");

    QFile f("/tmp/delme-locate-ip");
    if (!f.open(QIODevice::ReadOnly)) err("Cannot open");

    return f.readAll();
}
//=======================================================================================
bool Ip_Locator::has( const QByteArray &ip )
{
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) from ipinfo WHERE ip=:ip");
    q.bindValue(":ip", QString(ip));
    if ( !q.exec() ) throw 42;
    if ( !q.next() ) throw 43;

    auto count = q.value(0).toString().toInt(); // need to string because different types
    return count > 0;
}
//=======================================================================================
static QString clear_org(QString _org)
{
    QString res = _org;
    if (res.startsWith("AS"))
    {
        while (res.at(0) != " ") res.remove(0, 1);
    }
    res = res.trimmed();

    res = res.replace("PJSC", "");
    res = res.replace("Ltd",  "");
    res = res.replace("CJSC", "");
    res = res.replace("LLC",  "");
    res = res.replace(".",    "");
    res = res.replace("Limited", "");
    res = res.replace("Company", "");

    res = res.trimmed();

    return res;
}
//=======================================================================================
QByteArray Ip_Locator::from_base( const QByteArray &ip )
{
    QSqlQuery q(db);
    q.prepare("SELECT ip,city,region,org,postal,key,stamp FROM ipinfo WHERE ip=:ip");
    q.bindValue( ":ip", QString(ip) );
    q.exec();
    q.next();

    auto city = q.value("city").toString();
    auto org = q.value("org").toString();

    if (city.toLower() == "saint petersburg") {
        city = "SPb";
    }

    return QString("%1 (%2)")
            .arg(city)
            .arg(clear_org(org)).toLatin1();
}
//=======================================================================================
void Ip_Locator::add_to_base( const QByteArray &ip_ , const QByteArray& key )
{
    auto received = curl( "ipinfo.io", ip_ );

    QJsonParseError jerr;
    auto doc = QJsonDocument::fromJson(received,&jerr);
    if (jerr.error != QJsonParseError::NoError) err(jerr.errorString());

    auto obj = doc.object();

    auto ip     = obj["ip"].toString();
    auto city   = obj["city"].toString();
    auto region = obj["region"].toString();
    auto org    = obj["org"].toString();
    auto postal = obj["postal"].toString();

    auto stamp = QDateTime::currentDateTimeUtc();

    db.transaction();

    QSqlQuery q(db);
    q.prepare("INSERT INTO ipinfo ( key, stamp, ip, city, region, org, postal) VALUES "
                                 "(:key,:stamp,:ip,:city,:region,:org,:postal)");

    q.bindValue(":key",    QString(key));
    q.bindValue(":stamp",  stamp.toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":ip",     ip);
    q.bindValue(":city",   city);
    q.bindValue(":region", region);
    q.bindValue(":org",    org);
    q.bindValue(":postal", postal);

    if ( !q.exec() )
    {
        qdeb << "add to" << ip_ << "bad";
        throw 43;
    }

    db.commit();

    qdeb << "add to base" << ip_;
}
//=======================================================================================


