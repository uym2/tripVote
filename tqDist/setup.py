#from setuptools import setup, Extension
from distutils.core import setup, Extension


extra_compile_args= ["-Wno-long-long", "-m64", "-O3", "-DquartetsToo"]
headers = [ "AbstractDistanceCalculator.h",	"newick_parser.h",
            "QuartetDistanceCalculator.h",	"quartet_calc.h",
            "TripletDistanceCalculator.h",	"rooted_tree.h",
            "counting_linked_list.h",		"rooted_tree_factory.h",
            "counting_linked_list_num_only.h",	"templated_linked_list.h",
            "hdt.h",	"triplet_calc.h",
            "hdt_factory.h", "unrooted_tree.h",
            "int_stuff.h",	"util.h", "meory_allocator.h"]
sources =  ["tqdistmodule.cpp",
           "HDT.cpp",
           "HDTCountingCCToC.cpp",
           "HDTCountingCTransform.cpp",
           "HDTCountingG.cpp",
           "HDTCountingIGToC.cpp",
           "HDTCountingLeaf.cpp",
           "HDTFactory.cpp",
           "HDTListUtils.cpp",
           "NewickParser.cpp",
           "RootedTree.cpp",
           "RootedTreeFactory.cpp",
           "AbstractDistanceCalculator.cpp",
           "TripletDistanceCalculator.cpp",
           "QuartetDistanceCalculator.cpp",
           "int_stuff.cpp"]

def main():
    setup(name="tqdist",
          version="1.0",
          description="Python interface for the tqdist C library",
          author="uym2",
          author_email="ceciliamaitouyen@gmail.com",
          ext_modules=[Extension("tqdist",sources=sources,
                              extra_compile_args=extra_compile_args)])

if __name__ == "__main__":
    main()
