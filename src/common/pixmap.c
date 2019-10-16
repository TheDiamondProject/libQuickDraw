/* Copyright (c) 2019 Tom Hancocks, The Diamond Project
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include "common/pixmap.h"

int qd_pixmap_parse(struct qd_pixmap **out_pm, struct buffer *restrict buffer)
{
    struct qd_pixmap *pm = calloc(1, sizeof(*pm));
    if (out_pm) {
        *out_pm = pm;
    }

    if (buffer_read(&pm->base_addr, sizeof(uint32_t), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read base address of pixmap.\n");
        goto ERROR;
    }

    if (buffer_read(&pm->row_bytes, sizeof(short), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the row_bytes of pixmap.\n");
        goto ERROR;
    }
    pm->row_bytes &= 0x7FFF;

    if (buffer_read(&pm->bounds, sizeof(short), 4, buffer) != 4) {
        fprintf(stderr, "Failed to read the bounds of the pixmap.\n");
        goto ERROR;
    }

    if (buffer_read(&pm->pm_version, sizeof(short), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the pixmap version.\n");
        goto ERROR;
    }

    if (buffer_read(&pm->pack_type, sizeof(short), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the pixmap pack type.\n");
        goto ERROR;
    }

    if (buffer_read(&pm->pack_size, sizeof(int32_t), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the pixmap pack size.\n");
        goto ERROR;
    }

    if (buffer_read_fixed(&pm->h_res, 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the horizontal resolution from the pixmap.\n");
        goto ERROR;
    }

    if (buffer_read_fixed(&pm->v_res, 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the vertical resolution from the pixmap.\n");
        goto ERROR;
    }

    if (buffer_read(&pm->pixel_type, sizeof(short), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the pixel type for the pixmap.\n");
        goto ERROR;
    }

    if (buffer_read(&pm->pixel_size, sizeof(short), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the pixel size for the pixmap.\n");
        goto ERROR;
    }

    if (buffer_read(&pm->cmp_count, sizeof(short), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the component count for the pixmap.\n");
        goto ERROR;
    }

    if (buffer_read(&pm->cmp_size, sizeof(short), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read the component size for the pixmap.\n");
        goto ERROR;
    }

    if (buffer_read(&pm->pixel_format, sizeof(uint32_t), 1, buffer) != 1) {
    	fprintf(stderr, "Failed to read the pixel format from the pixmap.\n");
    	goto ERROR;
    }

    if (buffer_read(&pm->pm_table, sizeof(uint32_t), 1, buffer) != 1) {
    	fprintf(stderr, "Failed to read the pixmap color table handle.\n");
    	goto ERROR;
    }

    if (buffer_read(&pm->pm_extension, sizeof(uint32_t), 1, buffer) != 1) {
    	fprintf(stderr, "Failed to read the extension for the pixmap.\n");
    	goto ERROR;
    }

    return 0;

ERROR:
	qd_pixmap_free(pm);
	return 1;
}

void qd_pixmap_free(struct qd_pixmap *pm)
{
    free(pm);
}
