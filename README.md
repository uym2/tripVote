This is an implementation of tripVote: complete a set of gene trees to maximize the quartet score.
The core algorithm is the Maximum Triplet Rooting (MTR) problem:
    Root a tree T according to a reference tree R such that the triplet score of T and R is maximized.

The algorithm and the code heavily depend on the tqDist software http://birc.au.dk/software/tqDist/. 
We would like to thank the authors for the code and algorithmic ideas.

**Installation: using pip**

```bash
pip install tripvote
```

Now you should be able to run the TripletVoting program. Type 

``` bash
tripVote_complete_trees.py -h
```

to learn how to use.

Below is an example (to run, you need to download the file [incomplete.trees](test_cases/incomplete.trees) under folder `test_cases` of this repo to your machine):

```
tripVote_complete_trees.py -i incomplete.trees -o completed.trees
```

To root input trees with respect to rooted reference trees, use:

``` bash
tripVote_root_trees.py -i unrooted.trees -r refTrees.trees -o rooted.trees
```

By default, the rooting command suppresses the old root if rerooting would otherwise leave a unary internal node. To keep the old root node and preserve that branch-length split exactly, add `--preserve-old-root`.

**Notes:**

* Older releases were distributed on PyPI as `TripletVoting`. Maintained releases are now distributed as `tripvote`.
