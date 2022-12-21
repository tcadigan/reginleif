case $CONFIG in
    '')
        if test ! -f config.sh; then
            ln ../config.sh . || \
                ln ../../config.sh . || \
                ln ../../../config.sh . || \
                (echo "Can't find config.sh"; exit 1)
        fi
        . ./config.sh
        ;;
esac

case "$0" in
    */*)
        cd `expr X$0 : 'X\(.*\)/'`
        ;;
esac

echo "Extracting misc/Makefile (with variable substitutions)"
cat >Makefile <<EOF
#
# misc level Makefile for the GB+ program
# This Makefile will make the commands library routines
#
# Bug reports, patches, comments, suggestions should be sent to:
#
#       John Deragon deragon@jethro.cs.nyu.edu
#
# Revision 1.0  92/12/10 jpd
#
#
# Variables
#       Variables established by Configure
TOUCH=$touch
EOF

cat >>Makefile <<EOF
# Variables you may want to manually edit
# If you want debug logging then you'll want to uncomment the following.
# Other general variables
BIN=../bin
SHELL=/bin/sh

all: install

install:
	cp gbctl $(BIN)
	chmod +x $(BIN)/gbctl
EOF
