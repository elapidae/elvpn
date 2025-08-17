#include "reader.h"

#include <QFile>
#include <QTemporaryFile>
#include <QDebug>

LineData::vector reader::read()
{
    auto cmd = QString("docker exec -i ipsec-vpn-server ipsec whack --trafficstatus >delme-ipsec");
    auto rcode = system(cmd.toLatin1().data());

    if (rcode != 0) {
        qDebug() << "bad cont read";
        throw 45;
    }

    auto all = []
    {
        QFile f("delme-ipsec");
        if (!f.open(QIODevice::ReadOnly)) {
            throw "open file";
        }
        return f.readAll();
    }();

    return parser::parse_lines( all );
}
