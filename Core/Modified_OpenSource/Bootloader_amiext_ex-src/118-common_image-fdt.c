--- uboot/common/image-fdt.c	2020-04-29 15:56:30.064476198 +0800
+++ uboot.new/common/image-fdt.c	2020-04-29 16:02:18.011311080 +0800
@@ -32,6 +32,9 @@
 #include <libfdt.h>
 #include <asm/io.h>
 
+#ifdef CONFIG_HAS_SPI
+#include <spiflash.h>
+#endif
 #ifndef CONFIG_SYS_FDT_PAD
 #define CONFIG_SYS_FDT_PAD 0x3000
 #endif
@@ -146,6 +149,14 @@
 
 	/* If fdt_high is set use it to select the relocation address */
 	fdt_high = getenv("fdt_high");
+#ifdef CONFIG_OF_DTBFMH
+	if (!fdt_high)
+	{
+		/* Use the default if not available in environ */
+    	setenv("fdt_high",CONFIG_FDT_RELOC_ADDR_STR);	
+		fdt_high = getenv("fdt_high");
+	}
+#endif
 	if (fdt_high) {
 		void *desired_addr = (void *)simple_strtoul(fdt_high, NULL, 16);
 
@@ -211,6 +222,57 @@
 	return 1;
 }
 
+#ifdef CONFIG_OF_DTBFMH
+ulong genimg_get_dtbfmh(ulong fdt_addr)
+{
+	ulong ram_addr = fdt_addr;
+#ifdef CONFIG_HAS_SPI
+	ulong h_size, d_size;
+
+	if (addr_spi(fdt_addr)) {
+
+		/* get RAM address */
+		ram_addr = CONFIG_FDT_LOAD_ADDR;
+
+		h_size = sizeof(struct fdt_header);
+
+		/* read in fdt header */
+		debug("   Reading fdt header from spi address "
+			"%08lx to RAM address %08lx\n", fdt_addr, ram_addr);
+
+		flash_read(fdt_addr, h_size, (char *)ram_addr);
+
+		/* get fdt data size */
+		switch (genimg_get_format((void *)ram_addr)) {
+#if defined(CONFIG_FIT) || defined(CONFIG_OF_LIBFDT)
+		case IMAGE_FORMAT_FIT:
+			d_size = fdt_totalsize(ram_addr) - h_size;
+			debug("   FIT/FDT format image found at 0x%08lx, "
+					"size 0x%08lx\n",
+					ram_addr, d_size);
+			break;
+#endif
+		case IMAGE_FORMAT_INVALID:
+		//fall through
+		default:
+			printf("   No valid fdt found at 0x%08lx\n",
+				fdt_addr);
+			return ram_addr;
+		}
+
+		/* read in fdt data */
+		debug("   Reading fdt remaining data from spi address "
+			"%08lx to RAM address %08lx\n", fdt_addr + h_size,
+			ram_addr + h_size);
+
+		flash_read(fdt_addr + h_size, d_size,
+				(char *)(ram_addr + h_size));
+	}
+#endif
+
+	return ram_addr;
+}
+#endif
 /**
  * boot_get_fdt - main fdt handling routine
  * @argc: command argument count
@@ -305,7 +367,11 @@
 		      fdt_addr);
 
 		/* copy from dataflash if needed */
+#ifdef CONFIG_OF_DTBFMH
+		fdt_addr = genimg_get_dtbfmh(fdt_addr);
+#else
 		fdt_addr = genimg_get_image(fdt_addr);
+#endif 
 
 		/*
 		 * Check if there is an FDT image at the
