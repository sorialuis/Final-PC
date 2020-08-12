//mpicc para compilar
//mpirun -np 2 para ejecutar con 2 hilos
//Instalar mpi sudo apt install mpich

//#include <mpi.h>
#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PROC_GENERATE 0
#define PROC_VERIFIED 1
#define PROC_RECOLECTOR 2

void generador();
void verificador();
void recolector(int request);
void saveResult(int* result, int count);

int main(int argc, char *argv[]) {
    printf("inicio\n");
    double start_time,end_time;
    int request = atoi(argv[1]);



    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    if(rank == 0){
        start_time = MPI_Wtime();
        generador();
    }
    if(rank == 1){
        verificador();
    }
    if(rank == 2){
        recolector(request);
    }

//    printf("Hello world! I am %d out of %d.\n",rank,size);

    if(rank == 0){
        end_time = MPI_Wtime();
        printf("Se tardo %.2lf seconds\n",end_time-start_time);
    }
    MPI_Finalize();
    return 0;
}

void generador(){
    MPI_Status status;
    int element = 2;
    int finish = 0;
    MPI_Send(&element, 1, MPI_INT, PROC_VERIFIED, 0, MPI_COMM_WORLD);
    for(int i = 3; !finish; i = i + 2){
        element = i;
        MPI_Send(&element, 1, MPI_INT, PROC_VERIFIED, 0, MPI_COMM_WORLD);
        MPI_Recv(&finish, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD, &status);
    }
    printf("Termino Generador\n");
}
void verificador(){
    MPI_Status status;
    int element;
    int dividers = 0;
    int finish = 0;
    while(!finish){
//        printf("hola desde verificador\n");
        MPI_Recv(&element, 1, MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD, &status);
        for( int i = 1; i <= element; i++){
            if(element % i == 0){
                dividers++;
            }
        }

        if(dividers == 2){
            MPI_Send(&element, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD);
        }else {
            element = 0;
            MPI_Send(&element, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD);
        }

        dividers = 0;
        MPI_Recv(&finish, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD, &status);
    }
    printf("Termino Verificador\n");
}
void recolector(int request){
    int value;
    int finish = 0;
    MPI_Status status;
    int found = 0;
    int* prime_numbers = calloc(request,sizeof(int));

    while(found < request){
        MPI_Recv(&value, 1, MPI_INT, PROC_VERIFIED, 0, MPI_COMM_WORLD, &status);
        if(value != 0){
            prime_numbers[found] = value;
            printf("El numero %d es primo\n",prime_numbers[found]);
            found++;
        }
        MPI_Send(&finish, 1, MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD);
        MPI_Send(&finish, 1, MPI_INT, PROC_VERIFIED, 0, MPI_COMM_WORLD);
    }
    printf("Termino recolector\n");
    finish = 1;
    MPI_Send(&finish, 1, MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD);
    MPI_Send(&finish, 1, MPI_INT, PROC_VERIFIED, 0, MPI_COMM_WORLD);
    saveResult(prime_numbers,request);
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
