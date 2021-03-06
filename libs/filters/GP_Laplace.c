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

#include <core/GP_Debug.h>
#include <core/GP_GetPutPixel.h>
#include <filters/GP_Linear.h>
#include <filters/GP_Laplace.h>

int gp_filter_laplace(const gp_pixmap *src, gp_pixmap *dst,
		      gp_progress_cb *callback)
{
	GP_DEBUG(1, "Laplace filter %ux%u", src->w, src->h);

	float kern[9] = {0,  1,  0,
	                 1, -4,  1,
	                 0,  1,  0};

	if (gp_filter_linear_convolution_raw(src, 0, 0, src->w, src->h,
	                                     dst, 0, 0, kern, 3, 3, 1, callback))
		return 1;

	return 0;
}

gp_pixmap *gp_filter_laplace_alloc(const gp_pixmap *src,
                                   gp_progress_cb *callback)
{
	gp_pixmap *ret = gp_pixmap_copy(src, 0);

	if (ret == NULL)
		return NULL;

	if (gp_filter_laplace(src, ret, callback)) {
		gp_pixmap_free(ret);
		return NULL;
	}

	return ret;
}


int gp_filter_edge_sharpening(const gp_pixmap *src, gp_pixmap *dst,
                              float w, gp_progress_cb *callback)
{
	/* Identity kernel */
	float kern[9] = {0,  0,  0,
	                 0,  1,  0,
	                 0,  0,  0};

	GP_DEBUG(1, "Laplace Edge Sharpening filter %ux%u w=%f",
	         src->w, src->h, w);

	/* Create combined kernel */
	kern[1] -=  1.00 * w;
	kern[3] -=  1.00 * w;
	kern[4] -= -4.00 * w;
	kern[5] -=  1.00 * w;
	kern[7] -=  1.00 * w;

	if (gp_filter_linear_convolution_raw(src, 0, 0, src->w, src->h,
	                                     dst, 0, 0,  kern, 3, 3, 1, callback))
		return 1;

	return 0;
}

gp_pixmap *gp_filter_edge_sharpening_alloc(const gp_pixmap *src, float w,
                                           gp_progress_cb *callback)
{
	gp_pixmap *ret = gp_pixmap_copy(src, 0);

	if (ret == NULL)
		return NULL;

	if (gp_filter_edge_sharpening(src, ret, w, callback)) {
		gp_pixmap_free(ret);
		return NULL;
	}

	return ret;
}
