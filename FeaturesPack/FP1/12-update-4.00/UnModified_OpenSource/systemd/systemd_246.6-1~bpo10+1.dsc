-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: systemd
Binary: systemd, systemd-sysv, systemd-container, systemd-journal-remote, systemd-coredump, systemd-timesyncd, systemd-tests, libpam-systemd, libnss-myhostname, libnss-mymachines, libnss-resolve, libnss-systemd, libsystemd0, libsystemd-dev, udev, libudev1, libudev-dev, udev-udeb, libudev1-udeb
Architecture: linux-any
Version: 246.6-1~bpo10+1
Maintainer: Debian systemd Maintainers <pkg-systemd-maintainers@lists.alioth.debian.org>
Uploaders: Michael Biebl <biebl@debian.org>, Marco d'Itri <md@linux.it>, Sjoerd Simons <sjoerd@debian.org>, Martin Pitt <mpitt@debian.org>, Felipe Sateler <fsateler@debian.org>
Homepage: https://www.freedesktop.org/wiki/Software/systemd
Standards-Version: 4.5.0
Vcs-Browser: https://salsa.debian.org/systemd-team/systemd
Vcs-Git: https://salsa.debian.org/systemd-team/systemd.git
Testsuite: autopkgtest
Testsuite-Triggers: acl, apparmor, build-essential, busybox-static, cron, cryptsetup-bin, dbus-user-session, dmeventd, dnsmasq-base, e2fsprogs, evemu-tools, fdisk, gcc, gdm3, iproute2, iputils-ping, isc-dhcp-client, kbd, less, libc-dev, libc6-dev, libcap2-bin, liblz4-tool, locales, make, net-tools, netcat-openbsd, network-manager, perl, pkg-config, plymouth, policykit-1, python3, qemu-system-arm, qemu-system-ppc, qemu-system-s390x, qemu-system-x86, quota, rsyslog, seabios, socat, strace, tree, util-linux, xserver-xorg, xserver-xorg-video-dummy, xz-utils, zstd
Build-Depends: debhelper-compat (= 13), pkg-config, xsltproc, docbook-xsl, docbook-xml, m4, meson (>= 0.49), gettext, gperf, gnu-efi [amd64 i386 arm64 armhf], libcap-dev (>= 1:2.24-9~), libpam0g-dev, libapparmor-dev (>= 2.13) <!stage1>, libidn2-dev <!stage1>, libiptc-dev <!stage1>, libaudit-dev <!stage1>, libdbus-1-dev (>= 1.3.2) <!nocheck> <!noinsttest>, libcryptsetup-dev (>= 2:1.6.0) <!stage1>, libselinux1-dev (>= 2.1.9), libacl1-dev, liblzma-dev, liblz4-dev (>= 0.0~r125), liblz4-tool <!nocheck>, libbz2-dev <!stage1>, zlib1g-dev <!stage1> | libz-dev <!stage1>, libcurl4-gnutls-dev <!stage1> | libcurl-dev <!stage1>, libmicrohttpd-dev <!stage1>, libgnutls28-dev <!stage1>, libpcre2-dev <!stage1>, libgcrypt20-dev, libkmod-dev (>= 15), libblkid-dev (>= 2.24), libmount-dev (>= 2.30), libseccomp-dev (>= 2.3.1) [amd64 arm64 armel armhf i386 mips mipsel mips64 mips64el x32 powerpc ppc64 ppc64el s390x], libdw-dev (>= 0.158) <!stage1>, libpolkit-gobject-1-dev <!stage1>, libzstd-dev (>= 1.4.0), linux-base <!nocheck>, acl <!nocheck>, python3:native, python3-lxml:native, python3-pyparsing <!nocheck>, python3-evdev <!nocheck>, tzdata <!nocheck>, libcap2-bin <!nocheck>, iproute2 <!nocheck>, zstd <!nocheck>
Package-List:
 libnss-myhostname deb admin optional arch=linux-any
 libnss-mymachines deb admin optional arch=linux-any
 libnss-resolve deb admin optional arch=linux-any
 libnss-systemd deb admin standard arch=linux-any
 libpam-systemd deb admin standard arch=linux-any
 libsystemd-dev deb libdevel optional arch=linux-any
 libsystemd0 deb libs optional arch=linux-any
 libudev-dev deb libdevel optional arch=linux-any
 libudev1 deb libs optional arch=linux-any
 libudev1-udeb udeb debian-installer optional arch=linux-any profile=!noudeb
 systemd deb admin important arch=linux-any
 systemd-container deb admin optional arch=linux-any profile=!stage1
 systemd-coredump deb admin optional arch=linux-any profile=!stage1
 systemd-journal-remote deb admin optional arch=linux-any profile=!stage1
 systemd-sysv deb admin important arch=linux-any
 systemd-tests deb admin optional arch=linux-any profile=!noinsttest
 systemd-timesyncd deb admin optional arch=linux-any
 udev deb admin important arch=linux-any
 udev-udeb udeb debian-installer optional arch=linux-any profile=!noudeb
