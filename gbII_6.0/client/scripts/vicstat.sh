#!/bin/sh
#
# $Header: /var/cvs/gbp/gbII/scripts/vicstat,v 1.5 2006/06/20 22:45:35 gbp Exp $
#
# (c) Kari Seppanen 1995
#
# vicstat - make statistics from victory listings
#
# Usage: You should have saved 'victory' lists in same directory as 'vic.update'
#        i.e. vic.1 vic.2, etc. then just run vicstats in that directory.
#
# Switches:
#     -n <num_update> : print only 'num_updates' last updates
#     -s : sort races using last update

tmp=/usr/tmp

usage="vicstat [-s] [-n updates]"

trap "echo 'removing temporary files'; rm $tmp/$$.vic" 1 2 3 15

set -- `getopt sn: $*`

updates=9999
sort=0

while [ $# -gt 1 ]; do
    case $1 in
        -n)
            updates=$2
            shift 2
            ;;
        -s)
            sort=1
            shift
            ;;
        *)
            echo $usage
            shift
            ;;
    esac
done

for vicfile in `ls vic.* | sort -t. -k 2n,2`
do
    cat $vicfile >> $tmp/$$.vic
done

cat $tmp/$$.vic | tr -d "\[\]" | awk '
BEGIN { maxi = 0
        mini = 9999
        rnds = 0
        for (i = 0; i < 60; i++)
          itbl[i] = 0 }
/^[ ]*[0-9]+/ { racetab[$2, itbl[$2]] = $1
                tempnam = $3 " " $4 " " $5 " " $6 " " $7
                nametab[$2] = substr(tempnam, 1, 10)
                itbl[$2]++
                if (itbl[$2] > rnds)
                  rnds = itbl[$2]
                if ($2 < mini)
                  mini = $2
                if ($2 > maxi)
                  maxi = $2 }
END { startidx = rnds - '$updates'
      startidx = (startidx > 0) ? startidx : 0
      if ('$sort') {
        for (k = 1; k <= maxi - mini + 1; k++) {
          for(i = mini; i <= maxi; i++) {
            if(racetab[i, rnds - 1] == k) {
              printf("[%2d] %s\t", i, nametab[i])
              for(j = startidx; j < rnds; j++)
                printf("%3d", racetb[i, j])
              printf("\n")
              break;
            }
          }
        }
      }
      else {
        for(i = mini; i <= maxi; i++) {
          printf("[%2d] %s\t", i, nametab[i])
          for(j = startidx; j < rnds; j++)
            printf("%3d", racetab[i, j])
          printf("\n")
        }
      }
    }
' | grep -v " GOD " | grep -v " guest "

rm $tmp/$$.vic
