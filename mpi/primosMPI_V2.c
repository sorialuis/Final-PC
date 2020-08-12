# include <math.h>
# include <mpi/mpi.h>
# include <stdio.h>
# include <stdlib.h>
# include <time.h>

int verificar(int element);
void saveResult(int* result, int count);

int main(int argc, char *argv[]){
    int rank, size, error;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    int request = atoi(argv[1]);
    int* prime_numbers = calloc(request,sizeof(int));
    int* generator_data = calloc(size,sizeof(int));
    int finish = 0;
    int generados = 0;
    int element = 0;
    int prime_number_count = 0;
    int total = 0;
    double start_time,end_time;

    start_time = MPI_Wtime();//    printf("Hello world! I am %d out of %d.\n",rank,size);

    while(!finish){
        /*Generador*/
        if(rank == 0){
            for (int i = 0; i < size ;i++){
                if(generados != 0){
                    generator_data[i] = generados;
                    generados = generados + 2;
                }else{
                    generator_data[i] = 2;
                    generados = 3;
                }

            }
        }
        MPI_Scatter(generator_data,1, MPI_INT,&element,1, MPI_INT, 0, MPI_COMM_WORLD);

        /*Verificar*/
        element = verificar(element);
        MPI_Gather(&element, 1, MPI_INT, generator_data, 1, MPI_INT, 0, MPI_COMM_WORLD);

        /*Recoleccion*/
        if(rank == 0){
            for(int i = 0; i < size; i++){
                total++;
                if(generator_data[i] != 0){
                    prime_numbers[prime_number_count] = generator_data[i];
                    prime_number_count++;
                }
                if(prime_number_count == request){
                    finish = 1;
                    i = size;
                }
            }
        }
        MPI_Bcast(&finish, 1, MPI_INT, 0,MPI_COMM_WORLD);
    }
    end_time=MPI_Wtime();
    if(rank == 0){
        for(int i = 0; i < request; i++){
            printf("Primo encontrado = %d\n", prime_numbers[i]);
        }
        printf("Se encontraron los %d primos en %d verificaciones\n", request, total);
        printf("en %.2lf seconds\n",end_time-start_time);
        saveResult(prime_numbers, request);
    }
    MPI_Finalize();

    return 0;
}

int verificar(int element){
    int dividers = 0;
    for( int i = 1; i <= element; i++){
        if(element % i == 0){
            dividers++;
        }
        if(dividers > 2){
            return 0;
        }
    }
    if(dividers == 2){
        return element;
    }
    return 0;
}

void saveResult(int* result, int count){
    FILE* file;
    file = fopen("result.txt", "wt");
    for (int i = 0; i < count; i++) {
        fprintf(file,"%d\n", result[i]);
    }
    printf("Datos Guardados\n");
    fclose(file);
}