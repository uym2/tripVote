#! /bin/bash

inTrees=$1
outTrees=$2
refTrees=$3

while read tree; do 
    tempIn=`mktemp` 
    tempOut=`mktemp`
    tempWeights=`mktemp`  
    echo $tree > $tempIn
    quartet_dist_2_ref $tempIn $refTrees $tempWeights 
    tripVote $tempIn $tempOut $refTrees $tempWeights 
    cat $tempOut >> $outTrees 
    rm $tempIn 
    rm $tempOut 
    rm $tempWeights
done < $inTrees
