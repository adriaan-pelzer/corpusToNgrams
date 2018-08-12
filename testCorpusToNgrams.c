#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <corpusToNgrams.h>

#define BLOCKSIZE 1024
#define MAXORDER 7

int main ( int argc, char **argv ) {
    int rc = EXIT_FAILURE;
    char *filename = NULL;
    char *payload = NULL;
    FILE *fp = NULL;
    size_t bytes_read = 0, order = 0, i = 0;
    ngrams_t *ngrams = NULL;

    if ( argc < 2 ) {
        fprintf ( stderr, "Usage: %s filename\n", argv[0] );
        goto over;
    }

    filename = argv[1];

    if ( ( ngrams = ngrams_init ( MAXORDER ) ) == NULL ) {
        fprintf ( stderr, "Cannot initialise ngrams: %s\n", strerror ( errno ) );
        goto over;
    }

    if ( ( fp = fopen ( filename, "r" ) ) == NULL ) {
        fprintf ( stderr, "Cannot open file %s for reading: %s\n", filename, strerror ( errno ) );
        goto over;
    }

    while ( ! feof ( fp ) ) {
        if ( ( payload = realloc ( payload, bytes_read + BLOCKSIZE ) ) == NULL )
            goto over;
        bytes_read = bytes_read + fread ( payload + bytes_read, 1, BLOCKSIZE, fp );    
        printf ( "%d\n", (int) bytes_read );
    }

    if ( ngrams_corpus_parse ( payload, bytes_read, ngrams ) != EXIT_SUCCESS ) {
        fprintf ( stderr, "Cannot parse corpus: %s\n", strerror ( errno ) );
        goto over;
    }

    for ( order = 1; order <= ngrams->max_order; order++ ) {
        printf ( "%d:", (int) order );
        for ( i = 0; i < ngrams->orderset[order - 1].size; i++ )
            printf ( " | %s", ngrams->orderset[order - 1].ngram[i] );
        printf ( "\n" );
    }

    rc = EXIT_SUCCESS;
over:
    if ( ngrams )
        ngrams_free ( ngrams );
    if ( payload )
        free ( payload );

    return rc;
}
