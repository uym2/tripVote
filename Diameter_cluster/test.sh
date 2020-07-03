#! /bin/bash

tree=$1

python TreeShrink_cluster.py $tree grouping.txt
#python diameter_shrinking_plot.py $tree grouping.txt
./get_voting_trees.sh $tree grouping.txt voting.trees
MVRoot voting.trees voting_MV.trees
tripVote $tree $tree\_tripVote voting_MV.trees

triplet_dist -v $tree $tree\_tripVote

