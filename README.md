linprima
========

node+browser asmjs implementation of esprima

Esprima is one of the most highly used libraries in Node js, it sees something like 70k downloads a day. Significant  computational-hours could be saved by developing a much faster drop-in replacement of Esprima. 

The project also aims to demonstrate the viability of transpiling to ASMJS for unrivaled performance gains in small, everyday libraries and applications.

Related work
---

the only competitors to esprima on speed simply cannot match its functionality 1-to-1. And are limited in their advantage for large files because of the low ceiling and diminishing returns on pure, non-asmjs optimization.

Status
---
Currently the stable branch passes all but 4 of esprima's ~750 unit tests.
Stable branch is  at ~120% performance within FF with minimal options (see bench/ folder). 

Concurrent branch changes 
(switch to pared-down Write-Only Json class) likely to bring performance up quite a bit.

Motive
---

This project aims to demonstrate the viability of emscripten-compiled asmjs for small, basic libraries. Currently, asmjs is seen as some niche tool for niche uses like media editing or 3d graphics. 

This project aims to be a case study in ASMJS' viability, but also the cutting-edge speed advantage in can provide in applications with continuous-processing. 

As well, this project contains several innovations regarding retaining compatibility with the javascript ecosystem of functioning across browsers and with a variety of module loaders (requirejs, node, and bower in addition to independent use as a script)

Build instructions
---
You'll need to get a recent version of emscripten, please see [emscripten's downloads and instructions] page.

[emscripten's downloads and instructions]:http//google.com

after that's installed and emcc is on your path:

```
sudo apt-get install scons npm 

npm install

#ASMJS (builds to build/asm/)
gulp asmjs

#library (builds to build/linprima.so)
#and node-FFI (builds to build/ffi/)
gulp clang

```

Profiling and analysis
```
# memory profiling (massif)
# (outputs to folder in tmp/mem/)
gulp mem

# llvm (no emcc) flat profiling
# uses Profiny.h
# (outputs to tmp/profiny.flat.csv)
# best viewed in a spreadsheet app
# like gnumeric, etc.
gulp prof

# llvm (no emcc) callgraph profiling
# (outputs to tmp/profiny.graphed.out)
# results best viewed with a log viewer
gulp prof --graph

# memcheck
# check for and resolve any memory 
# leaks on edit (for the test case in
# tests/codetotest.js)
gulp clang --gdb
valgrind --leak-check=full tmp/test_gdb
```
