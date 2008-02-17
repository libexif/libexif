/* exif-byte-order.h
 *
 * Copyright (c) 2002 Lutz Mueller <lutz@users.sourceforge.net>
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __EXIF_BYTE_ORDER_H__
#define __EXIF_BYTE_ORDER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
	EXIF_BYTE_ORDER_MOTOROLA,
	EXIF_BYTE_ORDER_INTEL
} ExifByteOrder;

const char *exif_byte_order_get_name (ExifByteOrder order);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_BYTE_ORDER_H__ */
