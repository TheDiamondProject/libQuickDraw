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
#include "pict/pict.h"
#include "common/color_table.h"
#include "common/pixmap.h"
#include "common/geometry.h"
#include "internal/packbits.h"

// MARK: - PICT Constants

#define PICT_V2_MAGIC 				0x001102ff
#define PACK_BITS_THRESHOLD         4

// MARK: - PICT Opcodes

enum qd_pict_opcode
{
	qd_pict_opcode_nop              = 0x0000,
	qd_pict_opcode_clip_region      = 0x0001,
	qd_pict_opcode_direct_bits_rect = 0x009A,
	qd_pict_opcode_eof              = 0x00FF,
	qd_pict_opcode_def_hilite       = 0x001E,
	qd_pict_opcode_long_comment     = 0x00A1,
	qd_pict_opcode_ext_header       = 0x0C00,
};

// MARK: - Picture Parser

static inline int qd_pict_read_pict_rect(struct qd_rect *rect, struct qd_buffer *restrict buffer)
{
	// a "PICT rect" is encoded differently. It is (x, y, width, height). Read these values and
	// encode to a standard qd_rect (top, left, bottom, right).
	int16_t values[4];
	if (qd_buffer_read(values, sizeof(int16_t), 4, buffer) != 4) {
		fprintf(stderr, "Failed to read PICT rect from PICT.\n");
		return 1;
	}

	rect->top = values[0];
	rect->bottom = values[0] + values[2];
	rect->left = values[1];
	rect->right = values[1] + values[3];

	return 0;
}

static inline int qd_read_opcode(uint16_t *opcode, struct qd_buffer *restrict buffer)
{
	long pos = qd_buffer_tell(buffer);
	pos += pos % sizeof(uint16_t);
	qd_buffer_seek(buffer, pos, SEEK_SET);

	if (qd_buffer_read(opcode, sizeof(uint16_t), 1, buffer) != 1) {
		fprintf(stderr, "Failed to read PICT opcode, returning EOF..\n");
		return 1;
	}

	return 0;
}

static inline int qd_pict_read_region(struct qd_pict *pict, struct qd_rect *rect, struct qd_buffer *restrict buffer)
{
	uint16_t size = 0;
	if (qd_buffer_read(&size, sizeof(uint16_t), 1, buffer) != 1) {
		fprintf(stderr, "Failed to read the size of a clip region in PICT.\n");
		return 1;
	}

	if (qd_buffer_read(rect, sizeof(short), 4, buffer) != 4) {
		fprintf(stderr, "Failed to read the clip region rect in PICT.\n");
		return 1;
	}

	rect->left /= pict->x_ratio;
	rect->right /= pict->x_ratio;
	rect->top /= pict->y_ratio;
	rect->bottom /= pict->y_ratio;

	uint32_t points = (size - 10) >> 2;
	qd_buffer_seek(buffer, sizeof(uint16_t) * 2 * points, SEEK_CUR);

	return 0;
}

static inline int qd_pict_read_long_comment(struct qd_buffer *restrict buffer)
{
	qd_buffer_seek(buffer, 2, SEEK_CUR);

	int16_t length = 0;
	if (qd_buffer_read(&length, sizeof(int16_t), 1, buffer) != 1) {
		fprintf(stderr, "Failed to read long comment length from PICT.\n");
		return 1;
	}

	qd_buffer_seek(buffer, (long)length, SEEK_CUR);

	return 0;
}

