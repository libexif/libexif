/* actions.h
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
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA.
 */

#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include <stdio.h>
#include "exif-data.h"

void exif_action_dump_json (ExifData *ed, FILE* fp);
void exif_action_dump_xml     (ExifData *ed, FILE* fp);
void exif_action_dump_json (ExifData *ed, FILE* p);

#endif /* __ACTIONS_H__ */
