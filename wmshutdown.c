/*
 * wmshutdown.c
 *
 * (C) 2001 Rafael V. Aroca <rafael@linuxqos.cjb.net>
 *
 * This software is under GPL
 *
 * Based on foodock lib by Alexey Vyskubov <alexey@pepper.spb.ru>
 *
 */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gio/gio.h>

GtkWidget *dialog = NULL;

/* gtk3 dockapp code based on wmpasman by Brad Jorsch
 * <anomie@users.sourceforge.net>
 * http://sourceforge.net/projects/wmpasman/
 */
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

int fecha(void) {
	gtk_widget_destroy(dialog);
	dialog = NULL;
}

void handle_click(GtkWidget *widget, gpointer data) {
	GDBusConnection *connection;
	GDBusMessage *message;
	GError *error = NULL;

	gchar *method = (gchar *)data;

	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	message = g_dbus_message_new_method_call(
			NULL,
			"/org/freedesktop/ConsoleKit/Manager",
			"org.freedesktop.ConsoleKit.Manager",
			method);
	g_dbus_message_set_destination(message, "org.freedesktop.ConsoleKit");

	g_dbus_connection_send_message_with_reply_sync(
		connection, message, 0, -1, NULL, NULL, &error);

	g_object_unref(message);
	g_object_unref(connection);
	gtk_main_quit();
}

int button_press(GtkWidget *widget, GdkEvent *event) {
	GtkWidget *label;
	gchar *message;
	GtkWidget *halt_button;
	GtkWidget *reboot_button;
	GtkWidget *cancel_button;

	GdkEventButton  *bevent = (GdkEventButton *)event;
	switch (bevent->button) {
	case 1:
		if (dialog != NULL)
			return 1;
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
				 "Stop");
		g_signal_connect(reboot_button,
				 "clicked",
				 G_CALLBACK(handle_click),
				 "Restart");
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
	GdkDisplay *display;
	GtkWidget *gtkiw;
	GtkWidget *dockArea;
	GtkWidget *pixmap;

	gtk_init(&argc, &argv);

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
