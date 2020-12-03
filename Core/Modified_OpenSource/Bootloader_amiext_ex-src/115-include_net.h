diff -Naur uboot.pristine/include/net.h uboot.work/include/net.h
--- uboot.pristine/include/net.h	2020-04-08 13:20:24.346838349 -0400
+++ uboot.work/include/net.h	2020-04-14 12:39:17.000000000 -0400
@@ -203,7 +203,7 @@
 	uchar		et_dest[6];	/* Destination node		*/
 	uchar		et_src[6];	/* Source node			*/
 	ushort		et_protlen;	/* Protocol or length		*/
-};
+} __attribute__((packed));
 
 /* Ethernet header size */
 #define ETHER_HDR_SIZE	(sizeof(struct ethernet_hdr))
@@ -219,7 +219,7 @@
 	uchar		et_snap2;
 	uchar		et_snap3;
 	ushort		et_prot;	/* 802 protocol			*/
-};
+} __attribute__((packed));
 
 /* 802 + SNAP + ethernet header size */
 #define E802_HDR_SIZE	(sizeof(struct e802_hdr))
@@ -233,7 +233,7 @@
 	ushort		vet_vlan_type;	/* PROT_VLAN			*/
 	ushort		vet_tag;	/* TAG of VLAN			*/
 	ushort		vet_type;	/* protocol type		*/
-};
+} __attribute__((packed));
 
 /* VLAN Ethernet header size */
 #define VLAN_ETHER_HDR_SIZE	(sizeof(struct vlan_ethernet_hdr))
@@ -261,7 +261,7 @@
 	ushort		ip_sum;		/* checksum			*/
 	IPaddr_t	ip_src;		/* Source IP address		*/
 	IPaddr_t	ip_dst;		/* Destination IP address	*/
-};
+} __attribute__((packed));
 
 #define IP_OFFS		0x1fff /* ip offset *= 8 */
 #define IP_FLAGS	0xe000 /* first 3 bits */
@@ -289,7 +289,7 @@
 	ushort		udp_dst;	/* UDP destination port		*/
 	ushort		udp_len;	/* Length of UDP packet		*/
 	ushort		udp_xsum;	/* Checksum			*/
-};
+} __attribute__((packed));
 
 #define IP_UDP_HDR_SIZE		(sizeof(struct ip_udp_hdr))
 #define UDP_HDR_SIZE		(IP_UDP_HDR_SIZE - IP_HDR_SIZE)
@@ -328,7 +328,7 @@
 	uchar		ar_tha[];	/* Target hardware address	*/
 	uchar		ar_tpa[];	/* Target protocol address	*/
 #endif /* 0 */
-};
+} __attribute__((packed));
 
 #define ARP_HDR_SIZE	(8+20)		/* Size assuming ethernet	*/
 
@@ -363,7 +363,7 @@
 		} frag;
 		uchar data[0];
 	} un;
-};
+} __attribute__((packed));
 
 #define ICMP_HDR_SIZE		(sizeof(struct icmp_hdr))
 #define IP_ICMP_HDR_SIZE	(IP_HDR_SIZE + ICMP_HDR_SIZE)
