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
#include <string.h>
#include "internal/packbits.h"

int qd_packbits_decode(uint8_t **out_data, uint8_t *packed_data, int length, int value_size)
{
	uint32_t result_size = 0;

	// We need to do an initial pass through the data to determine how much space 
	// needs to be allocated for the unpacked data.
	uint32_t pos = 0;
	while (pos < length) {
		uint8_t count = packed_data[pos++];
		if (count >= 0 && count < 128) {
			uint8_t run = (1 + count) * value_size;
			pos += run;
			result_size += run;
		}
		else if (count >= 128) {
			uint8_t run = 256 - count + 1;
			pos += value_size;
			result_size += (run * value_size);
		}
		else {
			// No-op
		}
	}

	// We now know how many bytes the unpacked data requires.
	uint8_t *data;

	if (out_data && *out_data != NULL) {
		data = *out_data;
	}
	else {
		data = calloc(result_size, sizeof(*data));
		if (out_data) {
			*out_data = data;
		}
	}

	pos = 0;
	uint32_t out_pos = 0;
	while (pos < length) {
		uint8_t count = packed_data[pos++];
		if (count >= 0 && count < 128) {
			uint8_t run = (1 + count) * value_size;
			memmove(data + out_pos, packed_data + pos, run);
			pos += run;
			out_pos += run;
		}
		else if (count >= 128) {
			uint8_t run = 256 - count + 1;
			uint8_t value[value_size];
			memmove(value, packed_data + pos, value_size);
			pos += value_size;
			for (uint8_t i = 0; i < run; ++i) {
				memmove(data + out_pos, value, value_size);
				out_pos += value_size;
			}
		}
		else {
			// No-op
		}
	}

	return result_size;
}
