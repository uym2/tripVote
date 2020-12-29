from distutils.core import setup, Extension


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
            "int_stuff.cpp"]
def main():
    setup(name="triproot",
          version="1.0.2b",
          description="Python interface for the triproot C library function",
          author="uym2",
          author_email="umai@gmail.com",
          ext_modules=[Extension("triproot",sources=sources, 
                              extra_compile_args=extra_compile_args,
                              include_dirs=["/Users/uym2/my_gits/Triplet_rooting/tripRoot"])])

if __name__ == "__main__":
    main()
