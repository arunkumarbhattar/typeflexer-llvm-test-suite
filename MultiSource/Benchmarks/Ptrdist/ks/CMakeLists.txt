# Setting compilers
set(CMAKE_C_COMPILER "/home/arun/Desktop/4matic/Typematic/llvm/cmake-build-debug/bin/clang")
set(CMAKE_CXX_COMPILER "g++")

# Adding compile flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fwasmsbx -DWASM_SBX -fPIE")
link_libraries("/home/arun/Desktop/WASM_SBX/libSBX_CON_LIB.a" "/home/arun/Desktop/WASM_SBX/libisc_lib_final.a" dl "/usr/lib/x86_64-linux-gnu/libstdc++.so.6")

if(LARGE_PROBLEM_SIZE)
  set(RUN_OPTIONS KL-5.in)
  set(HASH_PROGRAM_OUTPUT 1)
  llvm_multisource(ks)
  llvm_test_data(ks KL-5.in)
else()
  set(RUN_OPTIONS KL-4.in)
  llvm_multisource(ks)
  llvm_test_data(ks KL-4.in)
endif()
