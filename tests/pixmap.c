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

#include <libUnit/unit.h>
#include "common/pixmap.h"

#if defined(UNIT_TEST)

TEST_CASE(PixMap, ParseResourceData)
{
    struct qd_buffer *pm_buffer = qd_buffer_open("tests/test.pixmap");
    struct qd_pixmap *pm = NULL;
    int err = qd_pixmap_parse(&pm, pm_buffer);

    ASSERT_EQ(err, 0);
    ASSERT_EQ(pm->row_bytes, 252);
    ASSERT_EQ(pm->bounds.top, 0);
    ASSERT_EQ(pm->bounds.left, 0);
    ASSERT_EQ(pm->bounds.bottom, 149);
    ASSERT_EQ(pm->bounds.right, 126);
    ASSERT_EQ(pm->pm_version, 0);
    ASSERT_EQ(pm->pack_type, 3);
    ASSERT_EQ(pm->pack_size, 0);
    ASSERT_EQ(pm->h_res, 72);
    ASSERT_EQ(pm->v_res, 72);
    ASSERT_EQ(pm->pixel_type, 16);
    ASSERT_EQ(pm->pixel_size, 16);
    ASSERT_EQ(pm->cmp_count, 3);
    ASSERT_EQ(pm->cmp_size, 5);
    ASSERT_EQ(pm->pixel_format, 0);

    qd_pixmap_free(pm);
    qd_buffer_free(pm_buffer);
}

#endif
