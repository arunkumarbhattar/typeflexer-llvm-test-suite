option(wasmsbx "Use WASM SBX" OFF)

# Setting compilers
set(CMAKE_C_COMPILER "/home/arun/Desktop/TypeFlexer-Clang/llvm/cmake-build-debug/bin/clang")
set(CMAKE_CXX_COMPILER "g++")

# Adding compile flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fwasmsbx -DWASM_SBX -fPIE")
link_libraries("/home/arun/Desktop/WASM_SBX/libSBX_CON_LIB.a" "/home/arun/Desktop/WASM_SBX/libisc_lib_final.a" dl "/usr/lib/x86_64-linux-gnu/libstdc++.so.6")

set(RUN_OPTIONS words 2 < input.OUT)
llvm_multisource(anagram)
llvm_test_data(anagram words input.OUT)
