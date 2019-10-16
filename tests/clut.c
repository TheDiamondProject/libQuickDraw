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
#include "common/color_table.h"

#if defined(UNIT_TEST)

TEST_CASE(ColorTable, ParseResourceData)
{
    struct qd_buffer *clut_buffer = qd_buffer_open("tests/test.clut");
    struct qd_color_table *clut = qd_color_table_parse(clut_buffer);

    // Check the number of colors defined in the Color Table.
    ASSERT_EQ(clut->ct_seed, 0x0000);
    ASSERT_EQ(clut->ct_flags, 0x0000);
    ASSERT_EQ(clut->ct_size, 0x0002);

    // Check each of the colors in the table
    ASSERT_EQ(clut->ct_table[0].rgb.red, 0xFFFF);
    ASSERT_EQ(clut->ct_table[0].rgb.green, 0xFFFF);
    ASSERT_EQ(clut->ct_table[0].rgb.blue, 0xFFFF);

    ASSERT_EQ(clut->ct_table[1].rgb.red, 0xFFFF);
    ASSERT_EQ(clut->ct_table[1].rgb.green, 0x0000);
    ASSERT_EQ(clut->ct_table[1].rgb.blue, 0xFFFF);

    ASSERT_EQ(clut->ct_table[2].rgb.red, 0x0000);
    ASSERT_EQ(clut->ct_table[2].rgb.green, 0xFFFF);
    ASSERT_EQ(clut->ct_table[2].rgb.blue, 0x0000);

    qd_color_table_free(clut);
    qd_buffer_free(clut_buffer);
}

#endif