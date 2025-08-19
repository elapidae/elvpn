#include <QCoreApplication>

#include <QRegExp>
#include <QDebug>
#include "reader.h"
#include "locator.h"

#include <iostream>


int gen_func(int argc, char *argv[]);
int main(int argc, char *argv[])
{
    return gen_func(argc, argv);

    auto ip1 = "178.67.193.41";
    auto ip2 = "178.71.108.49";

    qDebug() << locator::ipinfo_io(ip1).str();
    qDebug() << locator::ipinfo_io(ip2).str();

    return 0;



    QString line = "#165: \"l2tp-psk\"[19] 5.77.192.216, type=ESP, add_time=1755292966, inBytes=292564, outBytes=1707394, maxBytes=2^63B, id='192.168.10.162'";

    QString line2 = "#165: \"l2tp-psk\"[19] 5.77.192.216, type=ESP, add_time=1755292966, inBytes=292564, outBytes=1707394, maxBytes=2^63B, id='192.168.10.162', lease=192.168.43.10/32";

    QString line3 = R"(#198: "ikev2-cp"[43] 5.149.159.35, type=ESP, add_time=1755380677, inBytes=3617033, outBytes=26231783, maxBytes=2^63B, id='CN=vpnclient, O=IKEv2 VPN', lease=192.168.43.13/32)";

    auto l = parser::parse(line3.toLatin1());

    qDebug() << l.conn_id;
    qDebug() << l.name;
    qDebug() << l.name_id;
    qDebug() << l.ip();
    qDebug() << l.add_time;
    qDebug() << l.in_bytes;
    qDebug() << l.out_bytes;
    qDebug() << l.id;
    qDebug() << l.lease;

    return 0;
}

int gen_func(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    try {
        auto list = reader::read();
        for (auto && e: list)
        {
            std::cout << e.asLineLocate().toStdString() << std::endl;
        }
    }  catch (int i) {
        qDebug() << i;
    }
    return 0;
}
