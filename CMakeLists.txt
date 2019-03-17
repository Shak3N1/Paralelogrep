cmake_minimum_required (VERSION 3.10.2)
project (ParaleloGrep)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
ADD_LIBRARY(pgrep
    src/pgrep.hpp
    src/pgrep.cpp
)
target_link_libraries(pgrep -lpthread -lstdc++fs)
add_executable (paralelogrep src/main.cpp)
target_link_libraries(paralelogrep pgrep)
