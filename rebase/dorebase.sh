#! /bin/bash

function cfix_addtypedef () {
  s_typedef="$1"; shift;
  sed -r -i "/^(struct|enum)[ \t]+${s_typedef}[ \t]*[{]?$/,/^};$/ {s/^((struct|enum)[ \t]+${s_typedef}[ \t]*[{]?)$/typedef \1/; s/^};$/} ${s_typedef};/;};" "$@";
}

if [ \! -f src/cfgfile.c ]; then echo "Wrong dir... failed..."; exit -1;fi; #-o \! -f src/ncr9x_scsi.c

#sed -r -i "s/[ \t]*_P[(](.*)[)];/\1;/" src/include/misc.h

sed -r -i "s/(do_scsi)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\15\2/" src/blkdev.c
sed -r -i "s/([,(][ \t]*)(cd_standard_unit [a-zA-Z0-9]+)/\1enum \2/" src/blkdev.c

sed -r -i "s/(zfile_fopen)([ \t]*[(][^,]*[,][^,]*[,][^,]*[)])/\13\2/" src/driveclick.c src/hardfile.c src/main.c src/filesys.c src/od-fs/hardfile_host.c src/cfgfile.c \
  src/cdtvcr.c src/cpuboard.c src/cdtv.c src/akiko.c src/arcadia.c src/zfile.c src/audio.c src/include/zfile.h src/disk.c src/savestate.c src/zfile_archive.c src/rommgr.c src/od-fs/roms.c
sed -r -i "s/(zfile_fopen)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/cfgfile.c src/cpuboard.c src/rommgr.c src/zfile.c src/disk.c src/include/zfile.h
sed -r -i "s/(zfile_gunzip)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/zfile.c
sed -r -i "s/(zfile_fopen_empty)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/cfgfile.c src/include/zfile.h src/rommgr.c src/savestate.c src/zfile.c
sed -r -i "s/(esp_dma_enable)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/qemuvga/scsi/esp.h src/ncr9x_scsi.c
sed -r -i "s/(iszip)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/zfile.c
sed -r -i "s/(readhex|readint)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/debug.c

sed -r -i "s/(hdf_open)([ \t]*[(][^,]*[)])/\11\2/" src/hardfile.c src/filesys.c
sed -r -i "s/(zfile_fopen_archive)([ \t]*[(][^,]*[)])/\11\2/" src/filesys.c src/include/zfile.h src/zfile.c
sed -r -i "s/(flush_icache|flush_icache_hard)([ \t]*[(][^,]*[)])/\11\2/" src/jit/compemu.h src/jit/compemu_support.c
sed -r -i "s/(m68k_dumpstate)([ \t]*[(][^,]*[)])/\11\2/" src/include/newcpu.h src/debug.c src/memory.c src/newcpu.c
sed -r -i "s/(zfile_opendir_archive)([ \t]*[(][^,]*[)])/\11\2/" src/filesys.c src/zfile.c
sed -r -i "s/(my_opendir)([ \t]*[(][^,]*[)])/\11\2/" src/filesys.c src/fsdb.c src/od-fs/filesys_host.c src/zfile.c src/include/fsdb.h
sed -r -i "s/(fixaddr)([ \t]*[(][^,]*[)])/\11\2/" src/gfxboard.c
sed -r -i "s/(getromdatabycrc)([ \t]*[(][^,]*[)])/\11\2/" src/memory.c src/rommgr.c src/debug.c src/include/rommgr.h src/od-fs/roms.c
sed -r -i "s/(loaddat)([ \t]*[(][^,]*[)])/\11\2/" src/audio.c
sed -r -i "s/(getsem)([ \t]*[(][^,]*[)])/\11\2/" src/blkdev.c
sed -r -i "s/(es1370_reset)([ \t]*[(][^,]*[)])/\11\2/" src/qemuvga/es1370.c
sed -r -i "s/(decide_sprites)([ \t]*[(][^,]*[)])/\11\2/" src/custom.c
sed -r -i "s/(inprec_realtime)([ \t]*[(][^,]*[)])/\11\2/" src/od-fs/inputrecord.c


sed -r -i "s/(float32_round_to_int)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" ./softfloat/softfloat.h
sed -r -i "s/(float64_round_to_int)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" ./softfloat/softfloat.h
sed -r -i "s/(zfile_readdir_archive)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/filesys.c src/zfile.c
sed -r -i "s/(propagateFloat32NaN|propagateFloat64NaN|propagateFloatx80NaN)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" ./softfloat/softfloat-specialize.h softfloat/softfloatx80.c softfloat/softfloat.c
sed -r -i "s/(packFloat128)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" softfloat/softfloat-specialize.h ./softfloat/softfloat-specialize.{c,h} ./softfloat/softfloat.c


