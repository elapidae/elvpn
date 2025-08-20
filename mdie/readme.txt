https://dl.bobpony.com/windows/10/
https://github.com/dockur/windows/blob/master/src/define.sh#L720
https://hub.docker.com/r/dockurr/windows

#!/bin/bash -e

docker run -it --rm\
 --name windows\
 -p 8006:8006\
 --device=/dev/kvm\
 --device=/dev/net/tun\
 --cap-add NET_ADMIN\
 -v $(pwd)/win11:/storage\
 -v $(pwd)/xchg:/data\
 -v $(pwd)/en-us_windows_11_24h2_x64.iso:/boot.iso\
 --stop-timeout 120\
 dockurr/windows



In order to download an unsupported ISO image, specify its URL in the VERSION environment variable:

environment:
  VERSION: "https://example.com/win.iso"

Alternatively, you can also skip the download and use a local file instead, by binding it in your compose file in this way:

volumes:
  - ./example.iso:/boot.iso

 VERSION: "https://example.com/win.iso"

