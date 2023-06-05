option(wasmsbx "Use WASM SBX" OFF)

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

set(RUN_OPTIONS words 2 < input.OUT)
llvm_multisource(anagram)
llvm_test_data(anagram words input.OUT)

if(wasmsbx)
    target_link_libraries(anagram PRIVATE ${LIBS})
endif()
