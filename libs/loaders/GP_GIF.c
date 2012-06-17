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

  GIF image support using giflib.
  
 */

#include <stdint.h>
#include <inttypes.h>

#include <errno.h>
#include <string.h>

#include "../../config.h"
#include "core/GP_Pixel.h"
#include "core/GP_GetPutPixel.gen.h"
#include "gfx/GP_Fill.h"
#include "core/GP_Debug.h"

#include "GP_GIF.h"

#ifdef HAVE_GIFLIB

#include <gif_lib.h>

int GP_OpenGIF(const char *src_path, void **f)
{
	GifFileType *gf;

	gf = DGifOpenFileName(src_path);

	if (gf == NULL) {
		//TODO: error handling
		errno = EIO;
		return 1;
	}

	GP_DEBUG(1, "Have GIF image %ix%i, %i colors, %i bpp",
	         gf->SWidth, gf->SHeight, gf->SColorResolution,
		 gf->SColorMap ? gf->SColorMap->BitsPerPixel : -1);

	*f = gf;

	return 0;
}

static const char *rec_type_name(GifRecordType rec_type)
{
	switch (rec_type) {
	case UNDEFINED_RECORD_TYPE:
		return "Undefined";
	case SCREEN_DESC_RECORD_TYPE:
		return "ScreenDesc";
	case IMAGE_DESC_RECORD_TYPE:
		return "ImageDesc";
	case EXTENSION_RECORD_TYPE:
		return "Extension";
	case TERMINATE_RECORD_TYPE:
		return "Terminate";
	default:
		return "Invalid";
	}
}

static const char *gif_err_name(int err)
{
	switch (err) {
	case E_GIF_ERR_OPEN_FAILED:
		return "E_GIF_ERR_OPEN_FAILED";
	case E_GIF_ERR_WRITE_FAILED:
		return "E_GIF_ERR_WRITE_FAILED";
	case E_GIF_ERR_HAS_SCRN_DSCR:
		return "E_GIF_ERR_HAS_SCRN_DSCR";
	case E_GIF_ERR_HAS_IMAG_DSCR:
		return "E_GIF_ERR_HAS_IMAG_DSCR";
	case E_GIF_ERR_NO_COLOR_MAP:
		return "E_GIF_ERR_NO_COLOR_MAP";
	case E_GIF_ERR_DATA_TOO_BIG:
		return "E_GIF_ERR_DATA_TOO_BIG";
	case E_GIF_ERR_NOT_ENOUGH_MEM:
		return "E_GIF_ERR_NOT_ENOUGH_MEM";
	case E_GIF_ERR_DISK_IS_FULL:
		return "E_GIF_ERR_DISK_IS_FULL";
	case E_GIF_ERR_CLOSE_FAILED:
		return "E_GIF_ERR_CLOSE_FAILED";
	case E_GIF_ERR_NOT_WRITEABLE:
		return "E_GIF_ERR_NOT_WRITEABLE";
	default:
		return "UNKNOWN";
	}
}

static int read_extensions(GifFileType *gf)
{
	uint8_t *gif_ext_ptr;
	int gif_ext_type;

	//TODO: Should we free them?

	if (DGifGetExtension(gf, &gif_ext_type, &gif_ext_ptr) != GIF_OK) {
		GP_DEBUG(1, "DGifGetExtension() error %s (%i)",
		         gif_err_name(GifLastError()), GifLastError());
		return EIO;
	}

	GP_DEBUG(2, "Have GIF extension type %i (ignoring)", gif_ext_type);

	do {
		if (DGifGetExtensionNext(gf, &gif_ext_ptr) != GIF_OK) {
			GP_DEBUG(1, "DGifGetExtension() error %s (%i)",
			         gif_err_name(GifLastError()), GifLastError());
			return EIO;
		}

	} while (gif_ext_ptr != NULL);

	return 0;
}

static inline GifColorType *get_color_from_map(ColorMapObject *map, int idx)
{
	if (map->ColorCount <= idx) {
		GP_DEBUG(1, "Invalid colormap index %i (%i max)",
		         map->ColorCount, idx);
		return map->Colors;
	}

	return &map->Colors[idx];
}

static inline GP_Pixel get_color(GifFileType *gf, uint32_t idx)
{
	GifColorType *color;

	//TODO: no color map?
	if (gf->SColorMap == NULL)
		return 0;

	color = get_color_from_map(gf->SColorMap, idx);

	return  GP_Pixel_CREATE_RGB888(color->Red, color->Green, color->Blue);
}

