#ifndef MOVIE_H
#define MOVIE_H

#define MOVIE_SUCCESS 0
#define MOVIE_FAILURE 1
 
struct Movie{
	int movie_id;
	char movie_name[50];
	char genre[50];
	float rating;
	float runtime_in_mins;
};

extern struct PDS_RepoInfo *repoHandle;

// Add the given movie into the repository by calling put_rec_by_key
int add_movie( struct Movie *m );

// Overwite existing movie with the given movie pds_overwrite
// Hint: call the PDS function as follows
// pds_overwrite(c->movie_id, c);
int overwrite_movie( struct Movie *m );

// Display movie info in a single line as a CSV without any spaces
void print_movie( struct Movie *m );

// Use get_rec_by_key function to retrieve movie
int search_movie( int movie_id, struct Movie *m );

// Load all the movies from a CSV file
int store_movies( char *movie_data_file );

// Use get_rec_by_non_ndx_key function to retrieve movie
int search_movie_by_name( char *name, struct Movie *m, int *io_count );

/* Return 0 if name of the movie matches with name parameter */
/* Return 1 if name of the movie does NOT match */
/* Return > 1 in case of any other error */
int match_movie_name( void *rec, void *key );

// Function to delete movie by ID
int delete_movie ( int movie_id );

#endif
