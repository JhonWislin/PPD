/*
    This program solves Laplace's equation on a regular 2D grid using simple Jacobi iteration.

    The stencil calculation stops when  ( err >= CONV_THRESHOLD OR  iter > ITER_MAX )
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define ITER_MAX 3000 // number of maximum iterations
#define CONV_THRESHOLD 1.0e-5f // threshold of convergence

int num_threads;
// matrix to be solved
double **grid;

// auxiliary matrix
double **new_grid;

// size of each side of the grid
int size;

// return the maximum value
double max(double a, double b){
    if(a > b)
        return a;
    return b;
}

// return the absolute value of a number
double absolute(double num){
    if(num < 0)
        return -1.0 * num;
    return num;
}

// allocate memory for the grid
void allocate_memory(){
    grid = (double **) malloc(size * sizeof(double *));
    new_grid = (double **) malloc(size * sizeof(double *));

    for(int i = 0; i < size; i++){
        grid[i] = (double *) malloc(size * sizeof(double));
        new_grid[i] = (double *) malloc(size * sizeof(double));
    }
}

// initialize the grid
void initialize_grid(){
    // seed for random generator
    srand(10);

    int linf = size / 2;
    int lsup = linf + size / 10;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            // inicializa região de calor no centro do grid
            if ( i>=linf && i < lsup && j>=linf && j<lsup)
                grid[i][j] = 100;
            else
               grid[i][j] = 0;
            new_grid[i][j] = 0.0;
        }
    }
}

// save the grid in a file
void save_grid(){

    char file_name[30];
    sprintf(file_name, "grid_laplace_pth.txt");

    // save the result
    FILE *file;
    file = fopen(file_name, "w");

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            fprintf(file, "%lf ", grid[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

double *erro_global;

void* jacobi_iteration(void* arg);

void* atualiza_grid(void* arg);

int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Usage: ./laplace_pth N T\n");
        printf("N: The size of each side of the domain (grid)\n");
        printf("T: Number of threads\n");
        exit(-1);
    }

    // variables to measure execution time
    struct timeval time_start;
    struct timeval time_end;

    size = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    pthread_t threads[num_threads];

    // store each thread ID
    int id_thread[num_threads];

    // allocate memory to the error vector
    erro_global = (double *) malloc(num_threads * sizeof(double));

    // allocate memory to the grid (matrix)
    allocate_memory();

    // set grid initial conditions
    initialize_grid();

    double err = 1.0;
    int iter = 0;

    //printf("Jacobi relaxation calculation: %d x %d grid\n", size, size);

    // get the start time
    gettimeofday(&time_start, NULL);

    // Jacobi iteration
    // This loop will end if either the maximum change reaches below a set threshold (convergence)
    // or a fixed number of maximum iterations have completed
    while ( err > CONV_THRESHOLD && iter <= ITER_MAX ) {

        err = 0.0;

        for(int i = 0; i < num_threads; i++){
            id_thread[i] = i;
            pthread_create(&threads[i], NULL, jacobi_iteration, (void *) &id_thread[i]);
        }

        for(int i = 0; i < num_threads; i++){
            pthread_join(threads[i], NULL);
            err = max(err, erro_global[i]);
        }

        for(int i = 0; i < num_threads; i++){
            id_thread[i] = i;
            pthread_create(&threads[i], NULL, atualiza_grid, (void *) &id_thread[i]);
        }

        for(int i = 0; i < num_threads; i++){
            pthread_join(threads[i], NULL);
        }
        iter++;
    }

    /*for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            printf("%lf ", grid[i][j]);
        }
        printf("\n");
    }*/

    // get the end time
    gettimeofday(&time_end, NULL);

    double exec_time = (double) (time_end.tv_sec - time_start.tv_sec) +
                       (double) (time_end.tv_usec - time_start.tv_usec) / 1000000.0;

    //save the final grid in file
    save_grid();

    printf("\nKernel executed in %lf seconds with %d iterations and error of %0.10lf\n", exec_time, iter, err);

    return 0;
}

//Função que cada thread executa para calcular a iteração de Jacobi
void* jacobi_iteration(void* args){
   
    int id = *(int *) args;
    int chuck = (size - 2) / num_threads;
    int initial_row = id * chuck;
    int final_row = initial_row + chuck;
    double err_local = 0.0;
    
    if(id == num_threads - 1){
        final_row = size-2;
    }

    for(int i = initial_row+1; i <= final_row; i++){
        for(int j = 1; j < size - 1; j++){
            new_grid[i][j] = 0.25 * (grid[i][j+1] + grid[i][j-1] +
                                         grid[i-1][j] + grid[i+1][j]);

            err_local = max(err_local, absolute(new_grid[i][j] - grid[i][j]));
        }
    }

    // update the grid
    /*for(int i = initial_row+1; i <= final_row; i++){
        for(int j = 1; j < size - 1; j++){
            grid[i][j] = new_grid[i][j];
        }
    }*/

    erro_global[id] = err_local;

    //pthread_exit(NULL);
}

//Função que cada thread executa para atualizar o grid
void* atualiza_grid(void* args){
    int id = *(int *) args;
    int chuck = (size - 2) / num_threads;
    int initial_row = id * chuck;
    int final_row = initial_row + chuck;
    
    if(id == num_threads - 1){
        final_row = size-2;
    }

    for(int i = initial_row+1; i <= final_row; i++){
        for(int j = 1; j < size - 1; j++){
            grid[i][j] = new_grid[i][j];
        }
    }

    pthread_exit(NULL);
}