sed -r -i "s/(allocuci)([ \t]*[(][^,]*[,][^,]*[,][^,]*[)])/\13\2/" src/filesys.c
sed -r -i "s/(zfile_readdir_archive)([ \t]*[(][^,]*[,][^,]*[,][^,]*[)])/\13\2/" src/filesys.c
sed -r -i "s/(AUDxDAT)([ \t]*[(][^,]*[,][^,]*[,][^,]*[)])/\13\2/" src/audio.c src/custom.c src/include/audio.h

sed -r -i "s/(inputdevice_uaelib)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/cfgfile.c src/include/inputdevice.h src/inputdevice.c
sed -r -i "s/^([#]include[ \t]+\"luascript.h\")/\/\/ \1/" src/cfgfile.c
#sed -r -i "s/(cfgfile_unescape)([ \t]*[(][^,]*[)])/\11\2/" src/cfgfile.c
sed -r -i "s/(cfgfile_unescape)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/cfgfile.c
sed -r -i "s/(cfgfile_write_str)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\14\2/" src/cfgfile.c
sed -r -i "s/(cfgfile_write_str)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\14\2/" src/cfgfile.c
sed -r -i "s/(cfgfile_write_str)4([ \t]*[(].*target_get_display_name[ \t]*[(])/\1\2/" src/cfgfile.c; #FIX subfunction
sed -r -i "s/(cfgfile_dwrite_str|cfgfile_dwrite_bool|cfgfile_floatval|cfgfile_yesno)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\14\2/" src/cfgfile.c
sed -r -i "s/(cfgfile_yesno)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\15\2/" src/cfgfile.c
sed -r -i "s/(cfgfile_intval|cfg_dowrite|cfgfile_string|cfgfile_path)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\16\2/" src/cfgfile.c
sed -r -i "s/(cfgfile_strval)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\17\2/" src/cfgfile.c
sed -r -i "s/^(static void cfgfile_dwrite_bool)( [(].*int b[)])/\1_int\2/" src/cfgfile.c; # _int
sed -r -i "s/^((extern )?int cfgfile_intval)/\/\/ \1/" src/include/options.h; # src/include/cfgfile.h
sed -r -i "s/^(static int cfgfile_yesno5)( .*int \*\location, bool numbercheck[)])$/\1_int\2/" src/cfgfile.c
sed -r -i "s/^(static int cfgfile_yesno5)( .*bool \*\location, bool numbercheck[)])$/\1_bool\2/" src/cfgfile.c
sed -r -i "s/^(static int cfgfile_yesno4)( .*int \*\location[)])$/\1_int\2/" src/cfgfile.c
sed -r -i "s/^((static )?int cfgfile_yesno4)( .*bool \*\location[)])$/\1_bool\3/" src/cfgfile.c
sed -r -i "s/^((static )?int cfgfile_intval)( .*, unsigned int \*location, int scale[)])$/\1_unsigned\3/" src/cfgfile.c
sed -r -i "s/^((static )?int cfgfile_intval)( .*, int \*location, int scale[)])$/\1_signed\3/" src/cfgfile.c
sed -r -i "s/^((static )?int cfgfile_intval6)( .*, unsigned int \*location, int scale[)])$/\1_unsigned\3/" src/cfgfile.c
sed -r -i "s/^((static )?int cfgfile_intval6)( .*, int \*location, int scale[)])$/\1_signed\3/" src/cfgfile.c

sed -r -i "s/(crop|cache)([ \t]*[(][^,]*[)])/\11\2/" src/ppc/ppcd.c
sed -r -i "s/(rlw|put|ldst)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/ppc/ppcd.c
sed -r -i "s/(integer|put|ldst|crop|fpu)([ \t]*[(][^,]*[,][^,]*[,][^,]*[)])/\13\2/" src/ppc/ppcd.c
sed -r -i "s/(ldst)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\14\2/" src/ppc/ppcd.c
sed -r -i "s/(integer|ldst)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\15\2/" src/ppc/ppcd.c
sed -r -i "s/(integer)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\17\2/" src/ppc/ppcd.c

sed -r -i "s/(sys_command_cd_play)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\14\2/" src/blkdev.c src/scsiemul.c src/include/blkdev.h

