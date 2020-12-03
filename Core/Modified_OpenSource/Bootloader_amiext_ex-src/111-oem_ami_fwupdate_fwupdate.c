--- uboot_ori/oem/ami/fwupdate/fwupdate.c	2020-04-10 17:10:37.579435142 +0800
+++ uboot_new/oem/ami/fwupdate/fwupdate.c	2020-04-10 17:33:41.298127357 +0800
@@ -52,6 +52,9 @@
 extern int flash_erase (flash_info_t *info, int s_first, int s_last);
 extern flash_info_t flash_info[];
 
+#ifdef CONFIG_NCSI_SUPPORT
+extern void NCSI_Start(void);
+#endif
 
 /* From cmdhandler.c */
 extern int 				gReset;
@@ -406,6 +409,12 @@
     unsigned long wdt_reset_status = 0 ;
     char    *value = NULL;
     int retries = 3;
+    int tftp_retry = 2;
+    int tftp_fail = 0;
+#ifdef CONFIG_NCSI_SUPPORT
+    char current_eth[4], default_ncsi[4];
+#endif
+    int tmp = 0;
     u8  recoveryprogress = 0;
 
     value = getenv("recoveryretry");
@@ -513,18 +522,54 @@
 
 TFTP:
     printf("TFTP Recovery Image Booting started...\n");
-
-    /*Start the TFTP recovery falshing*/
-    if (0 != TFTPRecoveryBoot())
+    
+#ifdef CONFIG_NCSI_SUPPORT
+    strncpy(default_ncsi, CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE, 4);
+#endif
+    // If TFTP Recovery failed, try to use other interface
+    for (tmp = 0; tmp < tftp_retry; tmp++)
     {
-        printf("Recovery Image Flashing via TFTP Failed !!\n");
-        return -1;
+    	if (tmp != 0)
+    	{
+    		if(tmp == 1)
+    	    {
+    	        eth_try_another(1);
+    	    }
+    	    else
+    	    {
+    	        eth_try_another(0);
+    	    }
+    	}
+    	strncpy(current_eth, eth_get_name() + strlen(eth_get_name()) - 4, 4);
+    	    	
+    	/*Start the TFTP recovery falshing*/
+    	if (tmp == 0 || tftp_fail == 1)
+    	{
+#ifdef CONFIG_NCSI_SUPPORT
+    		/* If current interface is same as default NCSI interface,
+    	   	   send command "ncsi" */
+    		if (strncmp(default_ncsi, current_eth, 4) == 0)
+    			NetLoop(NCSI);
+#endif
+    		if (0 != TFTPRecoveryBoot())
+    		{
+    			printf("Recovery Image Flashing via TFTP Failed !!\n");
+    			tftp_fail = 1;
+    		}
+    		else
+    			tftp_fail = 0;
+    	}
+    }
+    if (tftp_fail == 1)
+    	return -1;
+
+    if (tftp_fail != 1)
+    {
+    	/*Set the last recoveryboot as TFTP*/
+    	setenv("lastrecoveryboot","tftp");
+    	saveenv();
     }
 
-    /*Set the last recoveryboot as TFTP*/
-    setenv("lastrecoveryboot","tftp");
-    saveenv();
-
 SUCCESS:
 	/*After successfully flashing the image, start the watchdog timer to detect the kernel boot success.*/
 
