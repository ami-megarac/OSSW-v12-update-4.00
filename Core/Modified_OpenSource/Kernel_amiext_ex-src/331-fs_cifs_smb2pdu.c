--- linux_old/fs/cifs/smb2pdu.c	2020-04-13 17:45:21.972796465 +0000
+++ linux/fs/cifs/smb2pdu.c	2020-04-13 16:33:05.207663328 +0000
@@ -539,11 +539,11 @@
 	struct smb2_sess_setup_rsp *rsp = NULL;
 	struct kvec iov[2];
 	int rc = 0;
-	int resp_buftype;
+	int resp_buftype = 0;
 	__le32 phase = NtLmNegotiate; /* NTLMSSP, if needed, is multistage */
 	struct TCP_Server_Info *server = ses->server;
 	u16 blob_length = 0;
-	char *security_blob;
+	char *security_blob = NULL;
 	char *ntlmssp_blob = NULL;
 	bool use_spnego = false; /* else use raw ntlmssp */
 
