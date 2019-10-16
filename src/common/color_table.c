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
#include "common/color_table.h"

struct qd_color_table *qd_color_table_parse(struct buffer *restrict buffer)
{
    struct qd_color_table *color_table = calloc(1, sizeof(*color_table));

    if (buffer_read(&color_table->ct_seed, sizeof(uint32_t), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read color table seed.\n");
        goto ERROR;
    }

    if (buffer_read(&color_table->ct_flags, sizeof(short), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read color table flags.\n");
        goto ERROR;
    }

    if (buffer_read(&color_table->ct_size, sizeof(short), 1, buffer) != 1) {
        fprintf(stderr, "Failed to read color table size.\n");
        goto ERROR;
    }

    // Allocate space for each of the color entries.
    color_table->ct_table = calloc(color_table->ct_size + 1, sizeof(*color_table->ct_table));
    for (int i = 0; i <= color_table->ct_size; ++i) {
        // Read the pixel value
        if (buffer_read(&color_table->ct_table[i].value, sizeof(unsigned short), 1, buffer) != 1) {
            fprintf(stderr, "Failed to read color table entry (pixel value).\n");
            goto ERROR;
        }

        // Read the pixel red value
        if (buffer_read(&color_table->ct_table[i].rgb.red, sizeof(unsigned short), 1, buffer) != 1) {
            fprintf(stderr, "Failed to read color table entry (red value).\n");
            goto ERROR;
        }

        // Read the pixel green value
        if (buffer_read(&color_table->ct_table[i].rgb.green, sizeof(unsigned short), 1, buffer) != 1) {
            fprintf(stderr, "Failed to read color table entry (green value).\n");
            goto ERROR;
        }

        // Read the pixel blue value
        if (buffer_read(&color_table->ct_table[i].rgb.blue, sizeof(unsigned short), 1, buffer) != 1) {
            fprintf(stderr, "Failed to read color table entry (blue value).\n");
            goto ERROR;
        }
    }

    return color_table;

ERROR:
    qd_color_table_free(color_table);
    return NULL;
}

void qd_color_table_free(struct qd_color_table *color_table)
{
    if (color_table) {
        free(color_table->ct_table);
        free(color_table);
    }
}
