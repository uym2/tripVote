* Version 1.3.1:
    * Preserve original internal node labels in rooted-tree output instead of leaking temporary `I*` labels
    * Treat internal node labels as branch labels when rerooting, moving labels along rerooted edges
    * Restore original internal node labels in completed trees after temporary placement labeling
    * Add `--preserve-old-root` to optionally keep the old root as a unary node; default output suppresses it for downstream compatibility
* Version 1.3:
    * Add public `tripVote_root_trees.py` CLI for rooting input trees with respect to reference trees
    * Fix `tripVote_root` wrapper issues with `log2`, `is_root()`, missing branch lengths, and numeric sampling
* Version 1.2:
    * Fix a mistake in version 1.1 in argument parsing   
* Version 1.1:
    * Fix the noeol issue in output file
	* Optimize function `complete_gene_trees` of module `tripVote.placement_lib` 
	* Add `placement_taxa` option
