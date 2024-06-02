/*
 * The one that I used for the other lesson
 */

#ifdef _OPENMP
#include <omp.h>
#endif /* ifdef _OPENMP */
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

int main (int argc, char *argv[]) 
{
#ifdef _OPENMP
	FILE *pFile;
	long file_size, match_size, pattern_size, total_matches;
	char * buffer;
	char * filename, *pattern;
	size_t result;
	int i, j, *match, th_num;

    if (argc != 4) 
    {
		printf ("Usage : %s <file_name> <string> <th_num>\n", argv[0]);
		return 1;
    }

	filename = argv[1];
	pattern = argv[2];
    th_num = strtol(argv[3], NULL, 10);
	
    omp_set_num_threads(th_num);
    omp_set_nested(1);

	pFile = fopen ( filename , "rb" );
	if (pFile==NULL) {printf ("File error\n"); return 2;}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	file_size = ftell (pFile);
	rewind (pFile);
	
	// allocate memory to contain the file:
	buffer = (char*) malloc (sizeof(char)*file_size);
	if (buffer == NULL) {printf ("Memory error\n"); return 3;}

	// copy the file into the buffer:
	result = fread (buffer,1,file_size,pFile);
	if (result != file_size) {printf ("Reading error\n"); return 4;} 
	
	pattern_size = strlen(pattern);
	match_size = file_size - pattern_size + 1;
	
	match = (int *) calloc (match_size, sizeof(int));
	if (match == NULL) {printf ("Malloc error\n"); return 5;}
	
	total_matches = 0;
 
    double start = omp_get_wtime();
    #pragma omp parallel for default(shared) private(i, j) reduction(+: total_matches)
        /* Brute Force string matching */
        for (j = 0; j < match_size; ++j)
        {   
            // #pragma omp task
            for (i = 0; i < pattern_size && pattern[i] == buffer[i + j]; ++i);
                if (i >= pattern_size) 
                {
                    match[j] = 1;
                    total_matches++;
                }		
        }
    double end = omp_get_wtime();

    // That is useless for big files 
	// for (j = 0; j < match_size; j++)
	// 	printf("%d", match[j]);

    printf("Time: %f", end - start);
    printf("\nTotal matches = %ld\n", total_matches);

	fclose (pFile);
	free (buffer);
	free (match);
#endif /* ifdef _OPENMP */	

	return EXIT_SUCCESS;
}
