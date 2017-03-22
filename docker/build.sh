#!/bin/bash

# If the LLVM build folder doesn't exist yet, create it.
if [[ ! -d /llvm/build ]]; then
  echo 'Creating /llvm/build folder'
  mkdir /llvm/build
fi

# If the folder is empty, build it.
if [[ -n $(find /llvm/build -maxdepth 0 -empty) ]]; then
  echo 'Building LLVM and clang ...'
  cd /llvm/build
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DLLVM_ENABLE_ASSERTIONS=OFF \
        -DCMAKE_CXX_COMPILER=clang++-3.8 \
        ..
  make -j4
  cd -
fi

# If the project build folder doesn't exist yet, create it.
if [[ ! -d /home/build ]]; then
  echo 'Creating /build folder'
  mkdir /home/build
fi

# If the folder is empty, build it.
echo 'Building project ...'
cd /home/build
cmake -DLLVM_PATH=/llvm \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_COMPILER=clang++-3.8 \
      -DVERBOSE_CONFIG=on \
      -DCLANG_EXPAND_OS_NAME= \
      /home/project
make -j4
cd -
