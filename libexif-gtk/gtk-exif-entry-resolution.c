/* gtk-exif-entry-resolution.c
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
#include "gtk-exif-entry-resolution.h"

#include <gtk/gtkspinbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkoptionmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtklabel.h>

#include <libexif/exif-i18n.h>

typedef struct _ResolutionObjects ResolutionObjects;
struct _ResolutionObjects
{
	GtkToggleButton *check;
	GtkWidget *sp, *sq;
	GtkAdjustment *ap, *aq;
};

typedef struct _ResolutionUnitObjects ResolutionUnitObjects;
struct _ResolutionUnitObjects
{
	GtkToggleButton *check;
	GtkOptionMenu *menu;
};

struct _GtkExifEntryResolutionPrivate
{
	ExifContent *content;

	GtkToggleButton *check;
	ResolutionObjects ox, oy;
	ResolutionUnitObjects u;

	ExifTag tag_x, tag_y, tag_u;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_resolution_destroy (GtkObject *object)
{
	GtkExifEntryResolution *entry = GTK_EXIF_ENTRY_RESOLUTION (object);

	if (entry->priv->content) {
		exif_content_unref (entry->priv->content);
		entry->priv->content = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_resolution_finalize (GtkObject *object)
{
	GtkExifEntryResolution *entry = GTK_EXIF_ENTRY_RESOLUTION (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_resolution_class_init (GtkExifEntryResolutionClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_resolution_destroy;
	object_class->finalize = gtk_exif_entry_resolution_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_resolution_init (GtkExifEntryResolution *entry)
{
	entry->priv = g_new0 (GtkExifEntryResolutionPrivate, 1);
}

GtkType
gtk_exif_entry_resolution_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryResolution",
			sizeof (GtkExifEntryResolution),
			sizeof (GtkExifEntryResolutionClass),
			(GtkClassInitFunc)  gtk_exif_entry_resolution_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_resolution_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

static void
on_inch_activate (GtkMenuItem *item, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_u);
	g_return_if_fail (e != NULL);
	exif_set_short (e->data, e->order, 2);
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed", e);
}

static void
on_centimeter_activate (GtkMenuItem *item, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_u);
	g_return_if_fail (e != NULL);
	exif_set_short (e->data, e->order, 3);
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed", e);
}

static void
on_wp_value_changed (GtkAdjustment *a, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_x);
	g_return_if_fail (e != NULL);
	exif_set_rational (e->data, e->order, entry->priv->ox.ap->value, 
					      entry->priv->ox.aq->value);
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed", e);
}

static void
on_wq_value_changed (GtkAdjustment *a, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_x);
	g_return_if_fail (e != NULL);
	exif_set_rational (e->data, e->order, entry->priv->ox.ap->value,
					      entry->priv->ox.aq->value);
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed", e);
}

static void
on_hp_value_changed (GtkAdjustment *a, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_y);
	g_return_if_fail (e != NULL);
	exif_set_rational (e->data, e->order, entry->priv->oy.ap->value,
					      entry->priv->oy.aq->value);
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed", e);
}

static void
on_hq_value_changed (GtkAdjustment *a, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_y);
	g_return_if_fail (e != NULL);
	exif_set_rational (e->data, e->order, entry->priv->oy.ap->value,
					      entry->priv->oy.aq->value);
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed", e);
}

static void
gtk_exif_entry_resolution_load_unit (GtkExifEntryResolution *entry,
				     ExifEntry *e)
{
	switch (e->format) {
	case EXIF_FORMAT_SHORT:
		switch (exif_get_short (e->data, e->order)) {
		case 2:
			gtk_option_menu_set_history (entry->priv->u.menu, 1);
			break;
		case 3:
			gtk_option_menu_set_history (entry->priv->u.menu, 0);
			break;
		default:
			g_warning ("Invalid unit!");
		}
		break;
	default:
		g_warning ("Invalid format!");
	}
}

static void
gtk_exif_entry_resolution_load (GtkExifEntryResolution *entry, ExifEntry *e)
{
	ExifRational  r;
	ExifSRational sr;
	ResolutionObjects o;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_RESOLUTION (entry));
	g_return_if_fail (e != NULL);

	switch (e->tag) {
	case EXIF_TAG_X_RESOLUTION:
		o = entry->priv->ox;
		break;
	case EXIF_TAG_Y_RESOLUTION:
		o = entry->priv->oy;
		break;
	default:
		return;
	}

	gtk_signal_handler_block_by_data (GTK_OBJECT (o.ap), entry);
	gtk_signal_handler_block_by_data (GTK_OBJECT (o.aq), entry);
	switch (e->format) {
	case EXIF_FORMAT_RATIONAL:
		r = exif_get_rational (e->data, e->order);
		gtk_adjustment_set_value (o.ap, r.numerator);
		gtk_adjustment_set_value (o.aq, r.denominator);
		break;
	case EXIF_FORMAT_SRATIONAL:
		sr = exif_get_srational (e->data, e->order);
		gtk_adjustment_set_value (o.ap, sr.numerator);
		gtk_adjustment_set_value (o.aq, sr.denominator);
		break;
	default:
		g_warning ("Invalid format!");
		break;
	}
	gtk_signal_handler_unblock_by_data (GTK_OBJECT (o.ap), entry);
	gtk_signal_handler_unblock_by_data (GTK_OBJECT (o.aq), entry);
}

static void
on_cw_toggled (GtkToggleButton *toggle, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_x);
	gtk_widget_set_sensitive (entry->priv->ox.sp, toggle->active);
	gtk_widget_set_sensitive (entry->priv->ox.sq, toggle->active);
	if (toggle->active && !e) { 
		e = exif_entry_new ();
		exif_entry_initialize (e, entry->priv->tag_x);
		exif_content_add_entry (entry->priv->content, e);
		gtk_exif_entry_resolution_load (entry, e);
		exif_entry_unref (e);
		gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_added", e);
	} else if (!toggle->active && e) {
		exif_entry_ref (e);
		exif_content_remove_entry (entry->priv->content, e);
		gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_removed",e);
		exif_entry_unref (e);
	}
}

static void
on_ch_toggled (GtkToggleButton *toggle, GtkExifEntryResolution *entry)
{
        ExifEntry *e;

        e = exif_content_get_entry (entry->priv->content,
                                    entry->priv->tag_y);
	gtk_widget_set_sensitive (entry->priv->oy.sp, toggle->active);
	gtk_widget_set_sensitive (entry->priv->oy.sq, toggle->active);
        if (toggle->active && !e) {
		e = exif_entry_new ();
		exif_entry_initialize (e, entry->priv->tag_y);
		exif_content_add_entry (entry->priv->content, e);
		gtk_exif_entry_resolution_load (entry, e);
		exif_entry_unref (e);
		gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_added", e);
        } else if (!toggle->active && e) {
		exif_entry_ref (e);
		exif_content_remove_entry (entry->priv->content, e);
		gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_removed",e);
		exif_entry_unref (e);
        }
}

static void
on_unit_toggled (GtkToggleButton *toggle, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_u);
	gtk_widget_set_sensitive (GTK_WIDGET (entry->priv->u.menu),
				  toggle->active);
	if (toggle->active && !e) {
		e = exif_entry_new ();
		exif_entry_initialize (e, entry->priv->tag_u);
		exif_content_add_entry (entry->priv->content, e);
		gtk_exif_entry_resolution_load_unit (entry, e);
		exif_entry_unref (e);
		gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_added", e);
	} else if (!toggle->active && e) {
		exif_entry_ref (e);
		exif_content_remove_entry (entry->priv->content, e);
		gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_removed",e);
		exif_entry_unref (e);
	}
}

GtkWidget *
gtk_exif_entry_resolution_new (ExifContent *content, gboolean focal_plane)
{
	GtkExifEntryResolution *entry;
	GtkWidget *hbox, *sp, *sq, *label, *menu, *item, *o, *c;
	GtkObject *ap, *aq;
	ExifEntry *e;

	g_return_val_if_fail (content != NULL, NULL);

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_RESOLUTION);
	entry->priv->content = content;
	exif_content_ref (content);

	if (focal_plane) {
		gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry), 
			_("Focal Plane Resolution"),
			_("The number of pixels on the camera focal plane."));
		entry->priv->tag_x = EXIF_TAG_FOCAL_PLANE_X_RESOLUTION;
		entry->priv->tag_y = EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION;
		entry->priv->tag_u = EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT;
	} else {
		gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
			_("Resolution"),
			_("The number of pixels per unit."));
		entry->priv->tag_x = EXIF_TAG_X_RESOLUTION;
		entry->priv->tag_y = EXIF_TAG_Y_RESOLUTION;
		entry->priv->tag_u = EXIF_TAG_RESOLUTION_UNIT;
	}

	/* Width */
	e = exif_content_get_entry (content, entry->priv->tag_x);
	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	c = gtk_check_button_new_with_label (_("Image width direction"));
	gtk_widget_show (c);
	gtk_box_pack_start (GTK_BOX (hbox), c, FALSE, FALSE, 0);
	entry->priv->ox.check = GTK_TOGGLE_BUTTON (c);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c), (e != NULL));
	gtk_signal_connect (GTK_OBJECT (c), "toggled",
			    GTK_SIGNAL_FUNC (on_cw_toggled), entry);
	ap = gtk_adjustment_new (0, 0, 0xffffffff, 1, 0xffff, 0);
	entry->priv->ox.ap = GTK_ADJUSTMENT (ap);
	sp = gtk_spin_button_new (GTK_ADJUSTMENT (ap), 0, 0);
	gtk_widget_show (sp);
	gtk_box_pack_start (GTK_BOX (hbox), sp, TRUE, TRUE, 0);
	gtk_widget_set_sensitive (sp, (e != NULL));
	entry->priv->ox.sp = sp;
	gtk_signal_connect (ap, "value_changed",
			    GTK_SIGNAL_FUNC (on_wp_value_changed), entry);
	label = gtk_label_new ("/");
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	aq = gtk_adjustment_new (0, 0, 0xffffffff, 1, 0xffff, 0);
	entry->priv->ox.aq = GTK_ADJUSTMENT (aq);
	sq = gtk_spin_button_new (GTK_ADJUSTMENT (aq), 0, 0);
	gtk_widget_show (sq);
	gtk_box_pack_start (GTK_BOX (hbox), sq, TRUE, TRUE, 0);
	gtk_widget_set_sensitive (sq, (e != NULL));
	entry->priv->ox.sq = sq;
	gtk_signal_connect (aq, "value_changed",
			    GTK_SIGNAL_FUNC (on_wq_value_changed), entry);
	if (e)
		gtk_exif_entry_resolution_load (entry, e);

	/* Height */
	e = exif_content_get_entry (content, entry->priv->tag_y);
	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	c = gtk_check_button_new_with_label (_("Image height direction:"));
	gtk_widget_show (c);
	gtk_box_pack_start (GTK_BOX (hbox), c, FALSE, FALSE, 0);
	entry->priv->oy.check = GTK_TOGGLE_BUTTON (c);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c), (e != NULL));
	gtk_signal_connect (GTK_OBJECT (c), "toggled",
			    GTK_SIGNAL_FUNC (on_ch_toggled), entry);
	ap = gtk_adjustment_new (0, 0, 0xffffffff, 1, 0xffff, 0);
	entry->priv->oy.ap = GTK_ADJUSTMENT (ap);
	sp = gtk_spin_button_new (GTK_ADJUSTMENT (ap), 0, 0);
	gtk_widget_show (sp);
	gtk_box_pack_start (GTK_BOX (hbox), sp, TRUE, TRUE, 0);
	entry->priv->oy.sp = sp;
	gtk_widget_set_sensitive (sp, (e != NULL));
	gtk_signal_connect (ap, "value_changed",
			    GTK_SIGNAL_FUNC (on_hp_value_changed), entry);
	label = gtk_label_new ("/");
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	aq = gtk_adjustment_new (0, 0, 0xffffffff, 1, 0xffff, 0);
	entry->priv->oy.aq = GTK_ADJUSTMENT (aq);
	sq = gtk_spin_button_new (GTK_ADJUSTMENT (aq), 0, 0);
	gtk_widget_show (sq);
	gtk_box_pack_start (GTK_BOX (hbox), sq, TRUE, TRUE, 0);
	entry->priv->oy.sq = sq;
	gtk_widget_set_sensitive (sq, (e != NULL));
	gtk_signal_connect (aq, "value_changed",
			    GTK_SIGNAL_FUNC (on_hq_value_changed), entry);
	if (e)
		gtk_exif_entry_resolution_load (entry, e);

	/* Unit */
	e = exif_content_get_entry (content, entry->priv->tag_u);
	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	c = gtk_check_button_new_with_label (_("Unit:"));
	gtk_widget_show (c);
	gtk_box_pack_start (GTK_BOX (hbox), c, FALSE, FALSE, 0);
	entry->priv->check = GTK_TOGGLE_BUTTON (c);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c), (e != NULL));
	gtk_signal_connect (GTK_OBJECT (c), "toggled",
			    GTK_SIGNAL_FUNC (on_unit_toggled), entry);
	o = gtk_option_menu_new ();
	gtk_widget_show (o);
	gtk_box_pack_start (GTK_BOX (hbox), o, TRUE, TRUE, 0);
	entry->priv->u.menu = GTK_OPTION_MENU (o);
	menu = gtk_menu_new ();
	gtk_widget_show (menu);
	item = gtk_menu_item_new_with_label (_("Centimeter"));
	gtk_widget_show (item);
	gtk_menu_append (GTK_MENU (menu), item);
	gtk_signal_connect (GTK_OBJECT (item), "activate",
			    GTK_SIGNAL_FUNC (on_centimeter_activate), entry);
	item = gtk_menu_item_new_with_label (_("Inch"));
	gtk_widget_show (item);
	gtk_menu_append (GTK_MENU (menu), item);
	gtk_signal_connect (GTK_OBJECT (item), "activate",
			    GTK_SIGNAL_FUNC (on_inch_activate), entry);
	gtk_option_menu_set_menu (GTK_OPTION_MENU (o), menu);
	if (e)
		gtk_exif_entry_resolution_load_unit (entry, e);

	return (GTK_WIDGET (entry));
}
