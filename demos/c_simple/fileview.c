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
 * Copyright (C) 2009-2013 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gfxprim.h>

static gp_pixmap *win;
static gp_backend *backend;

static gp_pixel white_pixel, gray_pixel, dark_gray_pixel, black_pixel,
		red_pixel, blue_pixel;

static int font_flag = 0;
static int tracking = 0;

static int mul = 1;
static int space = 0;

static gp_font_face *font;

struct file_line {
	char *text;
	struct file_line *next;
	struct file_line *prev;
};

struct file_line *first_line = NULL;
struct file_line *last_line = NULL;

void redraw_screen(void)
{
	gp_fill(win, gray_pixel);

	gp_text_style style = GP_DEFAULT_TEXT_STYLE;

	switch (font_flag) {
	case 0:
		style.font = gp_font_gfxprim_mono;
	break;
	case 1:
		style.font = gp_font_gfxprim;
	break;
	case 2:
		style.font = gp_font_tiny_mono;
	break;
	case 3:
		style.font = gp_font_tiny;
	break;
	case 4:
		style.font = gp_font_c64;
	break;
	case 5:
		style.font = font;
	break;
	}

	style.pixel_xmul = mul;
	style.pixel_ymul = mul;
	style.pixel_xspace = space;
	style.pixel_yspace = space;
	style.char_xspace = tracking;

	/* Text alignment (we are always drawing to the right
	 * and below the starting point).
	 */
	int align = GP_ALIGN_RIGHT|GP_VALIGN_BELOW;

	struct file_line *line = first_line;
	unsigned int i;
	for (i = 0; i < win->h/gp_text_height(&style); i++) {
		if (line == NULL)
			break;
		gp_text(win, &style, 16, 16 + (1.0 * gp_text_height(&style))*i,
		        align, black_pixel, gray_pixel, line->text);
		line = line->next;
	}
}

static void warp_up(int lines)
{
	while (lines-- > 0)
		if (first_line->prev != NULL)
			first_line = first_line->prev;

	redraw_screen();
	gp_backend_flip(backend);
}

static void warp_down(int lines)
{
	while (lines-- > 0)
		if (first_line->next != NULL)
			first_line = first_line->next;

	redraw_screen();
	gp_backend_flip(backend);
}

void event_loop(void)
{
	gp_event ev;

	for (;;) {
		gp_backend_wait_event(backend, &ev);

		switch (ev.type) {
		case GP_EV_KEY:
			if (ev.code != GP_EV_KEY_DOWN)
				continue;

			switch (ev.val.key.key) {
			case GP_KEY_SPACE:
				if (font)
					font_flag = (font_flag + 1) % 6;
				else
					font_flag = (font_flag + 1) % 5;

				redraw_screen();
				gp_backend_flip(backend);
			break;
			case GP_KEY_RIGHT:
				tracking++;
				redraw_screen();
				gp_backend_flip(backend);
			break;
			case GP_KEY_LEFT:
				tracking--;
				redraw_screen();
				gp_backend_flip(backend);
			break;
			case GP_KEY_UP:
				warp_up(1);
			break;
			case GP_KEY_DOWN:
				warp_down(1);
			break;
			case GP_KEY_DOT:
				space++;
				redraw_screen();
				gp_backend_flip(backend);
			break;
			case GP_KEY_COMMA:
				space--;
				redraw_screen();
				gp_backend_flip(backend);
			break;
			case GP_KEY_RIGHT_BRACE:
				mul++;
				redraw_screen();
				gp_backend_flip(backend);
			break;
			case GP_KEY_LEFT_BRACE:
				if (mul > 0)
					mul--;
				redraw_screen();
				gp_backend_flip(backend);
			break;
			case GP_KEY_PAGE_UP:
				warp_up(30);
			break;
			case GP_KEY_PAGE_DOWN:
				warp_down(30);
			break;
			case GP_KEY_ESC:
				gp_backend_exit(backend);
				exit(0);
			break;
			}
		break;
		case GP_EV_SYS:
			switch(ev.code) {
			case GP_EV_SYS_QUIT:
				gp_backend_exit(backend);
				exit(0);
			break;
			case GP_EV_SYS_RESIZE:
				gp_backend_resize_ack(backend);
				redraw_screen();
				gp_backend_flip(backend);
			break;
			}
		break;
		}
	}
}

static int read_file_head(const char *filename)
{
	FILE *f = fopen(filename, "r");
	char buf[512];

	if (f == NULL) {
		fprintf(stderr, "Could not open file: %s\n", filename);
		return 0;
	}

	for (;;) {

		if (fgets(buf, 511, f) == NULL)
			break;

		struct file_line *line = malloc(sizeof(*line));
		line->text = strdup(buf);
		line->next = NULL;
		line->prev = NULL;

		if (first_line == NULL) {
			first_line = line;
			last_line = line;
		} else {
			line->prev = last_line;
			last_line->next = line;
			last_line = line;
		}
	}

	fclose(f);
	return 1;
}

int main(int argc, char *argv[])
{
	const char *backend_opts = "X11";

	if (argc == 1) {
		fprintf(stderr, "No file specified\n");
		return 1;
	}

	if (argc > 2)
		font = gp_font_face_load(argv[2], 0, 16);

	if (!read_file_head(argv[1]))
		return 1;

	backend = gp_backend_init(backend_opts, "File View");

	if (backend == NULL) {
		fprintf(stderr, "Failed to initalize backend '%s'\n",
		        backend_opts);
		return 1;
	}

	win = backend->pixmap;

	white_pixel     = gp_rgb_to_pixmap_pixel(0xff, 0xff, 0xff, win);
	gray_pixel      = gp_rgb_to_pixmap_pixel(0xbe, 0xbe, 0xbe, win);
	dark_gray_pixel = gp_rgb_to_pixmap_pixel(0x7f, 0x7f, 0x7f, win);
	black_pixel     = gp_rgb_to_pixmap_pixel(0x00, 0x00, 0x00, win);
	red_pixel       = gp_rgb_to_pixmap_pixel(0xff, 0x00, 0x00, win);
	blue_pixel      = gp_rgb_to_pixmap_pixel(0x00, 0x00, 0xff, win);

	redraw_screen();
	gp_backend_flip(backend);

	event_loop();

	return 0;
}
