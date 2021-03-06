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
 * Copyright (C) 2009-2012 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

 /*

   Sigma Lee filter.

   The xrad and yrad denotes radius the filter works on. The number of neighbor
   pixels is exactly 2 * rad + 1 for both directions.

   The sigma denotes maximal symetric difference of pixels scaled to [0,1]
   interval. Greater sigma causes results to be closer to mean linear filter.

   The min parameter defines minimial number of pixels that must be in the two
   sigma iterval, if there is a less pixels in this interval the new pixel
   value is computed as mean of the surrounding pixels (not including the
   center one).

  */

#ifndef FILTERS_GP_SIGMA_H
#define FILTERS_GP_SIGMA_H

#include <filters/GP_Filter.h>

int gp_filter_sigma_ex(const gp_pixmap *src,
                       gp_coord x_src, gp_coord y_src,
                       gp_size w_src, gp_size h_src,
                       gp_pixmap *dst,
                       gp_coord x_dst, gp_coord y_dst,
                       int xrad, int yrad,
                       unsigned int min, float sigma,
                       gp_progress_cb *callback);

gp_pixmap *gp_filter_sigma_ex_alloc(const gp_pixmap *src,
                                    gp_coord x_src, gp_coord y_src,
                                    gp_size w_src, gp_size h_src,
                                    int xrad, int yrad,
                                    unsigned int min, float sigma,
                                    gp_progress_cb *callback);

static inline int gp_filter_sigma(const gp_pixmap *src,
                                  gp_pixmap *dst,
                                  int xrad, int yrad,
                                  unsigned int min, float sigma,
                                  gp_progress_cb *callback)
{
	return gp_filter_sigma_ex(src, 0, 0, src->w, src->h,
	                          dst, 0, 0, xrad, yrad, min, sigma, callback);
}

static inline gp_pixmap *gp_filter_sigma_alloc(const gp_pixmap *src,
                                               int xrad, int yrad,
                                               unsigned int min, float sigma,
                                               gp_progress_cb *callback)
{
	return gp_filter_sigma_ex_alloc(src, 0, 0, src->w, src->h,
	                                xrad, yrad, min, sigma, callback);
}

#endif /* FILTERS_GP_SIGMA_H */
