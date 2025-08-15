#include <QCoreApplication>

#include <QRegExp>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString line = "#165: \"l2tp-psk\"[19] 5.77.192.216, type=ESP, add_time=1755292966, inBytes=292564, outBytes=1707394, maxBytes=2^63B, id='192.168.10.162'";

    QRegExp rx(
        "^#(\\d+):\\s+\"([^\"]+)\"\\[(\\d+)\\]\\s+"
        "(\\d{1,3}(?:\\.\\d{1,3}){3}),\\s+"
        "type=(\\w+),\\s+"
        "add_time=(\\d+),\\s+"
        "inBytes=(\\d+),\\s+"
        "outBytes=(\\d+),\\s+"
        "maxBytes=([^,]+),\\s+"
        "id='([^']+)',\\s+"
        "lease=([^ ]+)"
    );

    if (rx.indexIn(line) != -1) {
        qDebug() << "conn_id:"    << rx.cap(1);
        qDebug() << "name:"       << rx.cap(2);
        qDebug() << "name_id:"    << rx.cap(3);
        qDebug() << "remote_ip:"  << rx.cap(4);
        qDebug() << "type:"       << rx.cap(5);
        qDebug() << "add_time:"   << rx.cap(6);
        qDebug() << "in_bytes:"   << rx.cap(7);
        qDebug() << "out_bytes:"  << rx.cap(8);
        qDebug() << "max_bytes:"  << rx.cap(9);
        qDebug() << "id:"         << rx.cap(10);
        qDebug() << "lease:"      << rx.cap(11);
    } else {
        qDebug() << "Не удалось распарсить строку";
    }

    return a.exec();
}