static int get_bg_color(GifFileType *gf, GP_Pixel *pixel)
{
	GifColorType *color;
	
	if (gf->SColorMap == NULL)
		return 0;

	color = get_color_from_map(gf->SColorMap, gf->SBackGroundColor);

	*pixel = GP_Pixel_CREATE_RGB888(color->Red, color->Green, color->Blue);
	
	return 1;
}

GP_Context *GP_ReadGIF(void *f, GP_ProgressCallback *callback)
{
	GifFileType *gf = f;
	GifRecordType rec_type;
	GP_Context *res = NULL;
	GP_Pixel bg;
	int32_t x, y;
	int err;

	do {
		if (DGifGetRecordType(gf, &rec_type) != GIF_OK) {
			//TODO: error handling
			GP_DEBUG(1, "DGifGetRecordType() error %s (%i)",
			         gif_err_name(GifLastError()), GifLastError()); 
			err = EIO;
			goto err1;
		}
		
		GP_DEBUG(2, "Have GIF record type %s",
		         rec_type_name(rec_type));

		switch (rec_type) {
		case EXTENSION_RECORD_TYPE:
			if ((err = read_extensions(gf)))
				goto err1;
			continue;
		case IMAGE_DESC_RECORD_TYPE:
		break;
		default:
			continue;
		}

		if (DGifGetImageDesc(gf) != GIF_OK) {
			//TODO: error handling
			GP_DEBUG(1, "DGifGetImageDesc() error %s (%i)",
			         gif_err_name(GifLastError()), GifLastError()); 
			err = EIO;
			goto err1;
		}

		GP_DEBUG(1, "Have GIF Image left-top %ix%i, width-height %ix%i,"
		         " interlace %i, bpp %i", gf->Image.Left, gf->Image.Top,
			 gf->Image.Width, gf->Image.Height, gf->Image.Interlace,
			 gf->Image.ColorMap ? gf->Image.ColorMap->BitsPerPixel : -1);

		res = GP_ContextAlloc(gf->SWidth, gf->SHeight, GP_PIXEL_RGB888);
		
		if (res == NULL) {
			err = ENOMEM;
			goto err1;
		}
		
		/* If background color is defined, use it */
		if (get_bg_color(gf, &bg)) {
			GP_DEBUG(1, "Filling bg color %x", bg);	
			GP_Fill(res, bg);
		}

		/* Now finally read gif image data */
		for (y = gf->Image.Top; y < gf->Image.Height; y++) {
			uint8_t line[gf->Image.Width];

			DGifGetLine(gf, line, gf->Image.Width);
			
			//TODO: just now we have only 8BPP
			for (x = 0; x < gf->Image.Width; x++)
				GP_PutPixel_Raw_24BPP(res, x + gf->Image.Left, y, get_color(gf, line[x]));
			
			if (GP_ProgressCallbackReport(callback, y - gf->Image.Top,
			                              gf->Image.Height,
						      gf->Image.Width)) {
				GP_DEBUG(1, "Operation aborted");
				err = ECANCELED;
				goto err2;
			}
		}

		//TODO: now we exit after reading first image
		break;

	} while (rec_type != TERMINATE_RECORD_TYPE);
	

	DGifCloseFile(gf);

	/* No Image record found :( */
	if (res == NULL)
		err = EIO;

	return res;
err2:
	GP_ContextFree(res);
err1:
	DGifCloseFile(gf);
	errno = err;
	return NULL;
}

GP_Context *GP_LoadGIF(const char *src_path, GP_ProgressCallback *callback)
{
	void *f;

	if (GP_OpenGIF(src_path, &f))
		return NULL;

	return GP_ReadGIF(f, callback);
}

#else

int GP_OpenGIF(const char GP_UNUSED(*src_path),
               void GP_UNUSED(**f))
{
	errno = ENOSYS;
	return 1;
}

GP_Context *GP_ReadGIF(void GP_UNUSED(*f),
                       GP_ProgressCallback GP_UNUSED(*callback))
{
	errno = ENOSYS;
	return NULL;
}

GP_Context *GP_LoadGIF(const char GP_UNUSED(*src_path),
                       GP_ProgressCallback GP_UNUSED(*callback))
{
	errno = ENOSYS;
	return NULL;
}

#endif /* HAVE_GIFLIB */