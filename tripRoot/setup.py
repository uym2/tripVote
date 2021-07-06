from setuptools import setup, Extension


extra_compile_args= ["-Wno-long-long", "-m64", "-O3"]
sources = [ "triprootmodule.cpp",
            "CountingArray.cpp",
            "HDT.cpp",
            "HDTCountingCCToC.cpp",
            "HDTCountingCTransform.cpp",
            "HDTCountingG.cpp",
            "HDTCountingIGToC.cpp",
            "HDTCountingLeaf.cpp",
            "HDTFactory.cpp",
            "HDTListUtils.cpp",
            "MinVarCounter.cpp",
            "MVRooting.cpp",
            "NewickParser.cpp",
            "RootedTree.cpp",
            "RootedTreeFactory.cpp",
            "int_stuff.cpp",
            "TripletCounter.cpp",
            "TripletRooting.cpp",]
def main():
    setup(name="triproot",
          version="1.0",
          description="Python interface for the triproot library",
          url="https://github.com/uym2/tripVote/tree/master/tripRoot",
          author="uym2",
          author_email="ceciliamaitouyen@gmail.com",
          ext_modules=[Extension("triproot",sources=sources, 
                              extra_compile_args=extra_compile_args,
                              include_dirs=["/Users/uym2/my_gits/Triplet_rooting/tripRoot"])])

if __name__ == "__main__":
    main()
