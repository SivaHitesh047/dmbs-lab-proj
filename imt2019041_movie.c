#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "movie.h"

// Load all the movies from a CSV file
int store_movies( char *movie_data_file )
{
	FILE *mfptr;
	char movie_line[500], token;
	struct Movie m, dummy;

	mfptr = (FILE *) fopen(movie_data_file, "r");
	while(fgets(movie_line, sizeof(movie_line)-1, mfptr)){
		//printf("line:%s",movie_line);
		sscanf(movie_line, "%d%s%s%f%f", &(m.movie_id),m.movie_name,m.genre,&(m.rating),&(m.runtime_in_mins));
		print_movie( &m );
		add_movie( &m );
	}
}

void print_movie( struct Movie *m )
{
	printf("%d,%s,%s,%f,%f\n", m->movie_id,m->movie_name,m->genre,m->rating,m->runtime_in_mins);
}

// Use get_rec_by_key function to retrieve movie
int search_movie( int movie_id, struct Movie *m )
{
	return get_rec_by_ndx_key( movie_id, m );
}

// Add the given movie into the repository by calling put_rec_by_key
int add_movie( struct Movie *m)
{
	int status;

	status = put_rec_by_key( m->movie_id, m );

	if( status != PDS_SUCCESS ){
		fprintf(stderr, "Unable to add movie with key %d. Error %d", m->movie_id, status );
		return MOVIE_FAILURE;
	}
	return status;
}

// Use get_rec_by_non_ndx_key function to retrieve movie
// Hint: get_rec_by_non_ndx_key( phone, c, &match_movie_phone, io_count );
int search_movie_by_name( char *name, struct Movie *m, int *io_count )
{
	// Call function
	get_rec_by_non_ndx_key( name, m, &match_movie_name, io_count );
}

/* Return 0 if phone of the movie matches with phone parameter */
/* Return 1 if phone of the movie does NOT match */
/* Return > 1 in case of any other error */
int match_movie_name( void *rec, void *key )
{
	// Store the rec in a struct movie pointer
	struct Movie *mPtr = rec;
	// Store the key in a char pointer
	char *key_dup = key;

	if(mPtr==NULL || key_dup==NULL)
		return 2;

	// Compare the name values in key and record
	// Return 0,1,>1 based on above condition
	if(strcmp(key_dup,mPtr->movie_name)==0)
		return 0;
	else
		return 1;
}

// Function to delete a record based on ndx_key
int delete_movie ( int movie_id )
{
	// Call the delete_rec_by_ndx_key function
	int d = delete_rec_by_ndx_key(movie_id);
	// Return MOVIE_SUCCESS or MOVIE_FAILURE based on status of above call
	if(d==PDS_SUCCESS)
		return MOVIE_SUCCESS;
	else
		return MOVIE_FAILURE;
}

int overwrite_movie( struct Movie *m )
{
	int mErr;
	mErr = pds_overwrite(m->movie_id, m);

	if(mErr==PDS_SUCCESS)
		return MOVIE_SUCCESS;
	else
		return MOVIE_FAILURE;
}