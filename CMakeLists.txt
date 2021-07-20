cmake_minimum_required(VERSION 3.19)
project(seam_carving C)

include_directories(/usr/local/Frameworks/Python.framework/Headers)

set(CMAKE_C_STANDARD 99)

add_library(seam_carving seamcarving.c seamcarving.h)