#!/bin/bash -e

kypath="."
rsapath="$kypath/openvpn-data/pki/issued/"

pykey_path="/home/pykey-server"
pykey_shared="$pykey_path/shared"

host=$(hostname -f):8000

echo host: $host
echo shared: $pykey_shared

find $pykey_shared -type f | while read -r file; do
    url="http://${file/$pykey_shared/$host}"
    echo $url
done

