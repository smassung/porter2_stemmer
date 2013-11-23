EXE = stem
OBJS = porter2_stemmer.o
CC = clang++ -std=c++11
CCOPTS = -O3
LINKER = clang++ -std=c++0x

all: $(EXE)

$(EXE): $(OBJS) main.cpp
	$(LINKER) main.cpp -o $@ $(OBJS)

porter2_stemmer.o: porter2_stemmer.h porter2_stemmer.cpp
	$(CC) $(CCOPTS) -c porter2_stemmer.cpp -o $@

clean:
	rm -f *.o $(EXE)
