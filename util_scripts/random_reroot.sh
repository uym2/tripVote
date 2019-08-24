#! /bin/bash

inTree=$1
L=`nw_labels $inTree`
R=`nw_labels -r $inTree`
N=1

for x in $L; do
    if [[ $x != $R ]]; then 
        r=`echo $RANDOM/32768*$N | bc -l`
        if (( $(echo "$r <= 1" | bc -l) )) ; then
            y=$x
        fi   
        N=$((N+1))
    fi    
done

nw_reroot $inTree $y 

