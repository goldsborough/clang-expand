
CLANG_LIBS := \
	-lclangAST \
	-lclangASTMatchers \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-lclangFormat \

WARNINGS := \
        -Wall \
        -Wextra \
        -Wunused \
        -Wdeprecated \
        -Wc++11-compat \
        -Wfuture-compat \
				-Wunreachable-code \

all: main

.phony: clean
.phony: iwyu

clean:
	rm main || echo -n ""

main: main.cpp
	$(CXX) $(WARNINGS) -I/usr/local/opt/llvm/include `llvm-config --cxxflags` -std=c++1z -fno-rtti main.cpp `llvm-config --libs --ldflags --system-libs` ${CLANG_LIBS} -g -o main
