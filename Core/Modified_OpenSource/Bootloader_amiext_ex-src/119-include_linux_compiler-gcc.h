diff -Naur uboot.pristine/include/linux/compiler-gcc.h uboot.work/include/linux/compiler-gcc.h
--- uboot.pristine/include/linux/compiler-gcc.h	2020-04-08 13:20:23.994833886 -0400
+++ uboot.work/include/linux/compiler-gcc.h	2020-04-14 12:24:35.207361808 -0400
@@ -44,9 +44,16 @@
  */
 #if !defined(CONFIG_ARCH_SUPPORTS_OPTIMIZED_INLINING) || \
     !defined(CONFIG_OPTIMIZE_INLINING) || (__GNUC__ < 4)
+
+#if (__GNUC__ < 8)
 # define inline		inline		__attribute__((always_inline))
 # define __inline__	__inline__	__attribute__((always_inline))
 # define __inline	__inline	__attribute__((always_inline))
+#else
+# define inline		inline		__attribute__((always_inline)) __attribute__((__gnu_inline__))
+# define __inline__	__inline__	__attribute__((always_inline)) __attribute__((__gnu_inline__))
+# define __inline	__inline	__attribute__((always_inline)) __attribute__((__gnu_inline__))
+#endif
 #endif
 
 #define __deprecated			__attribute__((deprecated))