sed -r -i "s/(set_status)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/a2091.c
sed -r -i "s/(dmac_a2091_xlate|dmac_a2091_wgeti|dmac_a2091_bget|dmac_gvp_lgeti|dmac_gvp_bget|dmac_a2091_bput|dmac_a2091_check)([ \t]*[(][^,]*[,][^,]*[,][^,]*[)])/\13\2/" src/a2091.c
sed -r -i "s/(dmac_gvp_lput|dmac_gvp_wput|dmac_gvp_bput|dmac_gvp_xlate|dmac_gvp_check|dmac_a2091_lput|dmac_a2091_wput)([ \t]*[(][^,]*[,][^,]*[,][^,]*[)])/\13\2/" src/a2091.c
sed -r -i "s/(dmac_a2091_xlate|dmac_gvp_lgeti|dmac_gvp_wgeti|dmac_gvp_lget|dmac_gvp_wget|dmac_gvp_bget|dmac_a2091_lget|dmac_a2091_lgeti|dmac_a2091_wgeti|dmac_a2091_bget|dmac_a2091_wget)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" \
  src/a2091.c
sed -r -i "s/(set_status)2( [(]wd, [(]wd->wdregs)/\1\2/" src/a2091.c; # fix 

sed -r -i "s/(disk_insert)([ \t]*[(][^,]*[,][^,]*[)])/\12\2/" src/include/disk.h src/disk.c src/od-fs/libamiga.c
sed -r -i "s/(sys_command_cd_rawread)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\15\2/" src/include/blkdev.h src/akiko.c src/cdtv.c src/blkdev.c src/scsiemul.c
sed -r -i "s/(isaudiotrack)([ \t]*[(][^,]*[)])/\11\2/" src/akiko.c
sed -r -i "s/(addcycles_ce020)([ \t]*[(][^,]*[)])/\1_1\2/" src/gencpu.c
sed -r -i "s/(addcycles_ce020|addcycles_ea_ce020)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\1_4\2/" src/gencpu.c
sed -r -i "s/(addcycles_ce020)([ \t]*[(][^,]*[,][^,]*[,][^,]*[,][^,]*[,][^,]*[)])/\1_5\2/" src/gencpu.c

sed -r -i "s/(map_banks)([ \t]*[(]([ \t]*|[ \t]*void[ \t]*)[)])/\10\2/" src/ppc/ppc.c
sed -r -i "s/(inputdevice_unacquire)([ \t]*[(]([ \t]*|[ \t]*void[ \t]*)[)])/\10\2/" src/debug.c src/include/inputdevice.h src/inputdevice.c
sed -r -i "s/(term)([ \t]*[(]([ \t]*|[ \t]*void[ \t]*)[)])/\10\2/" src/gencpu.c
sed -r -i "s/(irc2ir)([ \t]*[(]([ \t]*|[ \t]*void[ \t]*)[)])/\10\2/" src/gencpu.c


#cpu
sed -r -i "s/(get_word_prefetch)([ \t]*[(][ \t]*[0-9]+[ \t]*[)])/\1_int\2/" src/include/cpu_prefetch.h src/newcpu.c; #src/cpuemu_11.c
sed -r -i "s/(get_word_prefetch)([ \t]*[(][ \t]*[a-zA-Z0-9\t +*-]+[ \t]*[)])/\1_uaecptr\2/g" src/include/cpu_prefetch.h src/newcpu.c; #src/cpuemu_11.c

#float
sed -r -i "s/float_status_t [&]status/float_status_t *status/" ./softfloat/softfloat.h ./softfloat/softfloat-specialize.h ./softfloat/softfloatx80.h softfloat/softfloat-round-pack.h \
           softfloat/softfloatx80.c softfloat/softfloat.c softfloat/softfloat-specialize.c
sed -r -i "s/(status)[.]/\1->/" softfloat/softfloatx80.c
sed -r -i "/^floatx80 floatx80_extract/,/^floatx80 floatx80_scale/ s/[(]a[)]/(*a)/" softfloat/softfloatx80.c
sed -r -i "/^floatx80 floatx80_extract/,/^floatx80 floatx80_scale/ { s/^([ \t]*)(a)([ \t]*=)/\1*\2\3/;s/return a;/return *a;/; s/^(([ \t]*)a)[.]/\1->/; s/([(])(a,)/\1*\2/; }" softfloat/softfloatx80.c

