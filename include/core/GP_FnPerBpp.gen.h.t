@ include header.t
/*
 * All FnPerBpp macros
 *
 * Copyright (C) 2011-2014 Cyril Hrubis <metan@ucw.cz>
 */

/*
 * Macros used to create draving functions from macros.
 */
#define GP_DEF_FN_PER_BPP(fname, MACRO_NAME, fdraw) \
@ for ps in pixelsizes:
	GP_DEF_FN_FOR_BPP(fname, MACRO_NAME, fdraw, {{ ps.suffix }}) \
@ end
@
@ def bpp_suffix(suffix):
@     if suffix == "LE" or suffix == "BE":
_{{ suffix }}
@     else:

@ end

/*
 * Branch on bpp and bit_endian.
 */
#define GP_FN_PER_BPP(FN_NAME, bpp, bit_endian, ...) \
	switch (bpp) { \
@ for bpp in pixelsizes_by_bpp.keys():
	case {{ bpp }}: \
@     if len(pixelsizes_by_bpp[bpp]) == 1:
		FN_NAME##_{{ bpp }}BPP{@ bpp_suffix(pixelsizes_by_bpp[bpp][0]) @}(__VA_ARGS__); \
@     else:
		if (bit_endian == GP_BIT_ENDIAN_LE) \
			FN_NAME##_{{ bpp }}BPP_LE(__VA_ARGS__); \
		else \
			FN_NAME##_{{ bpp }}BPP_BE(__VA_ARGS__); \
@     end
	break; \
@ end
	}

/*
 * Branch on bpp and bit_endian.
 */
#define GP_FN_RET_PER_BPP(FN_NAME, bpp, bit_endian, ...) \
	switch (bpp) { \
@ for bpp in pixelsizes_by_bpp.keys():
	case {{ bpp }}: \
@     if len(pixelsizes_by_bpp[bpp]) == 1:
		return FN_NAME##_{{ bpp }}BPP{@ bpp_suffix(pixelsizes_by_bpp[bpp][0]) @}(__VA_ARGS__); \
@     else:
		if (bit_endian == GP_BIT_ENDIAN_LE) \
			return FN_NAME##_{{ bpp }}BPP_LE(__VA_ARGS__); \
		else \
			return FN_NAME##_{{ bpp }}BPP_BE(__VA_ARGS__); \
@     end
	break; \
@ end
	}

