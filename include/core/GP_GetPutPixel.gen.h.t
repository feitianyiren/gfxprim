%% extends "base.h.t"

%% block descr
Access pixel bytes, Get and PutPixel
Do not include directly, use GP_Pixel.h
%% endblock

%% block body

#include "GP_Common.h"

struct GP_Context;

%% for ps in pixelsizes
/* 
 * macro to get address of pixel in a {{ ps.suffix }} context
 */
#define GP_PIXEL_ADDR_{{ ps.suffix }}(context, x, y) \
	((GP_Pixel*)(((void*)((context)->pixels)) + (context)->bytes_per_row * (y) + ({{ ps.size }} * (x)) / 8))

/*
 * macro to get bit-offset of pixel in {{ ps.suffix }} context
 */
#define GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x) \
%% if not ps.needs_bit_endian()
	(0)
%% else
%% if ps.bit_endian == LE
%% if ps.size < 8
	(((x) % {{ 8 // ps.size }}) * {{ ps.size }})
%% else
	(({{ ps.size }} * (x)) % 8)
%% endif
%% else
%% if ps.size < 8
	({{ 8 - ps.size }} - ((x) % {{ 8 // ps.size }}) * {{ ps.size }})
%% else
	{{ error('Insanity check: big bit-endian with >8 bpp. Are you sure?') }}
%% endif
%% endif
%% endif

/*
 * GP_GetPixel for {{ ps.suffix }} 
 */
static inline GP_Pixel GP_GetPixel_Raw_{{ ps.suffix }}(const GP_Context *c, int x, int y)
{
	return GP_GET_BITS(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
		*(GP_PIXEL_ADDR_{{ ps.suffix}}(c, x, y)));
}

/*
 * GP_PutPixel for {{ ps.suffix }} 
 */
static inline void GP_PutPixel_Raw_{{ ps.suffix }}(GP_Context *c, int x, int y, GP_Pixel p)
{
	GP_SET_BITS(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
		*(GP_PIXEL_ADDR_{{ ps.suffix}}(c, x, y)), p);
}

%% endfor

%% endblock body