sed -r -i 's/status[.](float_rounding_mode|float_exception_flags|float_exception_masks|float_suppress_exception|float_nan_handling_mode|flush_underflow_to_zero|denormals_are_zeros|float_rounding_precision)/status->\1/g' \
  ./softfloat/softfloat.h ./softfloat/softfloat-specialize.h ./softfloat/softfloatx80.h softfloat/softfloat.c
sed -r -i "s/(floatx80_extract[(]floatx80 )\&(a)/\1*\2/" ./softfloat/softfloatx80.h ./softfloat/softfloatx80.c
sed -r -i "s/, floatx80 \&r, Bit64u \&q,/, floatx80 *r, Bit64u *q,/" src/../softfloat/softfloatx80.h
sed -r -i "s/floatx80\& floatx80_(abs|chs)[(]floatx80 \&reg[)]/floatx80 floatx80_\1(floatx80 *reg)/" src/../softfloat/softfloatx80.h
sed -r -i "s/reg[.]exp ([\^&])= /reg->exp \1= /" src/../softfloat/softfloatx80.h
sed -r -i "s/(int f(sin|cos|tan)[(]floatx80 )\&(a,)/\1*\3/" src/../softfloat/softfloatx80.h
sed -r -i "s/return reg;/return *reg;/" src/../softfloat/softfloatx80.h
sed -r -i "s/(,[\t ]*)(fxstatus[\t ]*[)])/\1\&\2/" src/fpp.c
sed -r -i "s/(floatx80_(abs|chs)[(][\t ]*)([a-zA-Z0-9]+)([\t ]*[)])/\1\&\3\4/" src/fpp.c
sed -r -i "s/(f(cos|sin|tan)[(])(f)/\1\&\3/" src/fpp.c
sed -r -i "s/(floatx80_ieee754_remainder[(]f, fx, )(regs.fp\[reg\].fpx, )(q,)/\1\&\2\&\3/" src/fpp.c
sed -r -i "s/(floatx80 floatx80_mul)([(]floatx80 a, float128)/\1_f128\2/" softfloat/softfloat.h ./softfloat/softfloat.c
sed -r -i 's/const unsigned float_all_exceptions_mask = 0x([0-9a-fA-F]+);/#define float_all_exceptions_mask (0x\1U)/' ./softfloat/softfloat.h
sed -r -i "s/(Bit64u)([(]aSig[)])/(\1)\2/" softfloat/softfloat.c
sed -r -i "s/^const[ \t]+([a-z0-9_]+)[ \t]+([a-z0-9A-Z_]+)[ \t]*=(.*);$/#define \2 ((\1)\3)/" ./softfloat/softfloat-specialize.h;

#static:
if false; then
echo $(sed -r -n "s/^(BX_CPP_INLINE[ \t]+[a-z0-9_]+[ \t]+([a-z0-9_]+)[ \t]*[(].*[)])$/\2/p" ./softfloat/softfloat.h ./softfloat/softfloat-specialize.h)|sed -r "s/ /|/g"
echo "|float64_round_to_int2|float32_round_to_int2|countLeadingZeros32|countLeadingZeros16|countLeadingZeros64"
echo "|float128ToCommonNaN|float16ToCommonNaN|float32ToCommonNaN|float64ToCommonNaN|floatx80ToCommonNaN|propagateFloat32NaN2|propagateFloat64NaN2|propagateFloatx80NaN2"
echo "|packFloatx80"
fi;
#--
sed -r -i "s/^(BX_CPP_INLINE[ \t]+)?([a-zA-Z0-9_]+[ \t]+(float16_is_nan|float16_is_signaling_nan|float16_is_denormal|float16_denormal_to_zero|float32_is_nan|float32_is_signaling_nan|float32_is_denormal|float32_denormal_to_zero|\
float64_is_nan|float64_is_signaling_nan|float64_is_denormal|float64_denormal_to_zero|floatx80_is_nan|floatx80_is_signaling_nan|floatx80_is_unsupported|float128_is_nan|float128_is_signaling_nan|\
float64_round_to_int2|float32_round_to_int2|countLeadingZeros32|countLeadingZeros16|countLeadingZeros64|\
float128ToCommonNaN|float16ToCommonNaN|float32ToCommonNaN|float64ToCommonNaN|floatx80ToCommonNaN|propagateFloat32NaN2|propagateFloat64NaN2|propagateFloatx80NaN2|\
packFloatx80)[ \t]*[(].*[)])/static BX_CPP_INLINE \2/" \
./softfloat/softfloat.h ./softfloat/softfloat-specialize.h ./softfloat/softfloat-macros.h

