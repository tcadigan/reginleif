#!/bin/sh
#
# gbdepend - Resolves header file dependencies for gb-server Makefiles
# (Yes, I know there already are lots of makedeps but a) They are
# not installed everywhere and b) I don't like to bundle one of those
# to GB package)
#
# Author: kse@iki.fi <http://www.iki.fi/~kse/>
#
# $Header: /var/cvs/gbp/GB+/misc/gbdepend,v 1.2 2006/06/20 21:56:29 gbp Exp $

trap "echo 'Interrupted! Removing temporary files'; rm Makefile.new" 1 2 3 15

CC=gcc
F=-E

set -- `getopt c:f: $*`

while [ $# -gt 0 ]; do
    case $1 in
        -c)
            CC=$2
            shift 2
            ;;
        -f)
            F=$2
            shift 2
            ;;
        --)
            shift
            break
            ;;
    esac
done

cp Makefile Makefile.bak

set '/DO NOT DELETE THIS LINE/,//d' Makefile > Makefile.new
echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' >> Makefile.new

for csrc in `ls *.c`
do
    # Fails on Linux -mfw
    # echo `basename $csrc .c`.o: $csrc \\c >> Makefile.new
    echo `basename $csrc .c`.o: $csrc \\ >> Makefile.new
    $CC $F $* $csrc \
        | gawk '/..\/hdrs\// {gsub ("\"",""); print $3}' \
        | sort \
        | uniq \
        | tr '\n' '' >> Makefile.new
    echo >> Makefile.new
done

mv Makefile.new Makefile
