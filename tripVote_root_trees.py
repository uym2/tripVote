#! /usr/bin/env python

import argparse
import sys
import time

import tripVote


def read_trees(path):
    with open(path, 'r') as fin:
        return [line.strip() for line in fin if line.strip()]


def parse_sampling(value):
    if value is None:
        return 'full', None

    fields = value.strip().split()
    if len(fields) != 2:
        raise argparse.ArgumentTypeError(
            'sampling must have two fields, e.g. "sqrt 10", "full 5", or "0 0"'
        )

    sample_size, nsample = fields
    if sample_size == '0' and nsample == '0':
        return 'full', None
    if sample_size not in ('sqrt', 'full'):
        sample_size = int(sample_size)
    nsample = int(nsample)
    if nsample < 0:
        raise argparse.ArgumentTypeError('number of samples must be non-negative')
    return sample_size, nsample


def main():
    parser = argparse.ArgumentParser(
        description='Root one or more trees by tripVote with respect to reference trees.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument('-i', '--input', required=True, help='Input trees to root, one Newick tree per line')
    parser.add_argument('-r', '--refs', required=True, help='Reference rooted trees, one Newick tree per line')
    parser.add_argument('-o', '--output', required=True, help='Output rooted trees')
    parser.add_argument('-d', '--max-depth', default='max',
                        help='Maximum reference-tree depth to use: max, log2, or an integer')
    parser.add_argument('-s', '--sampling', type=parse_sampling,
                        help='Sample size and number of samples, e.g. "sqrt 10" or "full 5"; use "0 0" for no sampling')
    parser.add_argument('-v', '--version', action='version', version=tripVote.PROGRAM_VERSION,
                        help='Show program version and exit')

    args = parser.parse_args()
    sample_size, nsample = args.sampling if args.sampling is not None else ('full', None)

    start = time.time()
    print("Running " + tripVote.PROGRAM_NAME + " version " + tripVote.PROGRAM_VERSION)
    print("tripVote_root_trees was called as follow: " + " ".join(sys.argv))

    input_trees = read_trees(args.input)
    ref_trees = read_trees(args.refs)

    from tripVote.tripvote_lib import tripVote_root

    rooted_trees = []
    for i, tree_str in enumerate(input_trees):
        print("Rooting tree " + str(i + 1))
        rooted_tree, d2root = tripVote_root(
            tree_str,
            ref_trees,
            max_depth=args.max_depth,
            sample_size=sample_size,
            nsample=nsample,
        )
        rooted_trees.append(rooted_tree)
        print("Rooted tree " + str(i + 1) + "; distance to original root: " + str(d2root))
        print(rooted_tree)

    with open(args.output, 'w') as fout:
        fout.write("\n".join(rooted_trees))
        if rooted_trees:
            fout.write("\n")

    end = time.time()
    print("Runtime: ", end - start)


if __name__ == "__main__":
    main()
