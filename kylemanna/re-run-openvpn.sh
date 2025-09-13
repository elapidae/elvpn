#!/bin/bash -e

docker stop openvpn ||true
docker rm openvpn ||true

docker run \
--name openvpn \
-v $(pwd)/openvpn-data:/etc/openvpn \
--cap-add=NET_ADMIN \
--sysctl net.ipv6.conf.all.forwarding=1 \
--sysctl net.ipv6.conf.default.forwarding=1 \
-p 443:1194/tcp \
-d --restart unless-stopped \
kylemanna/openvpn

#--network stunnel-net \

