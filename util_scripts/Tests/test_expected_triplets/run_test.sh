#! /bin/bash

dir=$1

echo "In theory:"
collapse_maxTrpl_score.py $dir/s_tree.trees `cat $dir/populationSize.txt` $dir/s_tree_clps.trees

triplet_dist_2_ref -v $dir/s_tree.trees $dir/truegenetrees $dir/dtrpl_before.txt
triplet_dist_2_ref -v $dir/s_tree_clps.trees $dir/truegenetrees $dir/dtrpl_after.txt

echo "In practice:"
echo "Before collapsing: " 
awk '{print $5;}' $dir/dtrpl_before.txt | numlist -avg
echo "After collapsing: " 
awk '{print $5;}' $dir/dtrpl_after.txt | numlist -avg

