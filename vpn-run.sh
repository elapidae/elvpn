#!/bin/bash -e

docker run                                  \
    --name ipsec-vpn-server                 \
    --restart=always                        \
    -v /root/ikev2-vpn-data:/etc/ipsec.d    \
    -v /lib/modules:/lib/modules:ro         \
    -p 500:500/udp                          \
    -p 4500:4500/udp                        \
    -d --privileged                         \
    -e VPN_DNS_NAME=$(hostname --fqdn)      \
    --env-file vpn-env.txt                  \
    hwdsl2/ipsec-vpn-server
