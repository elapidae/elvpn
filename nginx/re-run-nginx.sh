#!/bin/bash -e

docker stop nginx ||true
docker rm nginx ||true

docker run \
--name nginx \
-v $(pwd)/etc-nginx:/etc/nginx \
-v $(pwd)/srv-files:/srv/files \
-p 8081:80/tcp \
-d --restart unless-stopped \
nginx:alpine


