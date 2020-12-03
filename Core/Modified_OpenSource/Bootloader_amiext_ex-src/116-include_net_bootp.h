diff -Naur uboot.pristine/net/bootp.h uboot.work/net/bootp.h
--- uboot.pristine/net/bootp.h	2020-04-08 13:20:19.766780238 -0400
+++ uboot.work/net/bootp.h	2020-04-14 12:44:46.000000000 -0400
@@ -49,7 +49,7 @@
 	char		bp_sname[64];	/* Server host name		*/
 	char		bp_file[128];	/* Boot file name		*/
 	char		bp_vend[OPT_FIELD_SIZE]; /* Vendor information	*/
-};
+} __attribute__((packed));
 
 #define BOOTP_HDR_SIZE	sizeof(struct Bootp_t)
 
