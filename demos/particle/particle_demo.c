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

   Particle demo.

  */

#include <signal.h>
#include <string.h>
#include <gfxprim.h>
#include <backends/GP_Backends.h>

#include "space.h"

static gp_pixel black_pixel;
static gp_pixel white_pixel;

static gp_backend *backend = NULL;
static gp_pixmap *pixmap = NULL;

static void sighandler(int signo)
{
	if (backend != NULL)
		gp_backend_exit(backend);

	fprintf(stderr, "Got signal %i\n", signo);

	exit(1);
}

static void init_backend(const char *backend_opts)
{
	backend = gp_backend_init(backend_opts, "Particles");

	if (backend == NULL) {
		fprintf(stderr, "Failed to initalize backend '%s'\n", backend_opts);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	const char *backend_opts = "X11";
	int opt;
	int pause_flag = 0;
	int particles = 160;

	while ((opt = getopt(argc, argv, "b:n:")) != -1) {
		switch (opt) {
		case 'b':
			backend_opts = optarg;
		break;
		case 'n':
			particles = atoi(optarg);
		break;
		default:
			fprintf(stderr, "Invalid paramter '%c'\n", opt);
		}
	}

	signal(SIGINT, sighandler);
	signal(SIGSEGV, sighandler);
	signal(SIGBUS, sighandler);
	signal(SIGABRT, sighandler);

	init_backend(backend_opts);

	pixmap = backend->pixmap;

	black_pixel = gp_rgb_to_pixmap_pixel(0x00, 0x00, 0x00, pixmap);
	white_pixel = gp_rgb_to_pixmap_pixel(0xff, 0xff, 0xff, pixmap);

	gp_fill(pixmap, black_pixel);
	gp_backend_flip(backend);

	struct space *space;
	space = space_create(particles, 10<<8, 10<<8, (pixmap->w - 10)<<8, (pixmap->h - 10)<<8);

	for (;;) {
		if (backend->poll)
			gp_backend_poll(backend);

		usleep(1000);

		/* Read and parse events */
		gp_event ev;

		while (gp_backend_get_event(backend, &ev)) {

			gp_event_dump(&ev);

			switch (ev.type) {
			case GP_EV_KEY:
				if (ev.code != GP_EV_KEY_DOWN)
					continue;

				switch (ev.val.key.key) {
				case GP_KEY_ESC:
				case GP_KEY_ENTER:
				case GP_KEY_Q:
					gp_backend_exit(backend);
					return 0;
				break;
				case GP_KEY_P:
					pause_flag = !pause_flag;
				break;
				case GP_KEY_G:
					space->gay = 1;
				break;
				case GP_KEY_T:
					space->gay = 0;
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
					space_destroy(space);
					space = space_create(particles,
					                     10<<8, 10<<8,
					                     (pixmap->w - 10)<<8,
					                     (pixmap->h - 10)<<8);
				break;
				}
			break;
			}
		}

		if (!pause_flag) {
			space_time_tick(space, 1);
			space_draw_particles(pixmap, space);
			gp_backend_flip(backend);
		}
	}

	gp_backend_exit(backend);

	return 0;
}
