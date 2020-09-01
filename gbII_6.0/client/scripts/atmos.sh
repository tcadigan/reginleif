#!/bin/sh
#
# Author: Kari Seppanen (c) 1996 (Kari.Seppanen@hut.fi)
#
# Tools for GB
#
# $Header: /var/cvs/gbp/gbII/scripts/atmos,v 1.2 2006/06/20 22:46:02 gbp Exp $
#
# atmos - reads 'repo a' output and prints out commands to load fuel
#
# Usage (in gbII -client:
#     cd /
#     repo W | atmos > as
#     source as

sed 's/ [ ]*/ /g' | cut -d ' ' -f 9 | uniq | grep "^/" | awk 'NF > 0 {printf("cd %s\nload a f 3\n", $0)}'
