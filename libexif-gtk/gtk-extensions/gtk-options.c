/* gtk-options.c
 *
 * Copyright (C) 2001 Lutz Müller <lutz@users.sourceforge.net>
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

#include <config.h>
#include "gtk-options.h"

#include <string.h>

void
gtk_options_sort (GtkOptions *options)
{
	GtkOptions entry;
	guint i = 0;

	while (options[i+1].name) {
		if (strcmp (options[i].name, options[i + 1].name) > 0) {
			entry = options[i];
			options[i] = options[i + 1];
			options[i + 1] = entry;
			if (i)
				i--;
		} else
			i++;
	}
}

