# Makefile para ThreadPool (main), tptest y tpcustomtest

CXX        := g++
CXXFLAGS   := -std=c++11 -Wall -pthread -g

# Código fuente común
SRC        := thread-pool.cc Semaphore.cc
HEADERS    := thread-pool.h Semaphore.h

# Ejecutables y sus fuentes
MAIN_EXE   := threadpool
MAIN_SRC   := main.cc

TPTEST_EXE := tptest
TPTEST_SRC := tptest.cc

CUSTOM_EXE := tpcustomtest
CUSTOM_SRC := tpcustomtest.cc

.PHONY: all clean

# Target por defecto: compila todo
all: $(MAIN_EXE) $(TPTEST_EXE) $(CUSTOM_EXE)

# Ejecutable principal (main.cc)
$(MAIN_EXE): $(SRC) $(HEADERS) $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(MAIN_SRC)

# Test oficial
$(TPTEST_EXE): $(SRC) $(HEADERS) $(TPTEST_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(TPTEST_SRC)

# Test custom
$(CUSTOM_EXE): $(SRC) $(HEADERS) $(CUSTOM_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(CUSTOM_SRC)

# Limpieza
clean:
	rm -f $(MAIN_EXE) $(TPTEST_EXE) $(CUSTOM_EXE)
