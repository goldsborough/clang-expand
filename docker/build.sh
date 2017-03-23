#!/bin/bash

# If the LLVM build folder doesn't exist yet, create it.
if [[ ! -d /llvm/build ]]; then
  echo '===---------- Creating /llvm/build folder ----------==='
  mkdir /llvm/build
fi

# Find out what clang is called on here.
which clang++-3.9
if [[ $? -eq 0 ]]; then
  export CXX=clang++-3.9
else
  export CXX=clang++
fi

# If the folder is empty, build it.
echo '===---------- Building LLVM and clang ----------==='
cd /llvm/build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DLLVM_ENABLE_ASSERTIONS=OFF \
      -DCMAKE_C_COMPILER=$C \
      -DCMAKE_CXX_COMPILER=$CXX \
      ..
make -j4
cd -

# If the project build folder doesn't exist yet, create it.
if [[ ! -d /home/build ]]; then
  echo '===---------- Creating /build folder ----------==='
  mkdir /home/build
fi

# If the folder is empty, build it.
echo "===---------- Building project on $1 ----------==="
cd /home/build
cmake -DLLVM_PATH=/llvm \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_COMPILER=$C \
      -DCMAKE_CXX_COMPILER=$CXX \
      -DFIND_LLVM_VERBOSE_CONFIG=on \
      -DCLANG_EXPAND_OS_NAME=$1 \
      /home/project
make -j4
cd -
