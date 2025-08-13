#!/bin/bash -e

mkdir -p vdsina-root
sshfs -p28078 root@$vdsina:/ vdsina-root

