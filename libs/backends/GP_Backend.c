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
 * Copyright (C) 2009-2013 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#include <inttypes.h>
#include <poll.h>

#include <core/GP_Common.h>
#include <core/GP_Transform.h>
#include <core/GP_Pixmap.h>
#include <core/GP_Debug.h>

#include <input/GP_EventQueue.h>
#include <input/GP_TimeStamp.h>

#include <backends/GP_Backend.h>

void gp_backend_update_rect_xyxy(gp_backend *self,
                                 gp_coord x0, gp_coord y0,
                                 gp_coord x1, gp_coord y1)
{
	if (!self->update_rect)
		return;

	GP_TRANSFORM_POINT(self->pixmap, x0, y0);
	GP_TRANSFORM_POINT(self->pixmap, x1, y1);

	if (x1 < x0)
		GP_SWAP(x0, x1);

	if (y1 < y0)
		GP_SWAP(y0, y1);

	if (x0 < 0) {
		GP_WARN("Negative x coordinate %i, clipping to 0", x0);
		x0 = 0;
	}

	if (y0 < 0) {
		GP_WARN("Negative y coordinate %i, clipping to 0", y0);
		y0 = 0;
	}

	gp_coord w = self->pixmap->w;

	if (x1 >= w) {
		GP_WARN("Too large x coordinate %i, clipping to %u", x1, w - 1);
		x1 = w - 1;
	}

	gp_coord h = self->pixmap->h;

	if (y1 >= h) {
		GP_WARN("Too large y coordinate %i, clipping to %u", y1, h - 1);
		y1 = h - 1;
	}

	self->update_rect(self, x0, y0, x1, y1);
}

int gp_backend_resize(gp_backend *self, uint32_t w, uint32_t h)
{
	if (!self->set_attrs)
		return 1;

	if (w == 0)
		w = self->pixmap->w;

	if (h == 0)
		h = self->pixmap->h;

	if (self->pixmap->w == w && self->pixmap->h == h)
		return 0;

	return self->set_attrs(self, w, h, NULL);
}

int gp_backend_resize_ack(gp_backend *self)
{
	GP_DEBUG(2, "Calling backend %s resize_ack()", self->name);

	if (self->resize_ack)
		return self->resize_ack(self);

	return 0;
}

static uint32_t pushevent_callback(gp_timer *self)
{
	gp_event ev;

	ev.type = GP_EV_TMR;
	gettimeofday(&ev.time, NULL);
	ev.val.tmr = self;

	gp_event_queue_put(self->_priv, &ev);

	return 0;
}

void gp_backend_add_timer(gp_backend *self, gp_timer *timer)
{
	if (timer->callback == NULL) {
		timer->callback = pushevent_callback;
		timer->_priv = &self->event_queue;
	}

	gp_timer_queue_insert(&self->timers, gp_time_stamp(), timer);
}

void gp_backend_rem_timer(gp_backend *self, gp_timer *timer)
{
	gp_timer_queue_remove(&self->timers, timer);
}

void gp_backend_poll(gp_backend *self)
{
	self->poll(self);

	if (self->timers)
		gp_timer_queue_process(&self->timers, gp_time_stamp());
}

static void wait_timers_fd(gp_backend *self, uint64_t now)
{
	int timeout;

	timeout = self->timers->expires - now;

	struct pollfd fd = {.fd = self->fd, .events = POLLIN, fd.revents = 0};

	if (poll(&fd, 1, timeout))
		self->poll(self);

	now = gp_time_stamp();

	gp_timer_queue_process(&self->timers, now);
}

/*
 * Polling for backends that does not expose FD to wait on (namely SDL).
 */
static void wait_timers_poll(gp_backend *self)
{
	for (;;) {
		uint64_t now = gp_time_stamp();

		if (gp_timer_queue_process(&self->timers, now))
			return;

		self->poll(self);

		if (gp_backend_events_queued(self))
			return;

		usleep(10000);
	}
}

void gp_backend_wait(gp_backend *self)
{
	if (self->timers) {
		uint64_t now = gp_time_stamp();

		/* Get rid of possibly expired timers */
		if (gp_timer_queue_process(&self->timers, now))
			return;

		/* Wait for events or timer expiration */
		if (self->fd != -1)
			wait_timers_fd(self, now);
		else
			wait_timers_poll(self);

		return;
	}

	self->wait(self);
}

int gp_backend_wait_event(gp_backend *self, gp_event *ev)
{
	int ret;

	for (;;) {
		if ((ret = gp_backend_get_event(self, ev)))
			return ret;

		gp_backend_wait(self);
	}
}

int gp_backend_poll_event(gp_backend *self, gp_event *ev)
{
	int ret;

	if ((ret = gp_backend_get_event(self, ev)))
		return ret;

	gp_backend_poll(self);

	if ((ret = gp_backend_get_event(self, ev)))
		return ret;

	return 0;
}
