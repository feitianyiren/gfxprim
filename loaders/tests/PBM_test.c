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

 /*

  */

#include <GP.h>
#include <GP_PBM.h>

#define W 91
#define H 89

int main(void)
{
	GP_Context *context = GP_ContextAlloc(W, H, GP_PIXEL_G1);

	if (context == NULL) {
		fprintf(stderr, "Couldn't allocate context\n");
		return 1;
	}

	GP_Line(context, 0, 0, W, H, 1);
	GP_FillCircle(context, 20, 20, 9, 1);
	GP_FillCircle(context, 20, 20, 7, 0);
	GP_FillCircle(context, 20, 20, 4, 1);
	GP_Text(context, NULL, 10, 40, GP_VALIGN_BELOW | GP_ALIGN_RIGHT, "Test  Test", 1);

	if (GP_SavePBM("test.pbm", context)) {
		fprintf(stderr, "Can't save context\n");
		return 1;
	}

	GP_ContextFree(context);

	return 0;
}