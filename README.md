This is an implementation of tripVote: complete a set of gene trees to maximize the quartet score.
The core algorithm is the Maximum Triplet Rooting (MTR) problem:
    Root a tree T according to a reference tree R such that the triplet score of T and R is maximized.

The algorithm and the code heavily depend on the tqDist software http://birc.au.dk/software/tqDist/. 
We would like to thank the authors for the code and algorithmic ideas.

**Installation**

First, use pip to install treeswift, tqdist, and triproot:

```bash
pip install treeswift tqdist triproot

```

Then you should be able to run the tripVote program. Type 

``` bash
python tripVote_complete_trees.py -h
```

to see the help message.

Below is an example:

```
python tripVote_complete_trees.py -i <path to tripVote>/test_cases/incomplete.trees -o completed.trees
```
