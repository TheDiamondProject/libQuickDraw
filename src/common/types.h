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

#include <stdint.h>

#if !defined(libQuickDraw_Types)
#define libQuickDraw_Types

/* The purpose of this file is not to provide _all_ of the types that were used
 * in the classic Macintosh Toolbox, as most of them are purely aliases around
 * standard integer types. This includes types such as SInt8, SInt16, etc.
 *
 * One of the goals of libQuickDraw is deobfuscate some of the information hidden
 * behind type aliases. However there will still be some type aliasing. */

/** 
    MacOS Types
 **/
typedef uint32_t                    qd_four_char_code;
typedef qd_four_char_code           qd_os_type;

/** 
    QuickDraw Types
 **/
struct qd_point 
{
    short v;
    short h;
};

struct qd_rect
{
    short top;
    short left;
    short bottom;
    short right;
};

struct qd_fixed_point
{
    double x;
    double y;
};

struct qd_fixed_rect
{
    double left;
    double top;
    double right;
    double bottom;
};

typedef short                       qd_char_parameter;
enum
{
    qd_normal                       = 0x00,
    qd_bold                         = 0x01,
    qd_italic                       = 0x02,
    qd_underline                    = 0x04,
    qd_outline                      = 0x08,
    qd_shadow                       = 0x10,
    qd_condense                     = 0x20,
    qd_extend                       = 0x30,
};
typedef unsigned char               qd_style;
typedef short                       qd_style_parameter;

typedef short                       qd_err;

enum
{
    qd_src_copy                     = 0
};

struct qd_bitmap
{
    int32_t base_addr;
    short row_bytes;
    struct qd_rect bounds;
};

struct qd_rgb_color
{
    unsigned short red;
    unsigned short green;
    unsigned short blue;
};

struct qd_color_spec
{
    unsigned short value;
    struct qd_rgb_color rgb;
};

struct qd_color_table
{
    int32_t ct_seed;
    short ct_flags;
    short ct_size;
    struct qd_color_spec *ct_table;
};

struct qd_pixmap
{
    uint32_t base_addr;
    short row_bytes;
    struct qd_rect bounds;
    short pm_version;
    short pack_type;
    int32_t pack_size;
    double h_res;
    double v_res;
    short pixel_type;
    short pixel_size;
    short cmp_count;
    short cmp_size;
    uint32_t pixel_format;
    uint32_t pm_table;
    uint32_t pm_extension;
};

enum
{
    /* The original 32-bit Color QuickDraw Pixel Formats */
    qd_1_monochrome_pixel_format    = 0x01, /* 1 bit indexed */
    qd_2_indexed_pixel_format       = 0x02, /* 2 bit indexed */
    qd_4_indexed_pixel_format       = 0x04, /* 4 bit indexed */
    qd_8_indexed_pixel_format       = 0x08, /* 8 bit indexed */
    qd_16_555_pixel_format          = 0x10, /* 16 bit Big Endian RGB 555 (Mac) */
    qd_24_rgb_pixel_format          = 0x18, /* 24 bit RGB */
    qd_32_argb_pixel_format         = 0x20, /* 32 bit ARGB (Mac) */
};

struct qd_pattern
{
    uint8_t pat[8];
};

struct qd_pixpat
{
    short pat_type;
    struct qd_pixmap *pat_map;
    uint32_t pat_data;
    uint32_t pat_x_data;
    short pat_x_valid;
    uint32_t pat_x_map;
    uint32_t pat_1_data;
};

#endif