static inline int qd_pict_read_direct_bits_rect(struct qd_pict *pict, struct qd_buffer *restrict buffer)
{
	uint8_t tmp8 = 0;

	// Read the PixMap for the opcode. This defines information about the pixel
	// data represented.
	struct qd_pixmap *pm = NULL;
	if (qd_pixmap_parse(&pm, buffer)) {
		fprintf(stderr, "Failed to read PixMap structure from PICT.\n");
		return 1;
	}
	pict->pm = pm;

	struct qd_rect source_rect = { 0 };
	struct qd_rect destination_rect = { 0 };

	if (qd_pict_read_pict_rect(&source_rect, buffer) || qd_pict_read_pict_rect(&destination_rect, buffer)) {
		// Abort if failed to read either rect!
		return 1;
	}

	qd_buffer_seek(buffer, 2L, SEEK_CUR);

	// Verify the type of PixMap. We can only accept certain types for the time being
	// until support for decoding/rendering other types is added.
	if (pm->pack_type != 3 && pm->pack_type != 4) {
		fprintf(stderr, "Unsupported PixMap pack type (%d) encountered in PICT.\n", pm->pack_type);
		return 1;
	}

	// We're going to allocate memory privately, and not as part of the main PICT structure.
	uint8_t *raw = NULL;
	uint32_t raw_size = 0;
	void *px_buffer = NULL;

	if (pm->pack_type == 3) {
		raw_size = pm->row_bytes * sizeof(*raw);
		px_buffer = calloc((qd_rect_get_height(source_rect) * (pm->row_bytes + 1)) >> 1, sizeof(uint16_t));
	}
	else if (pm->pack_type == 4) {
		raw_size = ((pm->cmp_count * pm->row_bytes) >> 2) * sizeof(*raw);
		px_buffer = calloc((qd_rect_get_height(source_rect) * (pm->row_bytes + 3)) >> 1, sizeof(uint32_t));
	}
	raw = calloc(raw_size, 1);

	uint32_t px_buffer_offset = 0;
	uint16_t packed_bytes_count = 0;
	uint32_t height = qd_rect_get_height(source_rect);
	uint32_t width = qd_rect_get_width(source_rect);
	uint32_t bounds_width = qd_rect_get_width(pm->bounds);

	for (uint32_t scanline = 0; scanline < height; ++scanline) {
		if (pm->row_bytes <= PACK_BITS_THRESHOLD) {
			// No pack bits compression.
			if (qd_buffer_read(raw, 1, pm->row_bytes, buffer) != pm->row_bytes) {
				fprintf(stderr, "Failed to read pixel pattern data from PICT buffer (1).\n");
				goto ERROR;
			}
		}
		else  {
			if (pm->row_bytes > 250) {
				// Pack bits compression is in place, with the length encoded as a short.
				if (qd_buffer_read(&packed_bytes_count, sizeof(uint16_t), 1, buffer) != 1) {
					fprintf(stderr, "Failed to read the number of packed bytes in PICT buffer.\n");
					goto ERROR;
				}
			}
			else {
				// Pack bits compression is in place, with the length encoded as a byte.
				if (qd_buffer_read(&tmp8, sizeof(uint8_t), 1, buffer) != 1) {
					fprintf(stderr, "Failed to read the number of packed bytes in PICT buffer.\n");
					goto ERROR;
				}
				packed_bytes_count = (uint16_t)tmp8;
			}
			
			// Create a temporary buffer to read the packed data into on the stack. Avoid allocation
			// in a loop!
			uint8_t packed_data[packed_bytes_count];
			if (qd_buffer_read(packed_data, 1, packed_bytes_count, buffer) != packed_bytes_count) {
				fprintf(stderr, "Failed to read pixel pattern data from PICT buffer (2).\n");
				goto ERROR;
			}

			if (pm->pack_type == 3) {
				qd_packbits_decode(&raw, packed_data, packed_bytes_count, sizeof(uint16_t));
			}
			else if (pm->pack_type == 4) {
				qd_packbits_decode(&raw, packed_data, packed_bytes_count, sizeof(uint8_t));	
			}
			
		}

		if (pm->pack_type == 3) {
			for (uint32_t x = 0; x < width; ++x) {
                ((uint16_t *)px_buffer)[px_buffer_offset + x] = (uint16_t)(((0xFF & raw[2*x]) << 8) | (0xFF & raw[2*x+1]));
            }
		}
		else {
			if (pm->cmp_count == 3) {
				// RGB Formatted Data
				for (uint32_t x = 0; x < width; ++x) {
					((uint16_t *)px_buffer)[px_buffer_offset + x] = 0xFF000000
						| ((raw[x] & 0xFF) << 16)
						| ((raw[bounds_width + x] & 0xFF) << 8)
						| (raw[2 * bounds_width + x] & 0xFF);
				}
			}
			else {
				// ARGB Formatted Data
				for (uint32_t x = 0; x < width; ++x) {
					((uint16_t *)px_buffer)[px_buffer_offset + x] =
					  ((raw[x] & 0xFF) << 24)
					| ((raw[bounds_width] & 0xFF) << 16)
					| ((raw[2 * bounds_width + x] & 0xFF) << 8)
					| (raw[3 * bounds_width + x] & 0xFF);
				}
			}
		}
		
		px_buffer_offset += width;
	}

	uint32_t source_length = width * height;
	uint32_t rgb_length = source_length * 4;
	uint8_t *rgb = calloc(rgb_length, sizeof(*rgb));

	if (pm->pack_type == 3) {
		for (uint32_t p = 0, i = 0; i < source_length; ++i) {
            rgb[p++] = ((((uint16_t *)px_buffer)[i] & 0x001f) << 3);
            rgb[p++] = ((((uint16_t *)px_buffer)[i] & 0x03e0) >> 5) << 3;
            rgb[p++] = ((((uint16_t *)px_buffer)[i] & 0x7c00) >> 10) << 3;
            rgb[p++] = UINT8_MAX;
        }
	}
	else if (pm->pack_type == 4) {
		for (uint32_t p = 0, i = 0; i < source_length; ++i) {
            rgb[p++] = (((uint32_t *)px_buffer)[i] & 0xFF0000) >> 16;
            rgb[p++] = (((uint32_t *)px_buffer)[i] & 0xFF00) >> 8;
            rgb[p++] = (((uint32_t *)px_buffer)[i] & 0xFF);
            rgb[p++] = (((uint32_t *)px_buffer)[i] & 0xFF000000) >> 24;
        }
	}

	// Copy the data into the PICT surface and make sure everything is assigned 
	// correctly so that the image data can be used.
	// TODO: This should be improved so that we actually draw into the pict surface,
	// not just set the surface to this.
	pict->size = rgb_length;
	pict->surface = rgb;

	free(raw);
	free(px_buffer);
	return 0;

ERROR:
	free(raw);
	free(px_buffer);
	return 1;
}

