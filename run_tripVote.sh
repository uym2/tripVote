#! /bin/bash

inTrees=$1
outTrees=$2
refTrees=$3
weights=$4

while read tree; do 
    tempIn=`mktemp` 
    tempOut=`mktemp`
    tempWeights=`mktemp`  
    echo $tree > $tempIn
    if [ $weights = 'y' ]; then
        quartet_dist_2_ref $tempIn $refTrees $tempWeights 
    else
        n=`wc -l $refTrees | awk '{print $1;}'`
        for i in $(seq 1 1 $n); do
            echo 1.0
        done > $tempWeights                
    fi        
    tripVote $tempIn $tempOut $refTrees $tempWeights 
    cat $tempOut >> $outTrees 
    rm $tempIn 
    rm $tempOut 
    rm $tempWeights
done < $inTrees
