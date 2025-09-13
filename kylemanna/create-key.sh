#!/bin/bash -e



if [ -z "$1" ]; then
    echo "get user, pls"
    exit 1
fi

docker run -v $(pwd)/openvpn-data:/etc/openvpn --rm -it kylemanna/openvpn easyrsa build-client-full $1 $2
