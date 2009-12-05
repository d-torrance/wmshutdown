/*
 * wmShutdown.c
 *
 * (C) 2001 Rafael V. Aroca <rafael@linuxqos.cjb.net> 
 *
 * This software is under GPL
 *
 * Based on foodock lib by Alexey Vyskubov <alexey@pepper.spb.ru>
 *
 */

#include "wmShutdown.h"

GtkWidget *dialog = NULL;

GtkWidget *cria_dock(GtkWidget *mw, unsigned int s, int margc, char *margv[]) {

	GtkWidget *foobox; 
	Window xmw;
	XWMHints *wm_hints;

	xmw = GDK_WINDOW_XWINDOW(mw->window);

	foobox = gtk_event_box_new();
	gtk_widget_set_usize(foobox, s, s);
	gtk_container_add (GTK_CONTAINER (mw), foobox);
	gtk_widget_realize(foobox);

	wm_hints = XAllocWMHints();
	wm_hints->window_group = xmw;
	wm_hints->icon_window = GDK_WINDOW_XWINDOW(foobox->window);
	wm_hints->icon_x = 0;
	wm_hints->icon_y = 0;
	wm_hints->initial_state = WithdrawnState;
	wm_hints->flags = StateHint |
		IconPositionHint |
        	WindowGroupHint |
        	IconWindowHint;

	XSetWMHints(GDK_DISPLAY(), xmw, wm_hints);
	XSetCommand(GDK_DISPLAY(), xmw, margv, margc);

	XFree(wm_hints);

	return foobox;
}


int fecha(void) {
	//gtk_exit(0);
	gtk_widget_destroy(dialog);
	dialog = NULL;
	//close();	
}

int desliga(void) {
	FILE *output;
	char *teste;

	output = popen("/usr/local/bin/Shutdown -h", "r");
	teste = (char*) fgetc(output);
	while ((int)teste != EOF) {
		g_print("%c", teste);
		teste = (char*) fgetc(output);
	}
	pclose(output);
}

int reinicia(void) {
	FILE *output;
	char *teste;

	output = popen("/usr/local/bin/Shutdown -r", "r");
	teste = (char*) fgetc(output);
	while ((int)teste != EOF) {
		g_print("%c", teste);
		teste = (char*) fgetc(output);
	}
	pclose(output);
}


int button_press(GtkWidget *widget, GdkEvent *event) {

	GtkWidget *label;
	gchar *message;
	GtkWidget *halt_button;
	GtkWidget *reboot_button;
	GtkWidget *cancel_button;

	//Descomentar para debug
	//g_print("Evento ocorreu");

	if (event->type == GDK_BUTTON_PRESS) {
		GdkEventButton  *bevent = (GdkEventButton *)event;
		
		switch (bevent->button) {
			case 1:

				if (dialog != NULL)
			        	return;
				message = "Shutdown confirmation";
				dialog = gtk_dialog_new();
		    		label = gtk_label_new (message);
	    
			    	halt_button = gtk_button_new_with_label("Halt");
			    	reboot_button = gtk_button_new_with_label("Reboot");
			    	cancel_button = gtk_button_new_with_label("Cancel");

				gtk_signal_connect(GTK_OBJECT(dialog), "destroy", GTK_SIGNAL_FUNC(fecha), NULL);
				
				gtk_signal_connect_object (GTK_OBJECT (cancel_button), "clicked", GTK_SIGNAL_FUNC (fecha), (gpointer) dialog);
 
				gtk_signal_connect_object (GTK_OBJECT (halt_button), "clicked", GTK_SIGNAL_FUNC (desliga), (gpointer) dialog);
		
				gtk_signal_connect_object (GTK_OBJECT (reboot_button), "clicked", GTK_SIGNAL_FUNC (reinicia), (gpointer) dialog);

	    			gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area), halt_button);
	    			gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area), reboot_button);
	    			gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area), cancel_button);

	    			gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);

	    			gtk_widget_show_all (dialog);
		}
	}
}


int main(int argc, char *argv[]) {

	GtkWidget *gtkiw;
	GtkWidget *dockArea;
	GtkWidget *button;
	GtkWidget *box1;
	GtkWidget *icon;
	GtkWidget *pixmap;
	GdkBitmap *mask;
	    
	gtk_init(&argc, &argv);
	gtk_set_locale();

	gtkiw = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (gtkiw), "ShutDockApp");
	gtk_widget_realize(gtkiw);
	    
	dockArea = cria_dock(gtkiw, 47, argc, argv);
	gtk_widget_realize(dockArea);	    

        icon = (gpointer) gdk_pixmap_create_from_xpm (gtkiw->window, &mask,
                	NULL, "/usr/share/pixmaps/wmShutdown.xpm");

        pixmap = gtk_pixmap_new((gpointer) icon, mask);
        gtk_widget_show(pixmap);
        gtk_container_add(GTK_CONTAINER(dockArea), pixmap);

        gtk_signal_connect(GTK_OBJECT(gtkiw),
                        "event",
                        GTK_SIGNAL_FUNC(button_press),
                        NULL);

        gtk_widget_show(dockArea);
        gtk_widget_show(gtkiw);

    	gtk_main ();
     
    	return(0);
}