int qd_pict_parse(struct qd_pict **out_pict, struct qd_buffer *restrict buffer)
{
	uint16_t tmp16 = 0;
	uint32_t tmp32 = 0;
	struct qd_rect clip_rect = { 0 };

	struct qd_pict *pict = calloc(1, sizeof(*pict));
	if (out_pict) {
		*out_pict = pict;
	}

	qd_buffer_seek(buffer, 2L, SEEK_SET);

	if (qd_buffer_read(&pict->frame, sizeof(int16_t), 4, buffer) != 4) {
		fprintf(stderr, "Failed to read PICT frame.\n");
		goto ERROR;
	}

	// For now we're looking for Version 2 PICTs. Version 1 PICTs will come later on.
	if (qd_buffer_read(&tmp32, sizeof(uint32_t), 1, buffer) != 1 && tmp32 != PICT_V2_MAGIC) {
		fprintf(stderr, "Failed to read PICT Magic Number, or unexpected value encountered.\n");
		goto ERROR;
	}

	// The very first thing we should find is an extended header opcode. Read this
	// outside of the main opcode loop as it should only appear once, and at the beginning.
	if (qd_read_opcode(&tmp16, buffer) || tmp16 != qd_pict_opcode_ext_header) {
		fprintf(stderr, "Expected to find Extended PICT Header, but did not.\n");
		goto ERROR;
	}

	if (qd_buffer_read(&tmp32, sizeof(uint32_t), 1, buffer) && ((tmp32 >> 16) != 0xFFFE)) {
		// Standard Header Variant
		struct qd_fixed_rect rect = { 0 };
		if (qd_buffer_read_fixed(&rect, 4, buffer) != 4) {
			fprintf(stderr, "Failed to read fixed point rect from PICT standard header.\n");
			goto ERROR;
		}

		pict->x_ratio = qd_rect_get_width(pict->frame) / qd_fixed_rect_get_width(rect);
		pict->y_ratio = qd_rect_get_height(pict->frame) / qd_fixed_rect_get_height(rect);
	}
	else {
		// Extended Header Variant
		qd_buffer_seek(buffer, sizeof(uint32_t) * 2, SEEK_CUR);

		struct qd_rect rect = { 0 };
		if (qd_buffer_read(&rect, sizeof(int16_t), 4, buffer) != 4) {
			fprintf(stderr, "Failed to read rect from PICT extended header.\n");
			goto ERROR;
		}

		pict->x_ratio = qd_rect_get_width(pict->frame) / qd_rect_get_width(rect);
		pict->y_ratio = qd_rect_get_height(pict->frame) / qd_rect_get_height(rect);
	}

	if (pict->x_ratio <= 0 || pict->y_ratio <= 0) {
		fprintf(stderr, "Unrecognised PICT resource. Content ratio is not valid.\n");
		goto ERROR;
	}

	qd_buffer_seek(buffer, 4, SEEK_CUR);

	// Begin parsing the PICT opcodes
	while ( qd_buffer_eof(buffer) == 0) {
		uint16_t opcode = 0;
		if (qd_read_opcode(&opcode, buffer)) {
			fprintf(stderr, "Failed to read opcode from PICT.\n");
			return 1;
		}

		if (opcode == qd_pict_opcode_eof) {
			break;
		}

		switch (opcode) {
			case qd_pict_opcode_clip_region:
				if (qd_pict_read_region(pict, &clip_rect, buffer)) {
					return 1;
				}
				break;

			case qd_pict_opcode_direct_bits_rect:
				if (qd_pict_read_direct_bits_rect(pict, buffer)) {
					return 1;
				}
				break;

			case qd_pict_opcode_long_comment:
				if (qd_pict_read_long_comment(buffer)) {
					return 1;
				}
				break;

			case qd_pict_opcode_nop:
			case qd_pict_opcode_eof:
			case qd_pict_opcode_ext_header:
			case qd_pict_opcode_def_hilite:
				break;

			default:
				fprintf(stderr, "Unrecognised PICT opcode '%04x' encountered.\n", opcode);
				return 1;
		}
	}

	// Reaching this point is indicative that we have successfully parsed
	// the PICT.
	return 0;

ERROR:
	qd_pict_free(pict);
	return 1;
}

void qd_pict_free(struct qd_pict *p)
{
	if (p) {
		free(p->surface);
		free(p);
	}
}