sed -r -i "s/(row_pp = )new (png_bytep\[height\])(;)/\1malloc(sizeof(\2))\3/" src/specialmonitors.c
sed -r -i "s/delete\[\][ \t]+row_pp;/free(row_pp);/" src/specialmonitors.c
sed -r -i "s/^([ \t]*x_prefetch[ \t]*=[ \t]*get_word_prefetch)(;)/\1_int\2/" src/newcpu.c
sed -r -i "s/^([ \t]*)(plfstate nextstate =)/\1enum \2/" src/custom.c
sed -r -i "s/^([ \t]*)(my_opendir_s[ \t]+[*]dir;)/\1struct \2/" src/fsdb.c
sed -r -i "s/^([ \t]*)(strlist[ \t]*[*])/\1struct \2/" src/cfgfile.c
sed -r -i "s/([(])(const[ \t]+|)((regusage|cpu_history)([ \t]+|[ \t]*[*]))/\1\2struct \3/" src/jit/compemu_support.c
sed -r -i "s/([(])(const[ \t]+|)((instr)([ \t]+|[ \t]*[*]))/\1\2struct \3/" src/gencpu.c
sed -r -i "s/^(static[ \t]+)(instr[ \t][*])/\1struct \2/" src/gencpu.c
sed -r -i "s/(cpuemu_%d%s.c)pp/\1/" src/gencpu.c
sed -r -i "s/(\"cpustbl.c)pp(\")/\1\2/" src/gencpu.c
sed -r -i "s/(\"compemu.c)pp(\")/\1\2/" src/jit/gencomp.c
sed -r -i "s/(\"compstbl.c)pp(\")/\1\2/" src/jit/gencomp.c
sed -r -i "s/(\"uae)\/(memory.h.\")/\1_\2/" src/gencpu.c src/genblitter.c src/jit/gencomp.c
sed -r -i "s/(prefetch_word = \"get_word_prefetch)(\";)/\1_int\2/" src/gencpu.c
sed -r -i "s/(srcw = \"get_word_prefetch)(\";)/\1_uaecptr\2/" src/gencpu.c
sed -r -i "s/(\")(extern void comp_)/\1\/\/ \2/" src/jit/gencomp.c


#-- include
sed -r -i "s/^(#include[ \t]+\"sinctable.c)pp(\")/\1\2/" src/audio.c
sed -r -i "s,^(#include[ \t]+\"[.][.]/p96_blit.c)pp(\"),\1\2," src/od-win32/picasso96_win.c
sed -r -i "s,^(#include[ \t]+\")(caps/caps_win32.h\"),\1od-win32/\2," src/disk.c src/zfile.c
sed -r -i "s,^(#include[ \t]+\")(parser.h\"),\1od-win32/\2," src/od-win32/serial_win32.c
sed -r -i "s,^(#include[ \t]+)<(uae)/(fs.h)>$,\1\"\2_\3\"," src/cfgfile.c
sed -r -i "s/^(#include[ \t]+\"linetoscr.c)pp(\")/\1\2/" src/drawing.c
sed -r -i "s,^(#include[ \t]+\")(blit[.]h\"),\1../gen/\2," src/blitter.c
sed -r -i "s,^(#include[ \t]+\")(linetoscr[.]c\"),\1../gen/\2," src/expansion.c
sed -r -i "s,^(#include[ \t]+\")(comptbl[.]h\"),\1../gen/\2," src/jit/compemu_support.c
sed -r -i "s/^(#include[ \t]+\"bsdsocket_posix.c)pp(\")/\1\2/" src/od-fs/bsdsocket_host.c
sed -r -i "s/^(#include[ \t]+\"(codegen_arm|codegen_x86|compemu_midfunc_arm|compemu_midfunc_arm2|compemu_midfunc_x86)[.]c)pp(\")/\1\3/" src/jit/compemu_support.c
sed -r -i "s/^(#include \")(md-fpp[.]h\")$/\1..\/od-win32\/\2/" src/fpp.c src/newcpu.c src/jit/compemu_fpp.c


