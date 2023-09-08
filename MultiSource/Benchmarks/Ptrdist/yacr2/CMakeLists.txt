# Setting compilers
set(CMAKE_CXX_COMPILER "g++")

# Adding compile flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fwasmsbx -DWASM_SBX -fPIE")

link_libraries("/home/arun/Desktop/WASM_SBX/libSBX_CON_LIB.a" "/home/arun/Desktop/WASM_SBX/libisc_lib_final.a"
        dl "/usr/lib/x86_64-linux-gnu/libstdc++.so.6")


list(APPEND CPPFLAGS -DTODD)
set(RUN_OPTIONS input2.in)
set(HASH_PROGRAM_OUTPUT 1)
llvm_multisource(yacr2)
llvm_test_data(yacr2 input2.in)
