-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: krb5
Binary: krb5-user, krb5-kdc, krb5-kdc-ldap, krb5-admin-server, krb5-kpropd, krb5-multidev, libkrb5-dev, libkrb5-dbg, krb5-pkinit, krb5-otp, krb5-k5tls, krb5-doc, libkrb5-3, libgssapi-krb5-2, libgssrpc4, libkadm5srv-mit11, libkadm5clnt-mit11, libk5crypto3, libkdb5-9, libkrb5support0, libkrad0, krb5-gss-samples, krb5-locales, libkrad-dev
Architecture: any all
Version: 1.17-3
Maintainer: Sam Hartman <hartmans@debian.org>
Uploaders: Russ Allbery <rra@debian.org>, Benjamin Kaduk <kaduk@mit.edu>
Homepage: http://web.mit.edu/kerberos/
Standards-Version: 4.1.1
Vcs-Browser: https://salsa.debian.org/debian/krb5
Vcs-Git: https://salsa.debian.org/debian/krb5
Testsuite: autopkgtest
Testsuite-Triggers: ldap-utils, libsasl2-modules-gssapi-mit, slapd
Build-Depends: debhelper (>= 10), byacc | bison, comerr-dev, docbook-to-man, libkeyutils-dev [linux-any], libldap2-dev <!stage1>, libsasl2-dev <!stage1>, libncurses5-dev, libssl-dev, ss-dev, libverto-dev (>= 0.2.4), pkg-config
Build-Depends-Indep: python, python-cheetah, python-lxml, python-sphinx, doxygen, doxygen-latex, texlive-generic-extra
Package-List:
 krb5-admin-server deb net optional arch=any
 krb5-doc deb doc optional arch=all
 krb5-gss-samples deb net optional arch=any
 krb5-k5tls deb net optional arch=any
 krb5-kdc deb net optional arch=any
 krb5-kdc-ldap deb net optional arch=any profile=!stage1
 krb5-kpropd deb net optional arch=any
 krb5-locales deb localization optional arch=all
 krb5-multidev deb libdevel optional arch=any
 krb5-otp deb net optional arch=any
 krb5-pkinit deb net optional arch=any
 krb5-user deb net optional arch=any
 libgssapi-krb5-2 deb libs optional arch=any
 libgssrpc4 deb libs optional arch=any
 libk5crypto3 deb libs optional arch=any
 libkadm5clnt-mit11 deb libs optional arch=any
 libkadm5srv-mit11 deb libs optional arch=any
 libkdb5-9 deb libs optional arch=any
 libkrad-dev deb libdevel optional arch=any
 libkrad0 deb libs optional arch=any
 libkrb5-3 deb libs optional arch=any
 libkrb5-dbg deb debug optional arch=any
 libkrb5-dev deb libdevel optional arch=any
 libkrb5support0 deb libs optional arch=any
Checksums-Sha1:
 0c404b081db9c996c581f636ce450ee28778f338 8761763 krb5_1.17.orig.tar.gz
 88d2d502fbb060405304c9372e6ddc605b9fbeb1 99396 krb5_1.17-3.debian.tar.xz
Checksums-Sha256:
 5a6e2284a53de5702d3dc2be3b9339c963f9b5397d3fbbc53beb249380a781f5 8761763 krb5_1.17.orig.tar.gz
 35da9d221e3a29c57c38c9d326d625a5b9199f3d7d64983483bd82f871083c9f 99396 krb5_1.17-3.debian.tar.xz
Files:
 3b729d89eb441150e146780c4138481b 8761763 krb5_1.17.orig.tar.gz
 d1bfc39f90c11dd80ae06830141ac1b9 99396 krb5_1.17-3.debian.tar.xz
Dgit: 458207872e06bc94cdbe1f6965af30f7dd4f0b5e debian archive/debian/1.17-3 https://git.dgit.debian.org/krb5

-----BEGIN PGP SIGNATURE-----

iQEzBAEBCAAdFiEE9Li3nMNy++OFgPTCQe7SUh/WssoFAl0I1WoACgkQQe7SUh/W
ssrYogf+MpKbmgmirpaTjsNEsr1y9bs9TaimbcPZOMrIUbDMktKpCKH4lQnA7iFQ
uRNx5unXYNsh0Htt4hdLR7mOipS0MaM9O9nB6SbQGzlx+0QDpYVyrwqZ0p8F6IGF
hZKcSRMuIQJ07N23ogvTUvxtT77PtTTDjmRpoW6bC54AqolA/G8GRqlHhf5TrG+E
S62Uqc1cKfjOCZdPqp/INGr+3n76ZnQsmpJMx6qr0W+eCQJuf++1eJwNclWPLdJ9
7yTPQxAAp4ORhsc/iVMy40ZZp/tiKh+Ek/F3FC2yXeDt4w0SVMGwUSj7XV3aZ4Fd
GHUQj9gNlU5AXnnrhROC4PwrAxJAww==
=ZIUo
-----END PGP SIGNATURE-----
