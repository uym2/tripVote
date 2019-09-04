# !/bin/bash

s_tree=$1       # (input) the species tree
popSize=$2      # (input) population size
g_trees=$3      # (input) the gene trees to be rooted
clps_tree=$4    # (output) the collapsed species tree will be written here
rg_trees=$5     # (output) the rooted gene trees will be written here


echo "Collapsing the species tree..."
collapse_level_maxTrpl_score.py $s_tree $popSize $clps_tree
echo "Finish collapsing. The collapsed species tree was written to $clps_tree"

echo "Launching tripRoot. Using $clps_tree as reference"
tripRoot $clps_tree $g_trees $rg_trees 
echo "Finish tripRoot. Rooted trees were written to $rg_trees"
