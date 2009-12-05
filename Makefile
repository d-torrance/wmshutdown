all:
	@echo "wmShutdown version 0.1 - Rafael V. Aroca <rafael@linuxqos.cjb.net>"
	@echo "Order of makes to install: dock, shutdown, install"

clean:
	rm -f wmShutdown.o  
	rm -f wmShutdown
	rm -f Shutdown

install:
	install wmShutdown /usr/local/bin
	install wmShutdown.xpm /usr/share/pixmaps/
	install -g root -o root Shutdown /usr/local/bin
	chmod +s /sbin/shutdown
	chmod +s /usr/local/bin/Shutdown
	@echo Installed. Just type wmShutdown to use it

uninstall:
	rm -f /usr/local/bin/wmShutdown
	rm -f /usr/local/bin/Shutdown
	rm -f /usr/share/pixmaps/wmShutdown.xpm
	@echo ":-( Uninstalled."

dock: wmShutdown.c
	$(CC) -c -o wmShutdown.o wmShutdown.c `gtk-config --cflags` 
	$(CC) -o wmShutdown wmShutdown.o `gtk-config --libs` 

shutdown: shutdown.c
	$(CC) -o Shutdown shutdown.c 
