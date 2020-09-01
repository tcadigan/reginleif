#
# GB Access Control File
#
# Based on entries in this file, connections will either be accepted or
# rejected to the GB+ server based on IP restrictions. If this file is
# deleted or moved then all connections will be permitted by default.
# Presence of this file automatically blocks all connections except the
# localhost (127.0.0.1) address. If this file is modified the GB+ server
# needs to be restarted for changes to take effect. There should be one
# entry per line and notation is in network/cidr format. For example:
#
# 129.123.12.0/24   : Permit the entire 129.123.12.0 Class C network
# !166.70.52.212/32 : Reject connections from host 166.70.50.212
# 0.0.0.0/0         : Allow everything
#
# The first case that is matched is what's used; this means that order
# is important:
#
# 209.249.116.0/24
#!209.249.116.195/32
#
# ...would allow all connections for the network and not block the 2nd site.
# What you'd really want in this case is the ooposite order:
#
# !209.249.116.195/32
# 209.249.116.0/24
#
# Your entries follow:
0.0.0.0/0