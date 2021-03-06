/*****************************************************************************
 * This file is part of gfxprim library.                                     *
 *                                                                           *
 * Gfxprim is free software; you can redistribute it and/or                  *
 * modify it under the terms of the GNU Lesser General Public                *
 * License as published by the Free Software Foundation; either              *
 * version 2.1 of the License, or (at your option) any later version.        *
 *                                                                           *
 * Gfxprim is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Lesser General Public License for more details.                           *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public          *
 * License along with gfxprim; if not, write to the Free Software            *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,                        *
 * Boston, MA  02110-1301  USA                                               *
 *                                                                           *
 * Copyright (C) 2011      Tomas Gavenciak <gavento@ucw.cz>                  *
 *                                                                           *
 *****************************************************************************/

#ifndef CORE_GP_CONVERT_H
#define CORE_GP_CONVERT_H

#include "GP_Common.h"
#include "GP_Pixmap.h"
#include "GP_Pixel.h"

/* Generated headers */
#include "GP_Convert.gen.h"
#include "GP_Convert_Scale.gen.h"

/*
 * Generated function to convert RGB888 to any type.
 * Does not work with palette types.
 */
gp_pixel gp_RGB888_to_pixel(gp_pixel pixel, gp_pixel_type type);

/*
 * Generated function to convert RGBA8888 to any type.
 * Does not work with palette types.
 */
gp_pixel gp_RGBA8888_to_pixel(gp_pixel pixel, gp_pixel_type type);

/*
 * Generated function to convert to RGB888 from any type.
 * Does not work with palette types.
 */
gp_pixel gp_pixel_to_RGB888(gp_pixel pixel, gp_pixel_type type);

/*
 * Generated function to convert to RGBA8888 from any type.
 * Does not work with palette types.
 */
gp_pixel gp_pixel_to_RGBA8888(gp_pixel pixel, gp_pixel_type type);

/*
 * Converts a color specified by its R, G, B components to a specified type.
 */
static inline gp_pixel gp_rgb_to_pixel(uint8_t r, uint8_t g, uint8_t b,
                                       gp_pixel_type type)
{
	gp_pixel p = GP_PIXEL_CREATE_RGB888(r, g, b);
	return gp_RGB888_to_pixel(p, type);
}

/*
 * Converts a color specified by its R, G, B, A components to a specified type.
 */
static inline gp_pixel gp_rgba_to_pixel(uint8_t r, uint8_t g, uint8_t b,
                                        uint8_t a, gp_pixel_type type)
{
	gp_pixel p = GP_PIXEL_CREATE_RGBA8888(r, g, b, a);
	return gp_RGBA8888_to_pixel(p, type);
}

/*
 * Converts a color specified by its R, G, B components to a pixel value
 * compatible with the specified pixmap.
 */
static inline gp_pixel gp_rgb_to_pixmap_pixel(uint8_t r, uint8_t g, uint8_t b,
				              const gp_pixmap *pixmap)
{
	return gp_rgb_to_pixel(r, g, b, pixmap->pixel_type);
}

/*
 * Converts a color specified by its R, G, B, A components to a pixel value
 * compatible with the specified pixmap.
 */
static inline gp_pixel gp_rgba_to_pixmap_pixel(uint8_t r, uint8_t g,
                                               uint8_t b, uint8_t a,
					       const gp_pixmap *pixmap)
{
	return gp_rgba_to_pixel(r, g, b, a, pixmap->pixel_type);
}

/*
 * Convert between any pixel types (excl. palette types) via RGBA8888
 */
static inline gp_pixel gp_convert_pixel(gp_pixel pixel, gp_pixel_type from,
                                        gp_pixel_type to)
{
	return gp_RGBA8888_to_pixel(gp_pixel_to_RGBA8888(pixel, from), to);
}

/*
 * Convert between pixel types of given pixmaps (excl. palette types) via
 * RGBA8888.
 */
static inline gp_pixel gp_convert_pixmap_pixel(gp_pixel pixel,
                                               const gp_pixmap *from,
					       const gp_pixmap *to)
{
	return gp_RGBA8888_to_pixel(gp_pixel_to_RGBA8888(pixel, from->pixel_type),
	                            to->pixel_type);
}

#endif /* CORE_GP_CONVERT_H */
