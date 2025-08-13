#!/bin/bash -e

ufw allow 28078/tcp

ufw allow 500/udp    # IKE
ufw allow 4500/udp   # IPsec NAT-T
