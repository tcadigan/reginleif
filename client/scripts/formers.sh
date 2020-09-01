#!/bin/sh
#
# Author: Kari Seppanen (c) 1996 (Kari.Seppanen@hut.fi)
#
# Tools for GB
#
# $Header: /var/cvs/gbp/gbII/scripts/formers,v 1.2 2006/06/20 22:46:10 gbp Exp $
#
# formers - reads 'repo T' output and prints out commands to load fuel
#
# Usage (in gbII - client):
#     cd /
#     repo T | formers > fs
#     source fs

sed 's/ [ ]*/ /g' | cut -d ' ' -f 9 | uniq | grep "^/" | awk 'NF > 0 {printf("cd %s\nload T f 3\n", $0)}'
