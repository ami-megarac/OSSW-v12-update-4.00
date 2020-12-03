-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: openssl
Binary: openssl, libssl1.1, libcrypto1.1-udeb, libssl1.1-udeb, libssl-dev, libssl-doc
Architecture: any all
Version: 1.1.1d-0+deb10u3
Maintainer: Debian OpenSSL Team <pkg-openssl-devel@lists.alioth.debian.org>
Uploaders: Christoph Martin <christoph.martin@uni-mainz.de>, Kurt Roeckx <kurt@roeckx.be>, Sebastian Andrzej Siewior <sebastian@breakpoint.cc>
Homepage: https://www.openssl.org/
Standards-Version: 4.2.1
Vcs-Browser: https://salsa.debian.org/debian/openssl
Vcs-Git: https://salsa.debian.org/debian/openssl.git
Testsuite: autopkgtest
Testsuite-Triggers: perl
Build-Depends: debhelper (>= 11), m4, bc, dpkg-dev (>= 1.15.7)
Package-List:
 libcrypto1.1-udeb udeb debian-installer optional arch=any
 libssl-dev deb libdevel optional arch=any
 libssl-doc deb doc optional arch=all
 libssl1.1 deb libs optional arch=any
 libssl1.1-udeb udeb debian-installer optional arch=any
 openssl deb utils optional arch=any
Checksums-Sha1:
 056057782325134b76d1931c48f2c7e6595d7ef4 8845861 openssl_1.1.1d.orig.tar.gz
 d3bbfe1db19cc36bb17f2b6dc39fa8ade6a8cdd3 488 openssl_1.1.1d.orig.tar.gz.asc
 c381e35c050ab5e7fcb8b307f8c572ae96882b6a 86692 openssl_1.1.1d-0+deb10u3.debian.tar.xz
Checksums-Sha256:
 1e3a91bc1f9dfce01af26026f856e064eab4c8ee0a8f457b5ae30b40b8b711f2 8845861 openssl_1.1.1d.orig.tar.gz
 f3fd3299a79421fffd51d35f62636b8e987dab1d3033d93a19d7685868e15395 488 openssl_1.1.1d.orig.tar.gz.asc
 59db3dc3bf8e8abee0dc6dd6c62b644e57ac7a0e3ab98ace563885a4f3b205cd 86692 openssl_1.1.1d-0+deb10u3.debian.tar.xz
Files:
 3be209000dbc7e1b95bcdf47980a3baa 8845861 openssl_1.1.1d.orig.tar.gz
 56a525b2d934330e1c2de3bc9b55e4e2 488 openssl_1.1.1d.orig.tar.gz.asc
 27858f2e358a1f88f3f84232fef776ac 86692 openssl_1.1.1d-0+deb10u3.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQGzBAEBCgAdFiEEV4kucFIzBRM39v3RBWQfF1cS+lsFAl6eBrEACgkQBWQfF1cS
+ls7RAwAh5J4/wwziIiJAzSIPyVF9M/WHZQ0hMSZU7WOxvmyKdILIZEBJXEoPp6D
JbK3uiP7bT6LXEuNrzbqzthD/geicLLOsmBDjt1isLhinYgB7Mz4WhhBkQTupGOT
ULEskfMtwjCg1sOkpy3Y+sGutZRRrn08eQBsaJpe/pVKBrDauss6AIKdPVOi/Tvv
0PVuYtjUcnjAJknRDWbuE8MrHEJvvVeX1ZwXjVY7EEM0foqr0oYKUy2LzD4GrIqq
iZsufK5/G3YpCFRwNELLAKIyDYFFUNcnxd/S4MPnU7h0Afv6ymuw9MsryqA1zhve
xbZpZ5RQsWkOToFoxAh9R8m3rs+S9wLpEpfGphOCQFBjjU8dCAr4NBQTxQ6LXhXT
Ey08HJDBqH1PD0+/S06fet3e16S1Ny2/iNUvRZooMvFGzb8idaNeyo8ZOs1V6NZY
HF6jK+rKFaV8iy/ygwp84mMjq+ca3aCi85JVUuwHUnMweDgfKFCWKREqXqlUXszG
KC3eO1ab
=Tqjt
-----END PGP SIGNATURE-----
