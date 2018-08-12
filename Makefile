CFLAGS=-g -Wall -fPIC -I/usr/local/include
LIBS=-L/usr/local/lib

all: libCorpusToNgrams

test: testCorpusToNgrams
	./testCorpusToNgrams test-corpus.txt

testCorpusToNgrams: Makefile corpusToNgrams.h testCorpusToNgrams.c
	gcc ${CFLAGS} -o testCorpusToNgrams testCorpusToNgrams.c ${LIBS} -lCorpusToNgrams

libCorpusToNgrams: Makefile corpusToNgrams.o corpusToNgrams.h
	gcc -shared -o libCorpusToNgrams.so.1.0 corpusToNgrams.o ${LIBS}

corpusToNgrams.o: Makefile corpusToNgrams.h corpusToNgrams.c
	gcc ${CFLAGS} -c corpusToNgrams.c -o corpusToNgrams.o

install:
	cp libCorpusToNgrams.so.1.0 /usr/local/lib
	ln -sf /usr/local/lib/libCorpusToNgrams.so.1.0 /usr/local/lib/libCorpusToNgrams.so.1
	ln -sf /usr/local/lib/libCorpusToNgrams.so.1.0 /usr/local/lib/libCorpusToNgrams.so
	ldconfig /usr/local/lib
	cp corpusToNgrams.h /usr/local/include/corpusToNgrams.h

clean:
	rm *.o; rm *.so*; rm core*; rm testCorpusToNgrams
