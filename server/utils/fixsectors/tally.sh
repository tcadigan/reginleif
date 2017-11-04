#!/bin/bash

total=0

for poo in `cat sect.tot`;
do
    total=$(($total+$poo))
done

echo $total

exit
