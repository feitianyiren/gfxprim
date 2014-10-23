@ include source.t
@ include savers.t
/*
 * Iterate over all pixel types, try to save and load back context.
 *
 * Copyright (C) 2009-2014 Cyril Hrubis <metan@ucw.cz>
 */

#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <core/GP_Context.h>
#include <core/GP_GetPutPixel.h>
#include <loaders/GP_Loaders.h>

#include "tst_test.h"

typedef int (*Save)(const GP_Context *src, const char *path, GP_ProgressCallback *callback);
typedef GP_Context *(*Load)(const char *path, GP_ProgressCallback *callback);

static int test(Save Saver, Load Loader, GP_PixelType pixel_type)
{
	GP_Context *src;
	GP_Context *res;
	unsigned int x, y;
	int ret = TST_SUCCESS;

	src = GP_ContextAlloc(100, 100, pixel_type);

	if (!src) {
		tst_msg("Malloc failed");
		return TST_UNTESTED;
	}

	for (x = 0; x < src->w; x++)
		for (y = 0; y < src->w; y++)
			GP_PutPixel(src, x, y, 0);

	if (Saver(src, "testfile", NULL)) {
		if (errno == ENOSYS) {
			tst_msg("Unimplemented pixel value");
			ret = TST_SKIPPED;
			goto err;
		}

		if (errno == EINVAL) {
			tst_msg("Invalid pixel value for the format");
			ret = TST_SKIPPED;
			goto err;
		}

		tst_msg("Saver failed with %s", strerror(errno));
		ret = TST_FAILED;
		goto err;
	}

	res = Loader("testfile", NULL);

	if (!res) {
		tst_msg("Failed to load saved image");
		ret = TST_FAILED;
		goto err;
	}

	tst_msg("Loaded back as %s", GP_PixelTypeName(res->pixel_type));

	if (res->w != src->w || res->h != src->h) {
		tst_msg("Invalid loaded image size %ux%u", res->w, res->h);
		ret = TST_FAILED;
	}

	if (GP_GetPixel(res, 0, 0) != 0) {
		tst_msg("Pixel value is wrong %x", GP_GetPixel(res, 0, 0));
		ret = TST_FAILED;
	}

	GP_ContextFree(res);
err:
	GP_ContextFree(src);
	return ret;
}

@ for fmt in fmts:
@     for pt in pixeltypes:
@         if not pt.is_unknown():
static int test_{{ fmt }}_{{ pt.name }}(void)
{
	return test(GP_Save{{ fmt }}, GP_Load{{ fmt }}, GP_PIXEL_{{ pt.name }});
}

@ end
@
const struct tst_suite tst_suite = {
	.suite_name = "SaveLoad",
	.tests = {
@ for fmt in fmts:
@     for pt in pixeltypes:
@         if not pt.is_unknown():
		{.name = "{{ fmt }} {{ pt.name }}",
		 .tst_fn = test_{{ fmt }}_{{ pt.name }},
		 .flags = TST_TMPDIR | TST_MALLOC_CANARIES},
@ end
		{.name = NULL},
	}
};
