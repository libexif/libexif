#include <config.h>

#include <stdio.h>

#include <gtk/gtkdialog.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkscrolledwindow.h>

#include <libjpeg/jpeg-data.h>
#include <libexif/exif-data.h>
#include <libexif-gtk/gtk-exif-browser.h>

int
main (int argc, char **argv)
{
	GtkWidget *dialog, *button, *browser;
	JPEGData *jdata;
	ExifData *edata;

	if (argc <= 1) {
		printf ("You need to specify a file!\n");
		return (1);
	}

	g_log_set_always_fatal (G_LOG_LEVEL_CRITICAL);
	gtk_init (&argc, &argv);

	jdata = jpeg_data_new_from_file (argv[1]);
	if (!jdata) {
		printf ("Could not read '%s'!\n", argv[1]);
		return (1);
	}
	edata = jpeg_data_get_exif_data (jdata);
	jpeg_data_unref (jdata);
	if (!edata) {
		printf ("Could not find EXIF information in '%s'!\n", argv[1]);
		return (1);
	}

	dialog = gtk_dialog_new ();
	gtk_widget_show (dialog);
	gtk_signal_connect (GTK_OBJECT (dialog), "delete_event",
			    GTK_SIGNAL_FUNC (gtk_main_quit), dialog);
	gtk_widget_set_usize (dialog, 200, 300);

	browser = gtk_exif_browser_new ();
	gtk_widget_show (browser);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), browser);
	gtk_exif_browser_set_data (GTK_EXIF_BROWSER (browser), edata);
	exif_data_unref (edata);

	button = gtk_button_new_with_label ("Ok");
	gtk_widget_show (button);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->action_area),
			   button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (gtk_main_quit), dialog);

	gtk_main ();

	return (0);
}
