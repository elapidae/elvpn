#include "openvpn.h"

#include <QSet>
#include <QDebug>

OpenVPN::OpenVPN(QObject *parent)
    : QObject{parent}
{

}

/*
=== IPSEC START ===

OpenVPN CLIENT LIST
Updated,Sat Sep 13 19:44:12 2025
Common Name,Real Address,Bytes Received,Bytes Sent,Connected Since
anna_mod1,5.77.197.112:58998,9125169,251966535,Sat Sep 13 19:05:51 2025
serob-third,176.59.170.36:33303,961699,10422354,Sat Sep 13 19:42:08 2025
ara-mob1,178.64.128.104:47600,1916582,745348,Sat Sep 13 17:20:51 2025
ROUTING TABLE
Virtual Address,Common Name,Real Address,Last Ref
192.168.255.10,serob-third,176.59.170.36:33303,Sat Sep 13 19:44:10 2025
192.168.255.6,ara-mob1,178.64.128.104:47600,Sat Sep 13 19:41:46 2025
192.168.255.14,anna_mod1,5.77.197.112:58998,Sat Sep 13 19:44:11 2025
GLOBAL STATS
Max bcast/mcast queue length,3
END
=== IPSEC END ===
*/
static auto OpenVPN_CLIENT_LIST = "OpenVPN CLIENT LIST";
static auto Updated = "Updated";

static auto ROUTING_TABLE = "ROUTING TABLE";
static auto GLOBAL_STATS = "GLOBAL STATS";
static auto END = "END";

static auto Common_Name     = "Common Name";
static auto Real_Address    = "Real Address";
static auto Bytes_Received  = "Bytes Received";
static auto Bytes_Sent      = "Bytes Sent";
static auto KW_Connected_Since = "Connected Since";


void OpenVPN::process(QList<QByteArray> lines)
{
    if ( lines.isEmpty() )
    {
        users.clear();
        return;
    }

    while (lines.takeFirst() != OpenVPN_CLIENT_LIST)
    {}

    // skip if any.
    auto upd = lines.takeFirst().split(',');
    auto heap = lines.takeFirst().split(',');

    decltype(users) new_users;
    while ( !lines.isEmpty() )
    {
        auto list = lines.takeFirst().split(',');
        if (list.first() == ROUTING_TABLE) break;

        auto& Common_Name     = list.at(0);
        auto& Real_Address    = list.at(1);
        auto& Bytes_Received  = list.at(2);
        auto& Bytes_Sent      = list.at(3);
        auto& connected_Since = list.at(4);

        QString connected = connected_Since.trimmed();
        connected = connected.replace("\"", "");
        connected = connected.simplified();

        auto id = Real_Address;
        auto key = Common_Name;

        auto now = QDateTime::currentDateTimeUtc();
        auto from = QDateTime::fromString(connected, "ddd MMM d HH:mm:ss yyyy");
        if ( !from.isValid() )
        {
            qDebug() << "from invalid:" << connected;
            from.setDate(QDate(1983,7,28));
        }

        QLocale locale(QLocale::English, QLocale::UnitedStates);
        QDateTime dt = locale.toDateTime(connected, "ddd MMM d HH:mm:ss yyyy");

        // Проверим успешность
        if (!dt.isValid()) {
            qDebug() << "Ошибка парсинга" << connected;
        } else {
            //qDebug() << dt.toString(Qt::ISODate);
        }

        auto sent = Bytes_Sent.toULongLong();
        auto recv = Bytes_Received.toULongLong();


        if ( users.count(id) )
        {
            new_users[id] = users[id];
        } else
        {
            if (key == "UNDEF") continue;
            new_users.emplace( id, UserLog(UserLog::Style::OpenVPN, id, key, from) );
        }
        new_users.find(id)->second.update( now, sent, recv );
    } // while lines

    users = new_users;
}
//=======================================================================================
void OpenVPN::shift( const QDateTime& ts )
{
    for ( auto & u: users )
    {
        u.second.shift( ts );
    }
}
//=======================================================================================
