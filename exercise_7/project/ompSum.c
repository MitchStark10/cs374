/* arraySum.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * Mitch Stark, Fall 2017
 * Updates made to parallelize using OMP
 * for CS 374 (HPC) at Calvin College.
 */

#include <stdio.h>      /* I/O stuff */
#include <stdlib.h>     /* calloc, etc. */
#include <omp.h>

void readArray(char * fileName, double ** a, int * n);
double sumArray(double * a, int numValues) ;

int main(int argc, char * argv[])
{
    int howMany, numThreads;
    double sum, start_program_time, start_io_time, start_sum_time, end_program_time, end_io_time, end_sum_time;
    double * a;

    if (argc != 3) {
        fprintf(stderr, "\n*** Usage: arraySum <inputFile> <numThreads>\n\n");
        exit(1);
    }

    numThreads = atoi(argv[2]);
    omp_set_num_threads(numThreads);

    start_program_time = omp_get_wtime();

    //master reads array
    start_io_time = omp_get_wtime();
    readArray(argv[1], &a, &howMany);
    end_io_time = omp_get_wtime();

    start_sum_time = omp_get_wtime();
    sum = sumArray(a, howMany);
    end_sum_time = omp_get_wtime();

    end_program_time = omp_get_wtime();

    //master prints
    printf("The sum of the values in the input file '%s' is %g\n", argv[1], sum);
    printf("Total Time: %lf\nIO Time: %lf\nSum Time: %lf\n", end_program_time - start_program_time,
            end_io_time - start_io_time, end_sum_time - start_sum_time);

    free(a);

    return 0;
}

/* readArray fills an array with values from a file.
 * Receive: fileName, a char*,
 *          a, the address of a pointer to an array,
 *          n, the address of an int.
 * PRE: fileName contains N, followed by N double values.
 * POST: a points to a dynamically allocated array
 *        containing the N values from fileName
 *        and n == N.
 */

void readArray(char * fileName, double ** a, int * n) {
    int count, howMany;
    double * tempA;
    FILE * fin;

    fin = fopen(fileName, "r");
    if (fin == NULL) {
        fprintf(stderr, "\n*** Unable to open input file '%s'\n\n",
                fileName);
        exit(1);
    }

    fscanf(fin, "%d", &howMany);
    tempA = calloc(howMany, sizeof(double));
    if (tempA == NULL) {
        fprintf(stderr, "\n*** Unable to allocate %d-length array",
                howMany);
        exit(1);
    }

    for (count = 0; count < howMany; count++)
        fscanf(fin, "%lf", &tempA[count]);

    fclose(fin);

    *n = howMany;
    *a = tempA;
}

/* sumArray sums the values in an array of doubles.
 * Receive: a, a pointer to the head of an array;
 *          numValues, the number of values in the array.
 * Return: the sum of the values in the array.
 */

double sumArray(double * a, int numValues) {
    int i;
    double result = 0.0;

    //Need to use #pragma omp parallel for reduction(:+result)
    //Example is in reduction
    //#pragma omp parallel private(id, start_val, chunk_size, partial_sum, i) 
    #pragma omp parallel for reduction(+:result)
    for (i = 0; i < numValues; i++) {
        result += a[i];
    }

    return result;
}

