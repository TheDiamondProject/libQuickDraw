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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include "common/types.h"

#if !defined(libQuickDraw_Buffer)
#define libQuickDraw_Buffer

enum
{
    f_none = 0x00,
    f_endian = 0x01,
};

struct buffer
{
    void *data;
    uint64_t pos;
    uint64_t size;
};

struct buffer *buffer_open(const char *restrict path);
struct buffer *buffer_create(void *data, uint64_t size);
struct buffer *buffer_create_empty(uint64_t size);
void buffer_free(struct buffer *buffer);

int buffer_eof(struct buffer *restrict stream);
void buffer_seek(struct buffer *stream, long offset, int whence);
long buffer_tell(struct buffer *restrict stream);

size_t buffer_read_flags(
    void *restrict ptr, 
    size_t size, 
    size_t nitems,
    int flags,
    struct buffer *restrict stream
);

size_t buffer_read(
    void *restrict ptr, 
    size_t size, 
    size_t nitems,
    struct buffer *restrict stream
);

size_t buffer_read_fixed(
    void *restrict ptr,
    size_t nitems,
    struct buffer *restrict stream
);

#endif
