#include <stdio.h>
#include <string.h>

#include "corpusToNgrams.h"

void ngrams_free ( ngrams_t *ngrams ) {
    size_t i = 0, j = 0;

    if ( ngrams ) {
        for ( i = 0; i < ngrams->max_order; i++ ) {
            for ( j = 0; j < ngrams->orderset[i].size; j++ )
                free ( ngrams->orderset[i].ngram[j] );
        }
        free ( ngrams->orderset );
        free ( ngrams );
    }
}

ngrams_t *ngrams_init ( size_t max_order ) {
    ngrams_t *rc = NULL, *_rc = NULL;

    if ( ( _rc = malloc ( sizeof ( ngrams_t ) ) ) == NULL )
        goto over;

    _rc->max_order = max_order;

    if ( ( _rc->orderset = malloc ( sizeof ( orderset_t ) * max_order ) ) == NULL )
        goto over;

    memset ( _rc->orderset, 0, sizeof ( orderset_t ) * max_order );

    rc = _rc;
over:
    if ( rc != _rc && _rc != NULL )
        ngrams_free ( _rc );

    return rc;
}

void ngram_free ( char **ngram, size_t order ) {
    size_t i = 0;

    if ( ngram ) {
        for ( i = 0; i < order; i++ )
            if ( ngram[i] )
                free ( ngram[i] );
        free ( ngram );
    }
}

char **ngrams_calculate_next_of_order ( ngrams_t *ngrams, size_t order ) {
    char **rc = NULL, **_rc = NULL;
    size_t i = 0;

    if ( ( _rc = malloc ( sizeof ( char * ) * order ) ) == NULL )
        goto over;

    for ( i = 0; i < order; i++ ) {
        char *order_1_word = ngrams->orderset[0].ngram[ngrams->orderset[0].size - order + i];
        if ( ( _rc[i] = strndup ( order_1_word, strlen ( order_1_word ) ) ) == NULL )
            goto over;
    }

    rc = _rc;
over:
    if ( rc != _rc && _rc != NULL )
        ngram_free ( _rc, order );
    return rc;
}

int ngrams_add_to_orderset ( ngrams_t *ngrams, size_t order, const char **ngram ) {
    int rc = EXIT_FAILURE;
    char **next_ngram = NULL;
    size_t i = 0, length = 0, bytesWritten = 0, orderIndex = order - 1, sizeBefore = ngrams->orderset[orderIndex].size;

    if ( orderIndex < 0 || order > ngrams->max_order )
        goto over;

    for ( i = 0; i < order; i++ ) {
        length = length + strlen ( ngram[i] ) + 1;
    }

    if ( ( ngrams->orderset[orderIndex].ngram = realloc ( ngrams->orderset[orderIndex].ngram, sizeof ( char * ) * ( sizeBefore + 1 ) ) ) == NULL )
        goto over;

    if ( ( ngrams->orderset[orderIndex].ngram[sizeBefore] = malloc ( length ) ) == NULL )
        goto over;

    for ( i = 0; i < order; i++ ) {
        bytesWritten = bytesWritten + snprintf (
            ngrams->orderset[orderIndex].ngram[sizeBefore] + bytesWritten,
            length - bytesWritten,
            i == orderIndex ? "%s" : "%s ",
            ngram[i]
        );
    }

    ngrams->orderset[orderIndex].size++;

    if ( length != bytesWritten + 1 )
        goto over;

    if ( order < ngrams->max_order && ngrams->orderset[0].size > order ) {
        if ( ( next_ngram = ngrams_calculate_next_of_order ( ngrams, order + 1 ) ) == NULL )
            goto over;
        rc = ngrams_add_to_orderset ( ngrams, order + 1, (const char **) next_ngram );
        goto over;
    }

    rc = EXIT_SUCCESS;
over:
    if ( next_ngram )
        ngram_free ( next_ngram, order + 1 );

    if ( rc == EXIT_FAILURE ) {
        ngrams->orderset[orderIndex].size = sizeBefore;
        if ( ngrams->orderset[orderIndex].ngram[sizeBefore] )
            free ( ngrams->orderset[orderIndex].ngram[sizeBefore] );
    }

    return rc;
}

char is_stop_char ( char c ) {
    if ( c == ' ' || c == '.' || c == '!' || c == '?' || c == ',' || c == '\n' || c == '\0' || c == ';' )
        return 1;
    return 0;
}

int ngrams_corpus_parse ( const char *corpus, size_t corpus_size, ngrams_t *ngrams ) {
    int rc = EXIT_FAILURE;
    char **ngram = NULL;
    size_t i = 0, wordStart = 0;

    if ( ( ngram = malloc ( sizeof ( char * ) ) ) == NULL )
        goto over;

    for ( i = 0; i < corpus_size; i++ ) {
        if ( is_stop_char ( corpus[i] ) ) {
            if ( ngram[0] )
                free ( ngram[0] );
            ngram[0] = NULL;
            if ( ( ngram[0] = malloc ( i - wordStart + 1 ) ) == NULL )
                goto over;
            snprintf ( ngram[0], i - wordStart + 1, "%s", corpus + wordStart );
            if ( i - wordStart > 0 )
                if ( ngrams_add_to_orderset ( ngrams, 1, (const char **) ngram ) == EXIT_FAILURE )
                    goto over;
            wordStart = i + 1;
        }
    }
    
    rc = EXIT_SUCCESS;
over:
    if ( ngram ) {
        if ( ngram[0] )
            free ( ngram[0] );
        free ( ngram );
    }

    return rc;
}
