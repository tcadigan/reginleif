#!/bin/sh
#
# Author: Kari Seppanen (c) 1996 (Kari.Seppanen@hut.fi)
#
# Tools for GB
#
# $Header: /var/cvs/gbp/gbII/scripts/domes,v 1.2 2006/06/20 22:46:06 gbp Exp $
#
# domes - reads 'repo Y' output and prints commands to load res
#
# Usage (in gbII - client):
#     cd /
#     repo Y | domes > ys
#     source ys

sed 's/ [ ]*/ /g' | cut -d ' ' -f 9 | uniq | grep "^/" | awk 'NF > 0 {printf("cd %s\nload Y r 1\n", $0)}'
