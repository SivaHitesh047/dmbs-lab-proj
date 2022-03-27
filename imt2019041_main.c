#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "movie.h"

int main()
{
    int user_resp,flag1,mv_id,flag2,flag3,flag4,flag5,flag6,flag7,flag8;
    struct Movie m;
    int rec_size = sizeof(struct Movie);
    char rec_name[30] = "mv_database";
    pds_create(rec_name);
    pds_open(rec_name, rec_size);

    while(1)
    {
        printf("\nMovie Database Management System!\n\n");

        printf("What do you want to do? Enter your option:\n\n");
        printf("1. Add a movie to database.\n");
        printf("2. Search a movie by its id.\n");
        printf("3. Search a movie by its name.\n");
        printf("4. Re-enter a movie details.\n");
        printf("5. Show movie details.\n");
        printf("6. Remove a movie from database.\n");
        printf("7. Quit.\n");
        scanf("%d", &user_resp);

        if(user_resp==1)
        {
            printf("Enter the id of the movie you want to add:\n");
            int mv_id;
            scanf("%d",&mv_id);
            
            printf("Enter the name of the movie you want to add:\n");
            scanf("%s",(&m)->movie_name);
            
            printf("Enter the genre of the movie you want to add:\n");
            scanf("%s",(&m)->genre);
            
            printf("Enter the rating of the movie you want to add:\n");
            float mv_rating;
            scanf("%f",&mv_rating);
            
            printf("Enter the runtime of the movie you want to add:\n");
            float mv_runtime;
            scanf("%f",&mv_runtime);

            (&m)->movie_id = mv_id;
            (&m)->rating = mv_rating;
            (&m)->runtime_in_mins = mv_runtime;

            flag5 = add_movie(&m);

            if(flag5==0)
            {
                printf("Movie added successfully!\n");
                print_movie(&m);
            }

            else
                printf(" Movie couldn't added due to an error. Please try again.\n");

        }
        
        if(user_resp==2)
        {
            printf("Enter the id of the movie you want to search:\n");
            scanf("%d",&mv_id);
            flag1 = search_movie(mv_id, &m);
            
            if(flag1 == 0)
            {
                printf("Movie found!\n");
                print_movie(&m);
            }

            else
                printf("Movie not found :(\n");
        }
            
        if(user_resp==3)
        {
            printf("Enter the name of the movie you want to search:\n");
            char mv_name[30];
            int io_count;
            scanf("%s",mv_name);
            flag2 = search_movie_by_name(mv_name,&m, &io_count);

            if(flag2 == 0)
            {
                printf("Movie found!\n");
                print_movie(&m);
            }

            else
                printf(" Movie not found :(\n");

        }

        if(user_resp==4)
        {
            printf("Enter the id of the movie:\n");
            scanf("%d",&mv_id);

            printf("Enter the name of the movie you want to add:\n");
            scanf("%s",(&m)->movie_name);
            
            printf("Enter the genre of the movie you want to add:\n");
            scanf("%s",(&m)->genre);
            
            printf("Enter the rating of the movie you want to add:\n");
            float mv_rating;
            scanf("%f",&mv_rating);
            
            printf("Enter the runtime of the movie you want to add:\n");
            float mv_runtime;
            scanf("%f",&mv_runtime);

            (&m)->movie_id = mv_id;

            (&m)->rating = mv_rating;
            (&m)->runtime_in_mins = mv_runtime;

            flag5 = overwrite_movie(&m);

            if(flag5==0)
                printf("The movie details were updated sucessfully.\n");

            else
                printf(" The movie details couldn't be updated as the movie wasn't found.\n");
           
        }

        if(user_resp==5)
        {
            printf("Enter the id of the movie for which you want to know the details:\n");
            scanf("%d",&mv_id);
            flag4 = search_movie(mv_id, &m);
            
            if(flag4 == 0)
            {
                printf("Movie found!\n");
                printf("Movie id: %d\n",m.movie_id);
                printf("Movie name: %s\n",m.movie_name);
                printf("Movie genre: %s\n",m.genre);
                printf("Movie rating: %f\n",m.rating);
                printf("Movie runtime: %f\n",m.runtime_in_mins);
            }

            else
                printf(" Movie not found :(\n");
        }

        if(user_resp==6)
        {
            printf("Enter the id of the movie you want to delete:\n");
            scanf("%d",&mv_id);
            flag7 = delete_movie(mv_id);
            
            if(flag7==0)
                printf("The Movie was deleted from the database.\n");
            else
                printf(" The Movie couldn't be deleted as it was not found in the database.\n");
            
        }

        if(user_resp==7)
        {
            printf("Later Skater!\n");
            pds_close();
            break;
        }

    }

    return 0;
}