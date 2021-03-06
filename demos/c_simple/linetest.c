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

#include <math.h>

#include <gfxprim.h>

/* Values for color pixels in display format. */
static gp_pixel black, white;

static double start_angle = 0.0;

static int aa_flag = 0;
static int pause_flag = 0;

static gp_backend *win;

void redraw_screen(void)
{
	double angle;
	int x, y;
	int w = win->pixmap->w;
	int h = win->pixmap->h;
	int xcenter = w/2;
	int ycenter = h/2;

	gp_fill(win->pixmap, black);

	for (angle = 0.0; angle < 2*M_PI; angle += 0.1) {
		x = (int) (w/2 * cos(start_angle + angle));
		y = (int) (h/2 * sin(start_angle + angle));

		int r = 127.0 + 127.0 * cos(start_angle + angle);
		int b = 127.0 + 127.0 * sin(start_angle + angle);

		gp_pixel pixel;
		pixel = gp_rgb_to_pixel(r, 0, b, win->pixmap->pixel_type);

		if (aa_flag) {
			gp_line_aa_raw(win->pixmap, GP_FP_FROM_INT(xcenter), GP_FP_FROM_INT(ycenter),
				GP_FP_FROM_INT(xcenter + x), GP_FP_FROM_INT(ycenter + y), pixel);
		} else {
			gp_line(win->pixmap, xcenter + x, ycenter + y, xcenter, ycenter, pixel);
			gp_line(win->pixmap, xcenter, ycenter, xcenter + x, ycenter + y, pixel);
		}
	}

	gp_backend_flip(win);

	/* axes */
//	gp_hline_xyw(&pixmap, 0, ycenter, display->w, white);
//	gp_vlineXYH(&pixmap, xcenter, 0, display->h, white);
}

void event_loop(void)
{
	gp_event ev;

	while (gp_backend_get_event(win, &ev)) {
		switch (ev.type) {
		case GP_EV_KEY:
			if (ev.code != GP_EV_KEY_DOWN)
				continue;

			switch (ev.val.key.key) {
			case GP_KEY_A:
				aa_flag = !aa_flag;
			break;
			case GP_KEY_ESC:
				gp_backend_exit(win);
				exit(0);
			break;
			case GP_KEY_P:
				pause_flag = !pause_flag;
			break;
			}
		break;
		case GP_EV_SYS:
			switch(ev.code) {
			case GP_EV_SYS_QUIT:
				gp_backend_exit(win);
				exit(0);
			break;
			case GP_EV_SYS_RESIZE:
				gp_backend_resize_ack(win);
			break;
			}
		break;
		}
	}
}

int main(int argc, char *argv[])
{
	const char *backend_opts = "X11";
	int opt;

	while ((opt = getopt(argc, argv, "b:")) != -1) {
		switch (opt) {
		case 'b':
			backend_opts = optarg;
		break;
		default:
			fprintf(stderr, "Invalid paramter '%c'\n", opt);
		}
	}

	win = gp_backend_init(backend_opts, "Line Test");

	if (win == NULL) {
		fprintf(stderr, "Failed to initalize backend '%s'\n",
		        backend_opts);
		return 1;
	}

	white = gp_rgb_to_pixmap_pixel(0xff, 0xff, 0xff, win->pixmap);
	black = gp_rgb_to_pixmap_pixel(0x00, 0x00, 0x00, win->pixmap);

	redraw_screen();

	for (;;) {
		gp_backend_poll(win);
		event_loop();

		usleep(20000);

		if (pause_flag)
			continue;

		redraw_screen();
		gp_backend_flip(win);

		start_angle += 0.01;
		if (start_angle > 2*M_PI) {
			start_angle = 0.0;
		}
	}
}
