cmake_minimum_required(VERSION 3.10)
project(LzmaSdkExample C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# build LZMA SDK static library
file(GLOB LZMA_SOURCES "${PROJECT_SOURCE_DIR}/lzma2501/C/*.c")
add_library(lzma STATIC ${LZMA_SOURCES})
set_target_properties(lzma PROPERTIES LINKER_LANGUAGE C)
target_include_directories(lzma PUBLIC
    ${PROJECT_SOURCE_DIR}/lzma2501/C
)

# find zlib
find_package(ZLIB REQUIRED)

# Now build test executable
add_executable(test_lzma
    lzma.cpp
    lzma2.cpp
    zip.cpp
    main.cpp
)

target_include_directories(test_lzma PRIVATE
    ${PROJECT_SOURCE_DIR}/lzma2501
    ${PROJECT_SOURCE_DIR}/
)

target_link_libraries(test_lzma PRIVATE lzma ZLIB::ZLIB)
