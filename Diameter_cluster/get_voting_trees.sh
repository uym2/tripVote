#! /bin/bash

myTree=$1
group=$2
output=$3

while read line; do
    prune_tree.py -i $myTree -o temp -l "`echo $line`" -v
    cat temp >> $output
done < grouping.txt    
