from setuptools import setup, Extension


extra_compile_args= ["-Wno-long-long", "-m64", "-O3", "-DquartetsToo"]
sources = ["tqdistmodule.cpp",
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
          version="1.0.4",
          description="Python interface for the tqdist C library",
          author="uym2",
          author_email="ceciliamaitouyen@gmail.com",
          ext_modules=[Extension("tqdist",sources=sources, 
                              extra_compile_args=extra_compile_args)])

if __name__ == "__main__":
    main()