#-- fix _T("...")
sed -r -i "s/(zfile_fopen_empty)2([ \t]*[(][^,]*[,][ \t]*_T[(][ \t]*\"[a-z.]*\"[)][ \t]*[,][^,]*[)])/\1\2/" src/cfgfile.c src/rommgr.c; # - ,_T("configstore"), -
sed -r -i "s/(zfile_fopen)2([ \t]*[(][^,]*[,][ \t]*_T[(][ \t]*\"[a-z.]*\"[)][ \t]*[,][^,]*[)])/\13\2/" src/cfgfile.c src/rommgr.c;
sed -r -i "s/(zfile_fopen)([ \t]*[(][^,]*[,][ \t]*_T[(][ \t]*\"[a-z.]*\"[)][ \t]*[,][^,]*[)])/\13\2/" src/cfgfile.c src/rommgr.c;
sed -r -i "s/(zfile_fopen)([ \t]*[(][^,]*[,][ \t]*_T[(][ \t]*\"[a-z.]*\"[)][ \t]*[)])/\12\2/" src/cia.c;
sed -r -i "s/(inprec_realtime)1([ \t]*[(][ \t]*void[ \t]*[)])/\1\2/" src/od-fs/inputrecord.c;

#-- fix
sed -r -i "s/^(static void es1370_reset)1([(]struct pci_board_state [*]pcibs[)])/\1\2/" src/qemuvga/es1370.c; #fix
sed -r -i "s/^(addrbank_thread [{])$/struct \1/" src/memory.c

#--typedef
cfix_addtypedef "uaedev_config_data" src/include/options.h
cfix_addtypedef "romconfig" src/include/options.h
cfix_addtypedef "boardromconfig" src/include/options.h
cfix_addtypedef "library_data" src/uaenative.c 
cfix_addtypedef "uni_handle" src/uaenative.c 
cfix_addtypedef "uae_library_trap_def" src/uaenative.c 
cfix_addtypedef "bitbang_i2c_interface" src/flashrom.c
cfix_addtypedef "wd_state" src/include/a2091.h
cfix_addtypedef "rand_context" src/random.c
cfix_addtypedef "DeviceState" src/qemuvga/qemuuaeglue.h
cfix_addtypedef "vga_retrace_method" src/qemuvga/qemuuaeglue.h
cfix_addtypedef "BusState" src/qemuvga/qemuuaeglue.h
cfix_addtypedef "SWVoiceOut" src/qemuvga/qemuaudio.h
cfix_addtypedef "SWVoiceIn" src/qemuvga/qemuaudio.h
cfix_addtypedef "MACAddr" src/qemuvga/ne2000.c
cfix_addtypedef "NetClientState" src/qemuvga/ne2000.c
cfix_addtypedef "PPCLockMethod" src/ppc/ppc.c
cfix_addtypedef "PPCLockStatus" src/ppc/ppc.c
cfix_addtypedef "float_status_t" softfloat/softfloat.h
cfix_addtypedef "floatx80" softfloat/softfloat.h
cfix_addtypedef "float128" softfloat/softfloat.h
cfix_addtypedef "floatx80" softfloat/softfloat.h
cfix_addtypedef "MultiDisplay" src/od-win32/dxwrap.h
cfix_addtypedef "addrbank_thread" src/memory.c

#-- FSUAE forced
sed -r -i "s/^(#if)(def FSUAE)/\1 1 \/\/\2/" src/od-win32/mman.c src/od-win32/picasso96_win.c src/od-fs/parser.c \
  src/inputevents.def src/include/custom.h src/include/uae_memory.h src/custom.c src/include/autoconf.h src/sndboard.c src/disk.c \
  src/include/bsdsocket.h src/include/zfile.h src/jit/compemu_support.c src/filesys.c src/include/fsdb.h src/fsdb.c src/main.c \
  src/cpuboard.c src/jit/compemu_prefs.c src/memory.c src/writelog.c src/specialmonitors.c src/zfile.c src/qemuvga/qemuuaeglue.h src/od-win32/serial_win32.c \
  src/gfxutil.c src/include/commpipe.h src/od-win32/dxwrap.h src/include/rommgr.h src/inputdevice.c src/cfgfile.c src/rommgr.c

