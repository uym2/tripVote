from setuptools import setup, Extension
import tripVote


def main():
    setup(name="tripvote",
          version=tripVote.PROGRAM_VERSION,
          description="The tripvote Python software",
          long_description="This is an implementation of tripVote: complete a set of gene trees to maximize the quartet score. The core algorithm is the Maximum Triplet Rooting (MTR) problem: Root a tree T according to a reference tree R such that the triplet score of T and R is maximized. The algorithm and the code heavily depend on the tqDist software http://birc.au.dk/software/tqDist/. We would like to thank the authors for the code and algorithmic ideas.",
          long_description_content_type="text/markdown",
          url="https://github.com/uym2/tripVote",
          author="uym2",
          author_email="ceciliamaitouyen@gmail.com",
          python_requires='>=3',
          scripts=["tripVote_complete_trees.py","tripVote_root_trees.py"],
          install_requires=["tqdist","treeswift","triproot","setuptools","wheel"],
          packages=["tripVote"],
          license="GPL-3.0-or-later",
          license_files=["LICENSE", "COPYING", "COPYING.LESSER"],
          classifiers=["Environment :: Console",
                       "Intended Audience :: Developers",
                       "Intended Audience :: Science/Research",
                       "Natural Language :: English",
                       "Programming Language :: Python",
                       "Topic :: Scientific/Engineering :: Bio-Informatics",
                        ])

if __name__ == "__main__":
    main()
