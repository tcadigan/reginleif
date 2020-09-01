case $CONFIG in
    '')
	if test ! -f config.sh; then
	    ln ../config.sh . || \
		ln ../../config.sh . || \
		ln ../../../config.sh . || \
		(echo "Can't find config.sh."; exit 1)
	fi

	. ./config.sh
	;;
esac

case "$0" in
    */*)
	cd `expr X$0 : 'X\(.*\)/'`
	;;
esac

case "$d_symlink" in
    *define*)
	sln='ln -s'
	;;
    *)
	sln='ln'
	;;
esac

echo "Extracting Makefile (with variable substitutions)"
cat >Makefile <<!GROK!THIS!
#
# Top level Makefile for the GB+ program
#
# Bug reports, patches, comments, sugesstions should be sent to:
#   John Deregon deregon@jetrho.cs.nyu.edu
#
# Revision 1.5 18:08:02 jpd
#
# Variables established by Configure
CAT = $cat
GREP = $grep
ECHO = $echo
MAKE = $make
PR = $pr
SHAR = $shar -s 60000
SLEEP = $sleep
TAR = $tar
HOST = $host
PORT = $port
ARCHIVE = GB+6.0b.tar

!GROK!THIS!

cat >>Makefile <<'!NO!SUBS!'
# Other variables
SHELL = /bin/sh
SOURCE_FILES = `$(GREP) -v 'doc/' MANIFEST`
DISTRIBUTED_FILES = `$(CAT) MANIFEST`
VERSION = `$(CAT) VERSION`
PACKAGE = gb-$(VERSION)/Configure gb-$(VERSION)/docs gb-$(VERSION)/VERSION \
	  gb-$(VERSION)/MANIFEST gb-$(VERSION)/Makefile.sh \
	  gb-$(VERSION)/*.*.c gb-$(VERSION)/*/*.h gb-$(VERSION)/*/*.sh \
	  gb-$(VERSION)/scripts gb-$(VERSION)/misc

# Targets
#   Give default target first and alone
default_target: all

#   Targets that are simply executed in each subordinate makefile as is
all:
	@echo "Making in library in user"
	cd user; $(MAKE) -$(MAKEFLAGS) $@

	@echo "Making in server"
	cd server; $(MAKE) -$(MAKEFLAGS) $@

	@echo "Making in utils"
	cd utils; $(MAKE) -$(MAKEFLAGS) $@

	echo "Indexing help files"
	cd help; $(MAKE) -$(MAKEFLAGS) $@

	@echo "Now do a \"make install\""

shar:
	$(SHAR) $(DISTRIBUTED_FILES)

tar:
	chmod +x Configure
	$(TAR) -cvf $(ARCHIVE) $(DISTRIBUTED_FILES)

distrib:
	ln -s . gb-$(VERSION)
	tar -cf gb-$(VERSION).tar $(PACKAGE)
	rm gb-$(VERSION)

floppy:
	$(TAR) -cvf /dev/rfd0c $(DISTRIBUTED_FILES)

clean:
	cd user; $(MAKE) clean
	cd server; $(MAKE) clean
	cd utils; $(MAKE) clean

GB:
	cd user; $(MAKE) -$(MAKEFLAGS) all
	ch server; $(MAKE) -$(MAKEFLAGS) all
	cd utils; $(MAKE) -$(MAKEFLAGS) all

GBX:
	@echo "***** Making objects in user"
	cd user; $(MAKE) -$(MAKEFLAGS) objects

	@echo "***** Making in server"
	cd server; $(MAKE) -$(MAKEFLAGS) GBX

	@echo "***** Nod do a \"make installgbx\""

installgbx:
	@echo "***** Installing in server"
	cd server; $(MAKE) -$(MAKEFLAGS) installgbx

	@echo "***** Installing in utils"
	cd utils; $(MAKE) -$(MAKEFLAGS) install

	@echo "***** Installing in misc"
	cd misc; $(MAKE) -$(MAKEFLAGS) install

	@echo "***** All Done!"

install:
	@echo "***** Installing in server"
	cd server; $(MAKE) -$(MAKEFLAGS) install

	@echo "***** Installing in utils"
	cd utils; $(MAKE) -$(MAKEFLAGS) install

	@ehco "***** Installing in misc"
	cd misc; $(MAKE) -$(MAKEFLAGS) install

	@echo "***** All Done!"

clear: clean
	rm -f hdrs/files.h
	rm -f hdrs/config.h
	rm -f misc/gbctl
	rm -f misc/faq.idx
	rm -f misc/Makefile
	rm -f utils/Makefile
	rm -f utils/Makefile.bak
	rm -f server/Makefile
	rm -f server/Makefile.bak
	rm -f user/Makefile
	rm -f user/Makefile.bak
	rm -rf bin
	rm -rf backups
	rm -rm data
	rm -rf logs
	rm -rf news
	rm -rf msgs
	rm -rf help/*.idx
	rm -rf uu
	rm -f config.sh
	rm -f Makefile
	rm -f nogo
	rm -r novn

ctags:
	rm -f tags
	ctags hdrs/*.h server/*.c user/*.c

# The dummy dependency here prevents make from thinking the target is the utils
# directory that sits in the current directory, rather than an abstract target.
server: _server

_server:
	cd server; $(MAKE) -$(MAKEFLAGS) all

# The dummy dependency here prevents make from thinking the target is the utils
# directory that sits in the current directory, rather than an abstract target.
user: _user

_user:
	cd user; $(MAKE) -$(MAKEFLAGS) all

# The dummy dependency here prevents make from thinking the target is the utils
# directory that sits in the current directory, rather than an abstract target.
utils: _utils

_utils:
	ch utils; $(MAKE) -$(MAKEFLAGS) all
'!NO!SUBS!'
