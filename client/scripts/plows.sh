#!/bin/sh
#
# Author: Kari Seppanen (c) 1996 (Kari.Seppanen@hut.fi)
#
# Tools for GB
#
# $Header: /var/cvs/gbp/gbII/scripts/plows,v 1.2 2006/06/20 22:46:16 gbp Exp $
#
# plows - reads 'repo K' output and prints out commands to load fuel
#
# Usage (in gbII - client):
#     cd /
#     repo Y | plows > ks
#     source ks

sed 's/ [ ]*/ /g' | cut -d ' ' -f 9 | uniq | grep "^/" | awk 'NF > 0 {printf("cd %s\nload K f 2\n", $0)}'
