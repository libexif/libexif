/** \file test-integers.c
 * \brief Check assumptions about integer types (sizes, ranges).
 *
 * Copyright (C) 2007 Hans Ulrich Niedermann <gp@n-dimensional.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA.
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */


#include "libexif/_stdint.h"
#include <assert.h>
#include "libexif/exif-system.h"

typedef enum {
   EN_A,
   EN_B,
   EN_C,
   EN_D,
   EN_E,
   EN_F
} enum_t;

int main(void)
{
  size_t r;

  /* libexif assumes unsigned ints are not smaller than 32bit in many places */
  assert(sizeof(unsigned int) >= sizeof(uint32_t));

  /* libexif assumes that enums fit into ints */
  assert(sizeof(enum_t) <= sizeof(int));

  /* Range check used by the IFD and MakerNote loaders */
  assert(!exif_check_overflow(0, 10, 10));
  assert(!exif_check_overflow(6, 10, 4));
  assert(!exif_check_overflow(9, 10, 0));
  assert(exif_check_overflow(7, 10, 4));
  assert(exif_check_overflow(10, 10, 0));
  assert(exif_check_overflow(0, 10, 11));
  assert(exif_check_overflow(0, 0, 0));
  assert(exif_check_overflow((size_t)-1, 10, 2));
  assert(exif_check_overflow(2, 10, (size_t)-1));
  assert(exif_check_overflow(8, (size_t)-1, (size_t)-1 - 7));

  /* Size multiplication used by the MakerNote loaders */
  r = 1234;
  assert(!exif_check_mul_overflow(0, 99, 10, &r) && (r == 0));
  assert(!exif_check_mul_overflow(2, 5, 10, &r) && (r == 10));
  assert(!exif_check_mul_overflow(8, 0x1fffffff, 0xffffffff, &r) && (r == 0xfffffff8));
  r = 1234;
  assert(exif_check_mul_overflow(3, 4, 10, &r) && (r == 1234));
  assert(exif_check_mul_overflow(8, 0x20000000, 0xffffffff, &r) && (r == 1234));
  assert(exif_check_mul_overflow((size_t)-1, 2, (size_t)-1, &r) && (r == 1234));
  
  return 0;
}
