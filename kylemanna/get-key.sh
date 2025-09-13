#!/bin/bash -e



if [ -z "$1" ]; then
    echo "get user, pls"
    exit 1
fi

echo "user: $1"

user=$1

fuser="$(pwd)/$user.ovpn"
touch $fuser
echo "tun-mtu 1500" > $fuser
echo "mssfix 1450" >> $fuser
echo "" >> $fuser

docker run -v $(pwd)/openvpn-data:/etc/openvpn --rm kylemanna/openvpn ovpn_getclient $user >>$fuser

