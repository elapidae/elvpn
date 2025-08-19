#ifndef PARSER_H
#define PARSER_H

#include <QByteArray>
#include <QDateTime>

#include <vector>

/*
ipsec whack --trafficstatus
#154: "ikev2-cp"[27] 5.149.159.35, type=ESP, add_time=1755279621, inBytes=90686439, outBytes=2012385143, maxBytes=2^63B, id='CN=vpnclient, O=IKEv2 VPN', lease=192.168.43.13/32
#161: "ikev2-cp"[29] 31.173.82.50, type=ESP, add_time=1755287453, inBytes=21042993, outBytes=649337317, maxBytes=2^63B, id='CN=vpnclient, O=IKEv2 VPN', lease=192.168.43.10/32
*/

struct LineData
{
    QString asLine() const;
    QString asLineLocate() const;

    // conn / name /name_id
    QString cnn(int len = 16) const;
    QString ip(int len = 16) const;
    QString time(int len = 17) const;
    QString inCount(int len = 14) const;
    QString outCount(int len = 15) const;

    using vector = std::vector<LineData>;

    int conn_id = 0;
    QByteArray name;
    int name_id = 0;
    QByteArray ip_;
    QByteArray type;
    QDateTime add_time;
    size_t in_bytes = 0;
    size_t out_bytes = 0;
    QByteArray id;
    QByteArray lease;
};




class parser
{
public:

    static LineData parse(QByteArray line);
    static LineData::vector parse_lines(const QByteArray& lines);
};


#endif // PARSER_H
