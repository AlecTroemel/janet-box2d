# Copyright (c) 2018 Calvin Rose
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.7)
project(janetbox2d
	VERSION 1.0
	LANGUAGES C)
set(TARGET_NAME ${PROJECT_NAME})

set(SOURCES
main.c
)

# get the box2d headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/box2c/include/)

# include box2d library
# NOTE alpha box2d v3 currently has a cheeky name, as its being writen in C
set(BOX2D_SAMPLES OFF)
set(BOX2D_DOCS OFF)
set(BOX2D_USER_CONSTANTS ON)
add_subdirectory(box2c)

# Build our library
add_library(${TARGET_NAME} MODULE ${SOURCES})
target_link_libraries(${TARGET_NAME} box2d)
