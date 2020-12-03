-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: openldap
Binary: slapd, slapd-contrib, slapd-smbk5pwd, ldap-utils, libldap-2.4-2, libldap-common, libldap2-dev, slapi-dev
Architecture: any all
Version: 2.4.53+dfsg-1~bpo10+1
Maintainer: Debian OpenLDAP Maintainers <pkg-openldap-devel@lists.alioth.debian.org>
Uploaders: Steve Langasek <vorlon@debian.org>, Torsten Landschoff <torsten@debian.org>, Ryan Tandy <ryan@nardis.ca>
Homepage: http://www.openldap.org/
Standards-Version: 4.5.0
Vcs-Browser: https://salsa.debian.org/openldap-team/openldap
Vcs-Git: https://salsa.debian.org/openldap-team/openldap.git
Testsuite: autopkgtest
Build-Depends: debhelper (>= 10), dpkg-dev (>= 1.17.14), groff-base, heimdal-multidev (>= 7.4.0.dfsg.1-1~) <!pkg.openldap.noslapd>, libargon2-dev <!pkg.openldap.noslapd>, libdb5.3-dev <!pkg.openldap.noslapd>, libgnutls28-dev, libltdl-dev <!pkg.openldap.noslapd>, libperl-dev (>= 5.8.0) <!pkg.openldap.noslapd>, libsasl2-dev, libwrap0-dev <!pkg.openldap.noslapd>, nettle-dev <!pkg.openldap.noslapd>, perl:any, po-debconf, unixodbc-dev <!pkg.openldap.noslapd>
Build-Conflicts: autoconf2.13, bind-dev, libbind-dev
Package-List:
 ldap-utils deb net optional arch=any
 libldap-2.4-2 deb libs optional arch=any
 libldap-common deb libs optional arch=all
 libldap2-dev deb libdevel optional arch=any
 slapd deb net optional arch=any profile=!pkg.openldap.noslapd
 slapd-contrib deb net optional arch=any profile=!pkg.openldap.noslapd
 slapd-smbk5pwd deb oldlibs optional arch=all profile=!pkg.openldap.noslapd
 slapi-dev deb libdevel optional arch=any profile=!pkg.openldap.noslapd
Checksums-Sha1:
 de0027560ac5164109faaf3bd511dc0cd1b3056b 5013515 openldap_2.4.53+dfsg.orig.tar.gz
 520cde8528f3f6c2f5ecd34e041ac0650b31b630 167820 openldap_2.4.53+dfsg-1~bpo10+1.debian.tar.xz
Checksums-Sha256:
 939b281098f63432a0e29de06701823cc158225fce33973b768dc883f6756139 5013515 openldap_2.4.53+dfsg.orig.tar.gz
 707c4acf7ea4635439da4cf7e21cfb24fcc1a19bc1197722147f4b75efe3d287 167820 openldap_2.4.53+dfsg-1~bpo10+1.debian.tar.xz
Files:
 9a0d3687f5a903a4e010861f8d012e3b 5013515 openldap_2.4.53+dfsg.orig.tar.gz
 89068dfd777a005a65305182df651373 167820 openldap_2.4.53+dfsg-1~bpo10+1.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQJDBAEBCgAtFiEEPSfh0nqdQTd5kOFlIp/PEvXWa7YFAl9eX6MPHHJ5YW5AbmFy
ZGlzLmNhAAoJECKfzxL11mu2wiYP+wfq234ZlvtFPChDv8GjQBq4IkVOBi6/fi/+
/iwdKOFi0scd926yNAM7TxrPcbXIIkx3p6vshEadwruDB7peUVvnj/F0m4Larf6C
LPjMNlxIgqfu3jwU7BemfXWlYkrZ2w+UFMJVrW3x8wX9ft5BQmiRDbuEdqNe2Bed
mA3O71XvSc2VfhVDo+e5RqVlYxihb894pzJq9tQlro/jtuW9nPrZtXj9Xh+MHtAF
1+jDeMj5jU2eZF0ZO47BXdvk5KLSI9q7lv00n832fScF0AtLbPRuOVGJpziAVyH5
eFImriHTt71giLNOQ3RKc7VYszuvc7TUzvl1VWuPFTFure+oeEgkF+aq1Cj2kBVG
2ScekXmw28DOt0cDfzpe4aZHyBu1D6tGy5uUmKK5TlIfcbOGmHrfH6Il6DE/ocxE
DZGAz2Sr1qMEGJ++1pmlXJFpoFj4WRUqKSTAtW3N709XyOXnJ1L1fSrqv1QExfq/
iUvHHD9IWRTBBHTxMpMwtQ00ygtuD53E/kRCQNA8clQlZU8vgB3H0+lGj7LM/mUZ
jNzcL/ix+OdFK46VxDOE/hcqYZCrmmkIer+tpR8NObjka+kosdVCL2QYUltgx0pr
ennoopA13sSNFVk7R4s0UvtOfd8b8tXVEW3C9TS9hQ0wTDDRaFSVBO21tonoSt6o
9ZJebIEo
=Ix5v
-----END PGP SIGNATURE-----