#-- patch
(
patch -p1 -N -s <<EOF
--- fs-uae-2.7.14dev/src/cfgfile.c	2016-09-04 00:21:44.000000000 +0200
+++ sources/src/cfgfile.c	2016-09-04 00:19:21.000000000 +0200
@@ -12,7 +12,23 @@
 
 #include <ctype.h>
 
-#ifdef FSUAE // NL
+static int cfgfile_yesno5_bool (const TCHAR *option, const TCHAR *value, const TCHAR *name, bool *location, bool numbercheck);
+static int cfgfile_yesno5_int (const TCHAR *option, const TCHAR *value, const TCHAR *name, int *location, bool numbercheck);
+#define cfgfile_yesno5(a1,a2,a3,a4,a5) _Generic((a4), int *: cfgfile_yesno5_int, default: cfgfile_yesno5_bool)(a1,a2,a3,a4,a5)
+
+static int cfgfile_yesno4_int (const TCHAR *option, const TCHAR *value, const TCHAR *name, int *location);
+#define cfgfile_yesno4(a1,a2,a3,a4) _Generic((a4), int *: cfgfile_yesno4_int, default: cfgfile_yesno4_bool)(a1,a2,a3,a4)
+
+static int cfgfile_intval_unsigned (const TCHAR *option, const TCHAR *value, const TCHAR *name, unsigned int *location, int scale);
+int cfgfile_intval_signed (const TCHAR *option, const TCHAR *value, const TCHAR *name, int *location, int scale);
+#define cfgfile_intval(a1,a2,a3,a4,a5) _Generic((a4), unsigned int *: cfgfile_intval_unsigned, default: cfgfile_intval_signed)(a1,a2,a3,a4,a5)
+
+static int cfgfile_intval6_unsigned (const TCHAR *option, const TCHAR *value, const TCHAR *name, const TCHAR *nameext, unsigned int *location, int scale);
+static int cfgfile_intval6_signed (const TCHAR *option, const TCHAR *value, const TCHAR *name, const TCHAR *nameext, int *location, int scale);
+#define cfgfile_intval6(a1,a2,a3,a4,a5,a6) _Generic((a5), unsigned int *: cfgfile_intval6_unsigned, default: cfgfile_intval6_signed)(a1,a2,a3,a4,a5,a6)
+  
+
+#if 1 /*def FSUAE*/ // NL
 #ifdef _WIN32
 #include <winsock2.h>
 #else
--- fs-uae-2.7.14dev/softfloat/softfloat-specialize.c	2016-09-04 00:08:55.000000000 +0200
+++ sources/softfloat/softfloat-specialize.c	2016-09-01 13:15:38.000000000 +0200
@@ -37,6 +37,14 @@
 #include "softfloat.h"
 #include "softfloat-specialize.h"
 
+#ifdef BX_BIG_ENDIAN
+#define STRUCT_packFloatx80(zSign, zExp, zSig) { (Bit16u)(((zSign) << 15) + (zExp)), (Bit64u)(zSig) }
+#define STRUCT_packFloat1282(zHi, zLo) { (Bit64u)(zHi), (Bit64u)(zLo) }
+#else /*BX_BIG_ENDIAN*/
+#define STRUCT_packFloatx80(zSign, zExp, zSig) { (Bit64u)(zSig), (Bit16u)(((zSign) << 15) + (zExp)) }
+#define STRUCT_packFloat1282(zHi, zLo) { (Bit64u)(zLo), (Bit64u)(zHi) }
+#endif /*BX_BIG_ENDIAN*/
+
 /*----------------------------------------------------------------------------
 | Takes two single-precision floating-point values \`a' and \`b', one of which
 | is a NaN, and returns the appropriate NaN result.  If either \`a' or \`b' is a
@@ -146,7 +154,7 @@
 | The pattern for a default generated extended double-precision NaN.
 *----------------------------------------------------------------------------*/
 const floatx80 floatx80_default_nan =
-    packFloatx80(0, floatx80_default_nan_exp, floatx80_default_nan_fraction);
+    STRUCT_packFloatx80(0, floatx80_default_nan_exp, floatx80_default_nan_fraction);
 
 #endif /* FLOATX80 */
 
@@ -188,6 +196,6 @@
 | The pattern for a default generated quadruple-precision NaN.
 *----------------------------------------------------------------------------*/
 const float128 float128_default_nan =
-    packFloat1282(float128_default_nan_hi, float128_default_nan_lo);
+    STRUCT_packFloat1282(float128_default_nan_hi, float128_default_nan_lo);
 
 #endif /* FLOAT128 */
--- fs-uae-2.7.14dev/src/ppc/ppcd.c	2016-09-04 00:08:55.000000000 +0200
+++ sources/src/ppc/ppcd.c	2016-09-01 02:43:11.000000000 +0200
@@ -104,7 +104,9 @@
 }
 
 // Simple instruction form + reserved bitmask.
