#! /bin/bash

inTrees=$1
outTrees=$2
lbTrees=`mktemp`
temp=`mktemp`
labelTrees.py -i $inTrees -o $lbTrees

while read tree; do
    echo $tree > $temp
    random_reroot.sh $temp >> $outTrees 
done < $lbTrees

rm $temp $lbTrees    
