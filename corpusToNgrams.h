#include <stdlib.h>

#ifndef _CORPUS_TO_NGRAMS_H_
#define _CORPUS_TO_NGRAMS_H_

typedef struct orderset {
    size_t size;
    char **ngram;
} orderset_t;

typedef struct ngrams {
    size_t max_order;
    orderset_t *orderset;
} ngrams_t;

ngrams_t *ngrams_init ( size_t max_order );
int ngrams_corpus_parse ( const char *corpus, size_t corpus_size, ngrams_t *ngrams );
void ngrams_free ( ngrams_t *ngrams );

#endif
