FROM fedora
MAINTAINER <peter@goldsborough.me>

# Install packages.
RUN dnf update  -y \
 && dnf install -y git cmake vim make clang python ncurses-devel

ENV C clang
ENV CXX clang++

# Grab LLVM and clang.
RUN git clone --progress --verbose \
    https://github.com/llvm-mirror/llvm.git llvm
RUN git clone --progress --verbose \
    https://github.com/llvm-mirror/clang.git llvm/tools/clang

# These volumes should be mounted as named volumes.
VOLUME /llvm/build /home/build

# These volumes should be mounted on the host.
VOLUME /home/project /home/build/bin

WORKDIR /home
COPY build.sh .
