#!/bin/bash -e

kypath="../kylemanna"
rsapath="$kypath/openvpn-data/pki/issued/"

pykey_shared="/home/pykey-server/shared"
pykey_user="pykey-server"

find $rsapath -type f | while read -r file; do
    name=$(basename $file)
    name="${name%.crt}"   # убираем всё после последней точки (расширение)
    echo "Обрабатываю файл: $file $name"

    cur_path=$(pwd)
    cd $kypath

    ./get-key.sh $name #now key is $name.ovpn
    chown $pykey_user:$pykey_user $name.ovpn

    shadow=$(echo -n "sci-$name" | sha224sum | xxd -r -p| head -c 10 | base32)
    dst_path=$pykey_shared/$shadow
    mkdir -p $dst_path
    chown $pykey_user:$pykey_user $dst_path
    mv $name.ovpn $dst_path

    cd $cur_path

done

