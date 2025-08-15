#ifndef PARSER_H
#define PARSER_H

#include <QByteArray>
#include <QDateTime>

/*
ipsec whack --trafficstatus
#154: "ikev2-cp"[27] 5.149.159.35, type=ESP, add_time=1755279621, inBytes=90686439, outBytes=2012385143, maxBytes=2^63B, id='CN=vpnclient, O=IKEv2 VPN', lease=192.168.43.13/32
#161: "ikev2-cp"[29] 31.173.82.50, type=ESP, add_time=1755287453, inBytes=21042993, outBytes=649337317, maxBytes=2^63B, id='CN=vpnclient, O=IKEv2 VPN', lease=192.168.43.10/32
*/

struct LineData
{
    int num = 0;
    QByteArray conn_name;
    int session_id = 0;
    QByteArray ip;
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
    parser();

    static LineData parse(const QByteArray& line);
};


#endif // PARSER_H
