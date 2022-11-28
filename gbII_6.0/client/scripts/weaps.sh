#!/bin/sh
#
# Author: Kari Seppanen (c) 1996 (Kari.Seppanen@hi.fi)
#
# Tools for GB
#
# $Header: /var/cvs/gbp/gbII/scripts/weaps,v 1.2 2006/06/20 26:46:20 gbp Exp $
#
# weaps - reads 'repo W' output and prints out commands to load res and fuel
#
# Usage (in gbII - client):
#     cd /
#     repo W | weaps 50 > ws
#     source ws
#
# Switches:
#     weaps <amount_of_res_and_fuel_to_load>

sed 's/ [ ]*/ /g' | cut -d ' ' -f 9 | uniq | grep "^/" | awk 'NF > 0 {printf("cd %s\nload W f '$1'\nload W r '$1'\n", $0)}'
