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

#ifndef GP_CLIP_RECT_H
#define GP_CLIP_RECT_H

#include <SDL/SDL.h>

/*
 * Loads the clipping rectangle of the given surface into variables
 * whose names are passed in 'xmin', 'xmax', 'ymin', 'ymax'.
 */
#define GP_GET_CLIP_RECT(surf, xmin, xmax, ymin, ymax) { \
	xmin = surf->clip_rect.x; \
	xmax = surf->clip_rect.x + surf->clip_rect.w - 1; \
	ymin = surf->clip_rect.y; \
	ymax = surf->clip_rect.y + surf->clip_rect.h - 1; \
}

#endif /* GP_CLIP_RECT_H */

