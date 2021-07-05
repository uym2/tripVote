This is an implementation of tripVote: complete a set of gene trees to maximize the quartet score.
The core algorithm is the Maximum Triplet Rooting (MTR) problem:
    Root a tree T according to a reference tree R such that the triplet score of T and R is maximized.

The algorithm and the code heavily depend on the tqDist software http://birc.au.dk/software/tqDist/. 
We would like to thank the authors for the code and algorithmic ideas.

**Installation**

First, install CMake https://cmake.org/download/

Next, install treeswift:

```bash
pip install treeswift
```

To build and install the tripVote library, clone this repo or download the zip file and unzip. Then execute the
following commands in a terminal:

``` bash
cd <path to tripVote>/tqDist/
python setup.py install --user

cd <path to tripVote>/tripRoot/
python setup.py install --user
```

Then you should be able to run the program. Type 

``` bash
python tripVote_complete_trees.py -h
```

to see the help message.

Below is an example:

```
python tripVote_complete_trees.py -i <path to tripVote>/test_cases/incomplete.trees -o completed.trees
```