-static void put(const char * mnem, u32 mask, u32 chkval=0, int iclass=PPC_DISA_OTHER)
+#define put2(a1,a2) put(a1,a2,0,PPC_DISA_OTHER)
+#define put3(a1,a2,a3) put(a1,a2,a3,PPC_DISA_OTHER)
+static void put(const char * mnem, u32 mask, u32 chkval, int iclass) //static void put(const char * mnem, u32 mask, u32 chkval=0, int iclass=PPC_DISA_OTHER)
 {
     if( (Instr & mask) != chkval ) { ill(); return; }
     o->iclass |= iclass;
@@ -182,7 +184,10 @@
 // dab LSB bits : [D][A][B] (D should always present)
 // 'hex' for logic opcodes, 's' for alu opcodes, 'crfD' and 'L' for cmp opcodes
 // 'imm': 1 to show immediate operand
-static void integer(const char *mnem, char form, int dab, int hex=0, int s=1, int crfD=0, int L=0, int imm=1)
+#define integer3(a1,a2,a3)       	integer(a1,a2,a3, 0, 1, 0, 0, 1)
+#define integer5(a1,a2,a3,a4,a5)	integer(a1,a2,a3,a4,a5, 0 ,0, 1)
+#define integer7(a1,a2,a3,a4,a5,a6,a7)	integer(a1,a2,a3,a4,a5,a6,a7, 1)
+static void integer(const char *mnem, char form, int dab, int hex, int s, int crfD, int L, int imm) // (const char *mnem, char form, int dab, int hex=0, int s=1, int crfD=0, int L=0, int imm=1)
 {
     char * ptr = o->operands;
     int rd = DIS_RD, ra = DIS_RA, rb = DIS_RB;
@@ -461,7 +466,9 @@
 }
 
 // CR logic operations
-static void crop(const char *name, const char *simp="", int ddd=0, int daa=0)
+#define crop1(a1) crop(a1,"",0,0)
+#define crop3(a1,a2,a3) crop(a1,a2,a3,0)
+static void crop(const char *name, const char *simp, int ddd, int daa) // crop(const char *name, const char *simp="", int ddd=0, int daa=0)
 {
     if(Instr & 1) { ill(); return; }
 
@@ -506,7 +513,8 @@
 }
 
 // Rotate left word.
-static void rlw(const char *name, int rb, int ins=0)
+#define rlw2(a1,a2) rlw(a1,a2,0)
+static void rlw(const char *name, int rb, int ins) // rlw(const char *name, int rb, int ins=0)
 {
     int mb = DIS_MB, me = DIS_ME;
     char * ptr = o->operands;
@@ -564,7 +572,11 @@
 }
 
 // Load/Store.
-static void ldst(const char *name, int x/*indexed*/, int load=1, int L=0, int string=0, int fload=0)
+#define ldst2(a1,a2) ldst(a1,a2,1,0,0,0)
+#define ldst3(a1,a2,a3) ldst(a1,a2,a3,0,0,0)
+#define ldst4(a1,a2,a3,a4) ldst(a1,a2,a3,a4,0,0)
+#define ldst5(a1,a2,a3,a4,a5) ldst(a1,a2,a3,a4,a5,0)
+static void ldst(const char *name, int x/*indexed*/, int load, int L, int string, int fload) // ldst(const char *name, int x/*indexed*/, int load=1, int L=0, int string=0, int fload=0)
 {
     if(x) integer3(name, fload ? 'F' : 'X', DAB_D|DAB_A|DAB_B);
     else
@@ -586,7 +598,8 @@
 }
 
 // Cache.
-static void cache(const char *name, int flag=PPC_DISA_OTHER)
+#define cache1(a1) cache(a1,PPC_DISA_OTHER)
+static void cache(const char *name, int flag) // cache(const char *name, int flag=PPC_DISA_OTHER)
 {
     if (DIS_RD) { ill(); return; }
     else
@@ -874,7 +887,8 @@
 #define FPU_DACB    4
 #define FPU_D       5
 
-static void fpu(const char *name, u32 mask, int type, int flag=PPC_DISA_OTHER)
+#define fpu3(a1,a2,a3) fpu(a1,a2,a3,PPC_DISA_OTHER)
+static void fpu(const char *name, u32 mask, int type, int flag) // fpu(const char *name, u32 mask, int type, int flag=PPC_DISA_OTHER)
 {
     int d = DIS_RD, a = DIS_RA, c = DIS_RC, b = DIS_RB;
 
EOF
)
