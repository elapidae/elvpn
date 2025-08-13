#!/bin/bash -e

docker stop ipsec-vpn-server || true
docker rm ipsec-vpn-server || true

docker run                                  \
    --name ipsec-vpn-server                 \
    --restart=always                        \
    -v /root/ikev2-vpn-data:/etc/ipsec.d    \
    -v /lib/modules:/lib/modules:ro         \
    -p 500:500/udp                          \
    -p 4500:4500/udp                        \
    --privileged                            \
    -e VPN_DNS_NAME=$(hostname --fqdn)      \
    --env-file vpn-env.txt                  \
    -d \
    hwdsl2/ipsec-vpn-server
