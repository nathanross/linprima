#!/bin/bash
cp libjson-c1.12_20140712.c src.cpp && cat newlinebuffer.c >> src.cpp && cat esp.cpp >> src.cpp && clang++ -std=c++0x -stdlib=libc++ src.cpp 
