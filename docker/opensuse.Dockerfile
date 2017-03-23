FROM opensuse
MAINTAINER <peter@goldsborough.me>

# Install packages.
RUN zypper --non-interactive refresh \
 && zypper --non-interactive install \
 git cmake vim make llvm-clang gcc5 gcc5-c++ libstdc++6-devel-gcc5

 ENV C clang-3.8
 ENV CXX clang++-3.8

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
CMD ["./build.sh", "opensuse"]
