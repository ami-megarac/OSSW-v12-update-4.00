diff -Naur uboot.pristine/arch/arm/cpu/armv7/config.mk uboot.work/arch/arm/cpu/armv7/config.mk
--- uboot.pristine/arch/arm/cpu/armv7/config.mk	2020-04-08 13:20:13.222697130 -0400
+++ uboot.work/arch/arm/cpu/armv7/config.mk	2020-04-14 12:58:07.000000000 -0400
@@ -38,6 +38,7 @@
 # SEE README.arm-unaligned-accesses
 PF_NO_UNALIGNED := $(call cc-option, -mno-unaligned-access,)
 PLATFORM_NO_UNALIGNED := $(PF_NO_UNALIGNED)
+PLATFORM_CPPFLAGS += $(PF_NO_UNALIGNED)
 
 ifneq ($(CONFIG_IMX_CONFIG),)
 ifdef CONFIG_SPL
