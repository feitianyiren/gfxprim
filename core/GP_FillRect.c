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
 * Copyright (C) 2009-2010 Jiri "BlueBear" Dluhos                            *
 *                         <jiri.bluebear.dluhos@gmail.com>                  *
 *                                                                           *
 * Copyright (C) 2009-2010 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#include "GP.h"

void GP_FillRectXYXY(GP_Context *context, int x0, int y0, int x1, int y1,
                     GP_Pixel pixel)
{
	GP_CHECK_CONTEXT(context);

	if (y0 > y1)
		GP_SWAP(y0, y1);

	int y;
	for (y = y0; y <= y1; y++)
		GP_HLine(context, x0, x1, y, pixel);
}

void GP_FillRectXYWH(GP_Context *context, int x, int y,
                     unsigned int w, unsigned int h, GP_Pixel pixel)
{
	/* zero width/height: draw nothing */
	if (w == 0 || h == 0)
		return;

	return GP_FillRectXYXY(context, x, y, x + w - 1, y + h - 1, pixel);
}

void GP_TFillRectXYXY(GP_Context *context, int x0, int y0,
                      int x1, int y1, GP_Pixel pixel)
{
	GP_CHECK_CONTEXT(context);
	
	GP_TRANSFORM_POINT(context, x0, y0);
	GP_TRANSFORM_POINT(context, x1, y1);

	GP_FillRect(context, x0, y0, x1, y1, pixel);
}

void GP_TFillRectXYWH(GP_Context *context, int x, int y,
	              unsigned int w, unsigned int h, GP_Pixel pixel)
{
	/* zero width/height: draw nothing */
	if (w == 0 || h == 0)
		return;

	GP_TFillRectXYXY(context, x, y, x + w - 1, y + h - 1, pixel);
}
