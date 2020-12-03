-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: mariadb-10.3
Binary: libmariadb-dev, libmariadbclient-dev, libmariadb-dev-compat, libmariadb3, libmariadbd19, libmariadbd-dev, mariadb-common, mariadb-client-core-10.3, mariadb-client-10.3, mariadb-server-core-10.3, mariadb-server-10.3, mariadb-server, mariadb-client, mariadb-backup, mariadb-plugin-connect, mariadb-plugin-rocksdb, mariadb-plugin-oqgraph, mariadb-plugin-tokudb, mariadb-plugin-mroonga, mariadb-plugin-spider, mariadb-plugin-gssapi-server, mariadb-plugin-gssapi-client, mariadb-plugin-cracklib-password-check, mariadb-test, mariadb-test-data
Architecture: any all
Version: 1:10.3.23-0+deb10u1
Maintainer: Debian MySQL Maintainers <pkg-mysql-maint@lists.alioth.debian.org>
Uploaders: Otto Kekäläinen <otto@debian.org>
Homepage: https://mariadb.org/
Standards-Version: 4.2.1
Vcs-Browser: https://salsa.debian.org/mariadb-team/mariadb-10.3
Vcs-Git: https://salsa.debian.org/mariadb-team/mariadb-10.3.git
Testsuite: autopkgtest
Build-Depends: bison, chrpath, cmake, cracklib-runtime, debhelper (>= 9.20160709), dh-apparmor, dh-exec, gdb, libaio-dev [linux-any], libarchive-dev, libboost-dev, libcrack2-dev (>= 2.9.0), libgnutls28-dev (>= 3.3.24), libjemalloc-dev [linux-any], libjudy-dev, libkrb5-dev, liblz4-dev, libncurses5-dev (>= 5.0-6~), libpam0g-dev, libpcre3-dev (>= 2:8.35-3.2~), libreadline-gplv2-dev, libreadline-gplv2-dev:native, libsnappy-dev, libsystemd-dev [linux-any], libxml2-dev, libzstd-dev (>= 1.3.3), lsb-release, perl, po-debconf, psmisc, unixodbc-dev, zlib1g-dev (>= 1:1.1.3-5~)
Package-List:
 libmariadb-dev deb libdevel optional arch=any
 libmariadb-dev-compat deb libdevel optional arch=any
 libmariadb3 deb libs optional arch=any
 libmariadbclient-dev deb oldlibs optional arch=any
 libmariadbd-dev deb libdevel optional arch=any
 libmariadbd19 deb libs optional arch=any
 mariadb-backup deb database optional arch=any
 mariadb-client deb database optional arch=all
 mariadb-client-10.3 deb database optional arch=any
 mariadb-client-core-10.3 deb database optional arch=any
 mariadb-common deb database optional arch=all
 mariadb-plugin-connect deb database optional arch=any
 mariadb-plugin-cracklib-password-check deb database optional arch=any
 mariadb-plugin-gssapi-client deb database optional arch=any
 mariadb-plugin-gssapi-server deb database optional arch=any
 mariadb-plugin-mroonga deb database optional arch=any-alpha,any-amd64,any-arm,any-arm64,any-i386,any-ia64,any-mips64el,any-mips64r6el,any-mipsel,any-mipsr6el,any-nios2,any-powerpcel,any-ppc64el,any-sh3,any-sh4,any-tilegx
 mariadb-plugin-oqgraph deb database optional arch=any
 mariadb-plugin-rocksdb deb database optional arch=amd64,arm64,mips64el,ppc64el
 mariadb-plugin-spider deb database optional arch=any
 mariadb-plugin-tokudb deb database optional arch=amd64
 mariadb-server deb database optional arch=all
 mariadb-server-10.3 deb database optional arch=any
 mariadb-server-core-10.3 deb database optional arch=any
 mariadb-test deb database optional arch=any
 mariadb-test-data deb database optional arch=all
Checksums-Sha1:
 c95b6d4cff5e6d63eed05da20561802b9c83e717 72582611 mariadb-10.3_10.3.23.orig.tar.gz
 a989ae4b2613d8fdd418078f527757aa72730654 195 mariadb-10.3_10.3.23.orig.tar.gz.asc
 1eba360573a6e252f16e9525f68fc21608249843 223744 mariadb-10.3_10.3.23-0+deb10u1.debian.tar.xz
Checksums-Sha256:
 fc405022457d8eec5991b870cc1c9a07b83b551d6165c414c4d8f31523aa86ae 72582611 mariadb-10.3_10.3.23.orig.tar.gz
 641e4d384fca5a93a2382b6d522881c6076e72c201afaf8d6a470d6e9c2b6b12 195 mariadb-10.3_10.3.23.orig.tar.gz.asc
 d447215b78567b2efa40af745fc641e47debbb8d4d304e31ba2efbb05dbbfefd 223744 mariadb-10.3_10.3.23-0+deb10u1.debian.tar.xz
Files:
 473950893d29805d9384ec0ed5d7c276 72582611 mariadb-10.3_10.3.23.orig.tar.gz
 95c707deba220fbe16afb590e9ba933c 195 mariadb-10.3_10.3.23.orig.tar.gz.asc
 4a24a2409464c1a54cc58054d8e9c530 223744 mariadb-10.3_10.3.23-0+deb10u1.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEEmbRSsR88dMO0U+RvvthEn87o2ogFAl8IugMACgkQvthEn87o
2ohZKRAAxMxRDPRWUiacYiTmgTirVZHSsOiZii4zyZjJ01921JRkydmKLXwPrNAs
0CGkFvKl25Fb9zLyJkbOdYVxz0ooCwzuPLcpdpFJKQxyxJ+sCOuKzk7D8EHjn7U7
h792/ba+O8cq7AboA5z+oDMRGH+nSD5qxvkNFAmOO7gyGnB79jQD3qlPIovJ3sTB
uB+HiiQcu+2zqRaHsaj5hCjJA9BeYBOwIZfdf008YGh8HJfIACqZa1iH5SXOAUg5
YUyQRQM0o020c3FBBP/AuNHiEH6Aq5qrosQLBB8jsy+5I6odii2RAAmnhwznSVCH
hiSoelAt9KRpgKIHy07/QADmVyDRv1CgygFKQ8XLDaoANELMlTp1YmnA2dxjLGHR
6I3iyvhK5oWxQF7Z0Q/CzkajaSKpbK0VBjy9GDn1fF7+yiX6jk9TdKOoO/RF6QQw
WAfUX9BwZI3UknLoJCUDvVHFEaKhmyuhm74hlMdZL5RPn/dhDWF9eTZMEp4ATZ4d
5RfUPqfs3xoCdENyvxEKnAlRQC0GX8zXYZO+mnRZaKWuPBT9KM0+K7hyGQa94k/5
zzR/DF5NdxtYisprkOOli733pCfz+y1cM0Ah0eAffWDRYHP6uxoXwdBsKdfVTYKT
WJuM7Xjm/jxnnIaufHHPIIDNw8DZDndNmWwI5gpYgDpMQ9WoQ+I=
=vyWp
-----END PGP SIGNATURE-----