Checksums-Sha1:
 c265bb961cfb0913a442cf9f83df64d2453a1b59 9545237 systemd_246.6.orig.tar.gz
 969fd99ce2236eb260dfe54fa2d43c8b2f567c30 154068 systemd_246.6-1~bpo10+1.debian.tar.xz
Checksums-Sha256:
 e999dbf0cff5b0109c28b307741b7dc315877fe2e1999f25c153548db44bb020 9545237 systemd_246.6.orig.tar.gz
 ca546bae3de97efd3d70487a37d0627edca8d34fcd970deb1dbf626168a54c44 154068 systemd_246.6-1~bpo10+1.debian.tar.xz
Files:
 a17b5e6b9e0aa1ac71587c05124e46e7 9545237 systemd_246.6.orig.tar.gz
 777d10c2803e505af970638dcc32e72c 154068 systemd_246.6-1~bpo10+1.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCAAdFiEECbOsLssWnJBDRcxUauHfDWCPItwFAl9s0v0ACgkQauHfDWCP
Itz0HhAAnug8fmH9Q6cyb95/YH1ZSta6flbXPRpxOwgYuRsrkZU16I2x4MbXDT0J
wtLPubJ+gUc1vrdC8+0/7rVDF6Icig40KrUJQQ13gdsOq+3sP10Eyc0Cx+LOxiAb
odlHQxPQsDlmRcZ+kCTwz6c/yNdMjY4uxr9cyWOrX8P7uaEjlwB4EP4QUkmKMmmR
/F0t24Nlsr6xtB3fGrA49T6+Bf7vgqV0RuCvrcBNDQuRj8g5NOj1FQqnyQHLSO2a
oDv1svky0ugBYmetB2lmb5cizPuuoC9/ULSK5TsxjSHkit28QqB9SuaXegFAlDbc
QsZmL1leOGZq3d2Bc5rEE8fhnNUvN6541mRYWmqkBVrPCEapiNAe1PTXKep9Nlzs
gvQswsqbMnFZuDnG48E4ySn7T1hAmBr1x+2J2p7NAARipxAjaKwy3YTo0dDa3YW3
Gw5kLnF63vY63scyvVqobewKkDKopxzt6XR4VEibVrAla0QE6iYEuneFjq78COk3
HMxYTPxNIqS9zon7hpy2qrK7gisRzBrUI9y37duovaaJrYkwGYEsr3Zpz41eNLzf
SAlYVXJcEaMe7uAK4+/3RWZAMYmp+FDE7Eye6z963z2wyjvmhB6ZSUygE1GMnoDP
ld64lDWaUnp+iinyABFLfSshT1UEEqbRKINz29mWeWWksPMrxIk=
=ErEZ
-----END PGP SIGNATURE-----
