all:
	@echo "wmShutdown version 0.1 - Rafael V. Aroca <rafael@linuxqos.cjb.net>"
	@echo "Order of makes to install: dock, shutdown, install"

clean:
	rm -rf wmShutdown.o  
	rm -rf wmShutdown
	rm -rf Shutdown

install:
	install wmShutdown $(DESTDIR)/usr/bin/wmShutdown
	cp wmShutdown.xpm $(DESTDIR)/usr/share/pixmaps/wmShutdown.xpm
	#install wmShutdown.1 $(DESTDIR)/usr/share/man/man1
	#install Shutdown.1 $(DESTDIR)/usr/share/man/man1
	install -g root -o root Shutdown $(DESTDIR)/usr/bin/Shutdown
	#chmod +s $(DESTDIR)/sbin/shutdown
	#chmod +s $(DESTDIR)/bin/Shutdown
	@echo Installed. Just type wmShutdown to use it

uninstall:
	rm -f /bin/wmShutdown
	rm -f /bin/Shutdown
	rm -f /usr/share/pixmaps/wmShutdown.xpm
	@echo ":-( Uninstalled."

dock: wmShutdown.c
	$(CC) -c -o wmShutdown.o wmShutdown.c `pkg-config --cflags gtk+-2.0` 
	$(CC) -o wmShutdown wmShutdown.o `pkg-config --libs gtk+-2.0` 

shutdown: shutdown.c
	$(CC) -o Shutdown shutdown.c 
