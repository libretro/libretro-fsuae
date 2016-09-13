                FS-UAE LIBRETRO TEST/DEBUG VERSION (WIP)



Based on FS-UAE 2.7.14dev (https://fs-uae.net/)

First 'Testable' release:
* The input layer is the same as libretro-uae (README-libretro-puae):
  * Gamepad compatible (to be fixed).
  * AltL+F11: GUI screen (need to be fixed).
  * F11:      Grab mouse (default retroarch key)
  * RET: virtual keyboard.
* Audio OK now.

Misc Issues:
* The joystick must be fixed.
* Static Makefile (Will target mainly Linux x86_64/i686/arm)

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*Test
 -The kickstart must be located here: ~/FS-UAE/Kickstarts (Same configurations/directories as the regular FS UAE)
 -This software handle the 'fs-uae' configuration files
 -On the local directory where 'fsuae_libretro.so' is located, /storage/df0.adf is your 'adf' file:

```bash
cat >a500.fs-uae <<EOF
[config]
amiga_model = A500
keep_aspect = 1
scale_y = 2.05
scale_x = 2.05
zoom = 640x512+border
floppy_drive_0 = /storage/df0.adf
EOF
```

#The last command:

```bash
export LD_PREFER_MAP_32BIT_EXEC=1; # x86_64 only for JIT compatibility
retroarch -L ./fsuae_libretro.so ./a500.fs-uae
```

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#Linux x86-64:

```bash
./autogen.sh
CFLAGS="-O2" ./configure --build=x86_64-pc-linux-gnu --prefix=/usr/local --enable-shared --disable-static --libdir=/usr/local/lib64 --enable-jit
make gen -j 4
make clean
make -j 4
```

#Linux ARM: (-mthumb or -marm mode)

```bash
./autogen.sh
CFLAGS="-O2" ./configure --build=arm-pc-linux-gnueabihf --prefix=/usr/local --enable-shared --disable-static --libdir=/usr/local/lib --disable-jit --enable-neon
make gen -j 4
make clean
make -j 4
```

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
* x86_64: the JIT requires a 32 bits address range for some ELF loaded symbols; The following command is required:

```bash
export LD_PREFER_MAP_32BIT_EXEC=1

cat >jit.patch <<EOF
--- /tmp/libretro.c	2016-09-11 18:46:36.000000000 +0200
+++ ./libretro/libretro.c	2016-09-11 18:37:45.000000000 +0200
@@ -255,7 +255,7 @@
    amiga_set_init_function(on_init);
 #endif
 
-   if (0) {
+   if (1) {
      amiga_init_jit_compiler();
    }
 
EOF
```
