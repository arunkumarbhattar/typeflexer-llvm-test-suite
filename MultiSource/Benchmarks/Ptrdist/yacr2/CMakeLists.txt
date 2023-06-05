if(wasmsbx)
    # Setting compilers
    set(CMAKE_C_COMPILER "/home/arun/Desktop/TypeFlexer-Clang/llvm/cmake-build-debug/bin/clang")
    set(CMAKE_CXX_COMPILER "g++")

    # Adding compile flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fwasmsbx -DWASM_SBX -fPIE")

    # Setting libraries
    set(LIBS "dl;stdc++;SBX_CON_LIB;isc_lib_final")

    # Setting link directories
    link_directories("/home/arun/Desktop/WASM_SBX/")
endif()

list(APPEND CPPFLAGS -DTODD)
set(RUN_OPTIONS input2.in)
set(HASH_PROGRAM_OUTPUT 1)
llvm_multisource(yacr2)
llvm_test_data(yacr2 input2.in)
if(wasmsbx)
    target_link_libraries(yacr2 PRIVATE ${LIBS})
endif()
