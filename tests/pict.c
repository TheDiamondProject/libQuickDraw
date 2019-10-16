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
#include "pict/pict.h"

#if defined(UNIT_TEST)

TEST_CASE(PICT, ParseResourceData)
{
    struct qd_buffer *pm_buffer = qd_buffer_open("tests/test.pict");

    struct qd_pict *pict = NULL;
    int err = qd_pict_parse(&pict, pm_buffer);
    // ASSERT_EQ(err, 0);

    ASSERT_EQ(pict->frame.left, 0);
    ASSERT_EQ(pict->frame.right, 126);
    ASSERT_EQ(pict->frame.top, 0);
    ASSERT_EQ(pict->frame.bottom, 149);

    ASSERT_EQ(pict->x_ratio, 1.0);
    ASSERT_EQ(pict->y_ratio, 1.0);

    ASSERT_NEQ(pict->pm, NULL);
    ASSERT_EQ(pict->pm->bounds.top, 0);
    ASSERT_EQ(pict->pm->bounds.left, 0);
    ASSERT_EQ(pict->pm->bounds.bottom, 149);
    ASSERT_EQ(pict->pm->bounds.right, 126);

    qd_pict_free(pict);
    qd_buffer_free(pm_buffer);
}

#endif