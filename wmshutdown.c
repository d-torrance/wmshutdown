/* wmshutdown - dockapp to shutdown or reboot your machine
 *
 * Copyright 2001, 2002 Rafael V. Aroca <rafael@linuxqos.cjb.net>
 * Copyright 2014 Doug Torrance <dtorrance@monmouthcollege.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gio/gio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

static int showVersion = 0;
GtkWidget *dialog = NULL;

static GOptionEntry entries[] =
{
	{ "version", 'v', 0, G_OPTION_ARG_NONE, &showVersion,
	  "Display version information", NULL },
	{ NULL }
};


/* gtk3 dockapp code based on wmpasman by Brad Jorsch
 * <anomie@users.sourceforge.net>
 * http://sourceforge.net/projects/wmpasman */

GtkWidget *cria_dock(GtkWidget *mw, unsigned int s) {
	GdkDisplay *display;
	GtkWidget *foobox;

	display = gdk_display_get_default();
	foobox = gtk_window_new(GTK_WINDOW_POPUP);

	gtk_window_set_wmclass(GTK_WINDOW(mw), g_get_prgname(), "DockApp");
	gtk_widget_set_size_request(foobox, 47, 47);

	gtk_widget_realize(mw);
	gtk_widget_realize(foobox);

	Display *d = GDK_DISPLAY_XDISPLAY(display);
        Window mainwin = GDK_WINDOW_XID(gtk_widget_get_window(mw));
        Window iw = GDK_WINDOW_XID(gtk_widget_get_window(foobox));
        Window p, dummy1, *dummy2;
        unsigned int dummy3;
        XQueryTree(d, mainwin, &dummy1, &p, &dummy2, &dummy3);
        if (dummy2) XFree(dummy2);
        Window w = XCreateSimpleWindow(d, p, 0, 0, 1, 1, 0, 0, 0);
        XReparentWindow(d, mainwin, w, 0, 0);
        gtk_widget_show(mw);
        gtk_widget_show(foobox);
        XWMHints *wmHints = XGetWMHints(d, mainwin);
        if (!wmHints) {
            wmHints = XAllocWMHints();
        }
        wmHints->flags |= IconWindowHint;
        wmHints->icon_window = iw;
        XSetWMHints(d, mainwin, wmHints);
        XFree(wmHints);
        XReparentWindow(d, mainwin, p, 0, 0);
        XDestroyWindow(d, w);

	return foobox;
}

void fecha(void) {
	gtk_widget_destroy(dialog);
	dialog = NULL;
}

void handle_click(GtkWidget *widget, gpointer data) {
	GDBusConnection *connection;
	GDBusMessage *message, *reply;
	GError *error = NULL;

	gchar *method = (gchar *)data;

	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	message = g_dbus_message_new_method_call(
			NULL,
			"/org/freedesktop/login1",
			"org.freedesktop.login1.Manager",
			method);
	g_dbus_message_set_body(message, g_variant_new("(b)", TRUE));
	gchar *status = g_dbus_message_print(message, 0);
	g_printerr("sending following message:\n%s", status);
	g_free(status);

	g_dbus_message_set_destination(message, "org.freedesktop.login1");

	reply = g_dbus_connection_send_message_with_reply_sync(
		connection, message, 0, -1, NULL, NULL, &error);

	status = g_dbus_message_print(reply, 0);
	g_printerr("got response:\n%s", status);
	g_free(status);


	g_object_unref(message);
	g_object_unref(connection);
	gtk_main_quit();
}

void button_press(GtkWidget *widget, GdkEvent *event) {
	GtkWidget *label;
	gchar *message;
	GtkWidget *halt_button;
	GtkWidget *reboot_button;
	GtkWidget *cancel_button;

	GdkEventButton  *bevent = (GdkEventButton *)event;
	switch (bevent->button) {
	case 1:
		if (dialog != NULL)
			return;
		message = "Shutdown confirmation";
		dialog = gtk_dialog_new();
		label = gtk_label_new(message);

		halt_button = gtk_button_new_with_label("Halt");
		reboot_button = gtk_button_new_with_label("Reboot");
		cancel_button = gtk_button_new_with_label("Cancel");

		g_signal_connect(dialog, "destroy", G_CALLBACK(fecha), NULL);
		g_signal_connect(cancel_button,
				 "clicked",
				 G_CALLBACK(fecha),
				 (gpointer) dialog);
		g_signal_connect(halt_button,
				 "clicked",
				 G_CALLBACK(handle_click),
				 "PowerOff");
		g_signal_connect(reboot_button,
				 "clicked",
				 G_CALLBACK(handle_click),
				 "Reboot");
		gtk_container_add(GTK_CONTAINER(gtk_dialog_get_action_area(
							GTK_DIALOG(dialog))),
				  halt_button);
		gtk_container_add(GTK_CONTAINER(gtk_dialog_get_action_area(
							GTK_DIALOG(dialog))),
				  reboot_button);
		gtk_container_add(GTK_CONTAINER(gtk_dialog_get_action_area(
							GTK_DIALOG(dialog))),
				  cancel_button);
		gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(
							GTK_DIALOG(dialog))),
				  label);
		gtk_widget_show_all(dialog);
	}
}

int main(int argc, char *argv[]) {
	GError *error = NULL;
	GOptionContext *context;
	GtkWidget *gtkiw;
	GtkWidget *dockArea;
	GtkWidget *pixmap;


	gtk_init(&argc, &argv);


	context = g_option_context_new ("- dockapp to shutdown or reboot your "
					"machine");
	g_option_context_add_main_entries (context, entries, NULL);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	g_option_context_parse (context, &argc, &argv, &error);

	if (showVersion) {
		printf("wmforecast "VERSION"\n");
		return 0;
	}


	gtkiw = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	dockArea = cria_dock(gtkiw, 47);


	pixmap = gtk_image_new_from_file(DATADIR"/wmshutdown.xpm");
        gtk_widget_show(pixmap);
        gtk_container_add(GTK_CONTAINER(dockArea), pixmap);

	gtk_widget_add_events(dockArea, GDK_BUTTON_PRESS_MASK);
	g_signal_connect(dockArea, "button-press-event",
			 G_CALLBACK(button_press), NULL);

	gtk_main();

    	return(0);
}
