#! /bin/bash

# check all the rooting possible (check all branches) and
# find the minimum triplet distance comparing to the true tree

truetree=$1
esttree=$2
opttree=$3 # output

info=`mktemp info-XXXXX`
tmptre=`mktemp tre-XXXXX`

min_dtrpl=`triplet_dist $truetree $esttree`
cp $esttree $opttree

root_lb=`nw_labels -r $esttree`
nw_labels $esttree > $info

while read node; do
    if [ ! "$root_lb" == "$node" ]; then
		nw_reroot $esttree $node > $tmptre
		dtrpl=`triplet_dist $truetree $tmptre`
        if [ "$min_dtrpl" -gt "$dtrpl" ]; then
			min_dtrpl=$dtrpl
			cp $tmptre $opttree
		fi
	fi
done < $info

echo $min_dtrpl

rm $info $tmptre
