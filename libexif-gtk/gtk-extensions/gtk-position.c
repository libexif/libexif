/* gtk-position.c
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
#include "gtk-position.h"

#include <gtk/gtksignal.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

struct _GtkPositionPrivate {
};

#define PARENT_TYPE GTK_TYPE_OPTIONS
static GtkOptionsClass *parent_class;

enum {
	TYPE_SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_position_destroy (GtkObject *object)
{
	GtkPosition *pos = GTK_POSITION (object);

	pos = NULL;

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_position_finalize (GtkObject *object)
{
	GtkPosition *pos = GTK_POSITION (object);

	g_free (pos->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_position_class_init (GtkPositionClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_position_destroy;
	object_class->finalize = gtk_position_finalize;

	signals[TYPE_SELECTED] = gtk_signal_new ("type_selected",
		GTK_RUN_LAST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkPositionClass, type_selected),
		gtk_marshal_NONE__UINT, GTK_TYPE_NONE, 1, GTK_TYPE_UINT);
	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_position_init (GtkPosition *pos)
{
	pos->priv = g_new0 (GtkPositionPrivate, 1);
}

GtkType
gtk_position_get_type (void)
{
	static GtkType pos_type = 0;

	if (!pos_type) {
		static const GtkTypeInfo pos_info = {
			"GtkPosition",
			sizeof (GtkPosition),
			sizeof (GtkPositionClass),
			(GtkClassInitFunc)  gtk_position_class_init,
			(GtkObjectInitFunc) gtk_position_init,
			NULL, NULL, NULL};
		pos_type = gtk_type_unique (PARENT_TYPE, &pos_info);
	}

	return (pos_type);
}

static void
on_option_selected (GtkOptions *options, guint option, GtkPosition *pos)
{
	gtk_signal_emit (GTK_OBJECT (pos), signals[TYPE_SELECTED],
			 option);
}

static GtkOptionsList positions[] = {
	{GTK_POS_LEFT,   N_("left")},
	{GTK_POS_RIGHT,  N_("right")},
	{GTK_POS_TOP,    N_("top")},
	{GTK_POS_BOTTOM, N_("bottom")},
	{0, NULL}
};

GtkWidget *
gtk_position_new (void)
{
	GtkPosition *pos;

	pos = gtk_type_new (GTK_TYPE_POSITION);
	gtk_options_construct (GTK_OPTIONS (pos), positions);
	gtk_signal_connect (GTK_OBJECT (pos), "option_selected",
			    GTK_SIGNAL_FUNC (on_option_selected), pos);

	return (GTK_WIDGET (pos));
}
