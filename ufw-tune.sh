#!/bin/bash -e

ufw default deny

ufw allow 28078/tcp

ufw allow 500/udp    # IKE
ufw allow 4500/udp   # IPsec NAT-T

ufw enable

echo ============================
ufw status verbose
echo ============================
ufw status numbered
echo ============================

