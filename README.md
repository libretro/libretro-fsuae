                FS-UAE LIBRETRO TEST/DEBUG VERSION (WIP)



Based on FS-UAE 2.8.1u5 (https://fs-uae.net/)

'Beta' release:
* The input layer is the same as libretro-uae (README-libretro-puae):
  * Gamepad compatible (to be fixed).
  * AltL+F11: GUI screen (need to be fixed).
  * F11:      Grab mouse (default retroarch key)
  * RET: virtual keyboard.
* Audio OK now.
* Mouse + Jostick setup as default emulation inputs.

Misc Issues:
* Static Makefile (Will target mainly Linux x86_64/i686/arm)

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#Test
* The kickstart must be located here: (Retro_Save_Directory)/fsuae/Kickstarts
* Using USE_FSUAEDIRS=1 at the make level, the software will use the same directories/configurations as the regular FS-UAE (kickstart: ~/FS-UAE/Kickstarts)
* This software processes the 'fs-uae' configuration files
* Your ADF file: /storage/df0.adf

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

ARM: The following parameter is likely required to get real-time operation: accuracy = 0, or accuracy = -1.

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#A4000 / A3000 Emulation / Regular Installation
The file: 'fs-uae.dat' is required, and must be located, on one of the following directory (the base is the 'retroarch' executable directory): base/fs-uae.dat, base/../share/fs-uae/fs-uae.dat or base/../../Data/fs-uae.dat

e.g.:
```bash
install -o root -g bin -m 0644 fsuae_libretro.so "${prefix}/libexec/libretro/x86_64-pc-linux-gnu/fsuae_libretro.so"
install -o root -g bin -m 0644 "fs-uae.dat" "${prefix}/share/fs-uae/fs-uae.dat"
```

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#JIT

Operational on the 'x86_64' and 'i686' architecture.

```bash
cat >a4000.fs-uae <<EOF
[config]
amiga_model = A4000
jit_compiler = 1
uae_compfpu = 1
accuracy = 0
keep_aspect = 1
scale_y = 2.05
scale_x = 2.05
zoom = 640x512+border
floppy_drive_0 = /storage/df0.adf
EOF
```


* x86_64: the JIT requires a 32 bits address range for some ELF loaded symbols; The following command is required:

```bash
export LD_PREFER_MAP_32BIT_EXEC=1
```

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#Graphic Resolution / Crop

Add a new option to 'crop' the graphic output: zoom = GFXWIDTHxGFXHEIGHT/GFX_OFFSETX/GFX_OFFSETY/GFX_CROPWIDTH/GFX_CROPHEIGHT The last two values are optional. This option is not compatible with the classical FS-UAE configuration file. Only the (GFX_CROPWIDTH,GFX_CROPHEIGHT) image size will be displayed.  e.g.: zoom = 688x464/8/56/680/404   (GFX_OFFSETX and GFX_CROPWIDTH must be a multiple of 4)

