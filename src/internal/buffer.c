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

#include "internal/buffer.h"

struct buffer *buffer_open(const char *restrict path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open a file buffer for '%s'\n", path);
        return NULL;
    }

    fseek(f, 0L, SEEK_END);
    uint64_t size = ftell(f);
    fseek(f, 0L, SEEK_SET);

    void *data = malloc(size);
    if (fread(data, 1, size, f) != size) {
        fprintf(stderr, "Failed to read file buffer for '%s'\n", path);
        free(data);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return buffer_create(data, size);
}

struct buffer *buffer_create(void *data, uint64_t size)
{
    struct buffer *buffer = calloc(1, sizeof(*buffer));
    if (data == NULL) {
        buffer->data = calloc(size, 1);
    } else {
        buffer->data = data;
    }
    buffer->size = size;
    return buffer;
}

struct buffer *buffer_create_empty(uint64_t size)
{
    return buffer_create(NULL, size);
}

void buffer_free(struct buffer *buffer)
{
    if (buffer) {
        free(buffer->data);
        free(buffer);
    }
}

int buffer_eof(struct buffer *restrict stream)
{
    if (!stream) {
        return 0;
    }
    return (stream->pos >= stream->size);
}

void buffer_seek(struct buffer *stream, long offset, int whence)
{
    if (!stream) {
        return;
    }

    switch (whence) {
        case SEEK_SET:
            stream->pos = offset;
            break;
        case SEEK_CUR:
            stream->pos += offset;
            break;
        case SEEK_END:
            stream->pos = stream->size - 1 - offset;
            break;
        default:
            break;
    }
}

long buffer_tell(struct buffer *restrict stream)
{
    return stream ? stream->pos : 0;
}

size_t buffer_read_flags(
    void *restrict ptr, 
    size_t size, 
    size_t nitems,
    int flags,
    struct buffer *restrict stream
) {
    uint8_t *data = stream->data;

    if (!stream) {
        return 0;
    }
    
    size_t count = 0;
    while (!buffer_eof(stream) && nitems--) {
        // Get a representation that we can work with easily.
        uint8_t *p = (uint8_t *)ptr;
        uint8_t *pp = (uint8_t *)ptr;
        for (int len = 0; len < size && stream->pos < stream->size; ++len) {        
            *pp++ = data[stream->pos++];
        }

        // Perform the big endian swap. However this is only done
        // on integer values (2, 3, 4 & 8 bytes).
        if ((flags & f_endian) && ((size >= 2 && size <= 4) || size == 8)) {
            for (int i = 0; i < (size >> 1); ++i) {
                uint8_t tmp = p[size - 1 - i];
                p[size - 1 - i] = p[i];
                p[i] = tmp;
            }
        }   

        // Advance to the next memory location.
        ptr = (void *)((uintptr_t)ptr + size);
        count++;
    }

    // Return the number of items read.
    return count;
}

size_t buffer_read(
    void *restrict ptr, 
    size_t size, 
    size_t nitems,
    struct buffer *restrict stream
) {
    return buffer_read_flags(ptr, size, nitems, f_endian, stream);
}

size_t buffer_read_fixed(
    void *restrict ptr,
    size_t nitems,
    struct buffer *restrict stream
) {
    int32_t *buffer = calloc(nitems, sizeof(*buffer));
    double *fixed = ptr;
    size_t r = buffer_read(buffer, sizeof(*buffer), nitems, stream);
    for (int i = 0; i < nitems; ++i) {
    	fixed[i] = buffer[i] / ((double)(1 << 16));
    }
    return r;
}
