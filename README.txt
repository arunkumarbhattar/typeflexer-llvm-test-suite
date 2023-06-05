LLVM "test-suite" Repository
----------------------------

Please see the LLVM testing infrastructure guide:

  https://llvm.org/docs/TestSuiteGuide.html

for more information on the contents of this repository.

Command to use -->
cmake -DCMAKE_C_COMPILER=/home/arun/Desktop/TypeFlexer-Clang/llvm/cmake-build-debug/bin/clang -DCMAKE_CXX_COMPILER=g++ -Dwasmsbx=ON \
                                          -C ../cmake/caches/O3.cmake ../


