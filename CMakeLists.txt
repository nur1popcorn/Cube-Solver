cmake_minimum_required(VERSION 3.17)
project(DLX C)

set(CMAKE_C_STANDARD 11)

add_executable(DLX cube.c cube.h dlx.c dlx.h globals.h main.c)
