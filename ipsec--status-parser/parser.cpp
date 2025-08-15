#include "parser.h"

#include <QRegExp>

/*
ipsec whack --trafficstatus
#154: "ikev2-cp"[27] 5.149.159.35, type=ESP, add_time=1755279621, inBytes=90686439, outBytes=2012385143, maxBytes=2^63B, id='CN=vpnclient, O=IKEv2 VPN', lease=192.168.43.13/32
#161: "ikev2-cp"[29] 31.173.82.50, type=ESP, add_time=1755287453, inBytes=21042993, outBytes=649337317, maxBytes=2^63B, id='CN=vpnclient, O=IKEv2 VPN', lease=192.168.43.10/32

#154: "ikev2-cp"[27] 5.149.159.35, type=ESP, add_time=1755279621, inBytes=92941552, outBytes=2098679266, maxBytes=2^63B, id='CN=vpnclient, O=IKEv2 VPN', lease=192.168.43.13/32
#161: "ikev2-cp"[29] 31.173.82.50, type=ESP, add_time=1755287453, inBytes=22054033, outBytes=680672599, maxBytes=2^63B, id='CN=vpnclient, O=IKEv2 VPN', lease=192.168.43.10/32
#165: "l2tp-psk"[19] 5.77.192.216, type=ESP, add_time=1755292966, inBytes=292564, outBytes=1707394, maxBytes=2^63B, id='192.168.10.162', lease=192.168.43.10/32


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
*/

//=======================================================================================
LineData parser::parse(const QByteArray &line)
{
    auto fields = line.split(' ');
    LineData res {};

    auto rnum = fields.at(0);

    QRegExp
}
//=======================================================================================
