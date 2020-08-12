//mpicc para compilar
//mpirun -np 2 para ejecutar con 2 hilos
//Instalar mpi sudo apt install mpich

//#include <mpi.h>
#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PROC_GENERATE 0
#define PROC_RECOLECTOR 1
#define PROC_VERIFIED 2


void generador();
void verificador();
void recolector(int request);
void saveResult(int* result, int count);

int main(int argc, char *argv[]) {

    double start_time,end_time;
    int request = atoi(argv[1]);



    int rank, size, error;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(size < 3){
        if(rank == 0){
            printf("Se necesita al menos 3 procesos (Generador, Recolector y la menos 1 Verificador)\n");
        }
        return 0;
    }

    if(rank == 0){
        start_time = MPI_Wtime();
        generador();
    }
    if(rank == 1){
        recolector(request);
    }
    if(rank >= 2){
        verificador();
    }

    if(rank == 0){
        end_time = MPI_Wtime();
//        printf("Se tardo %.2lf seconds\n",end_time-start_time);
        printf("Se tardo %f seconds\n",end_time-start_time);

    }
    MPI_Finalize();
    return 0;
}

void generador(){
    MPI_Status status;
    MPI_Request found_request;
    int finish = 0;
    int cores;
    MPI_Comm_size(MPI_COMM_WORLD,&cores);
    int idVerificador = 0;
    /*Envio el primer numero primo al verificador que si o si tengo*/
    int element = 2;
    MPI_Send(&element, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);


    for(int i = 3; !finish; i = i + 2){
        do {
            /*Escucha que un verificador para enviarle un dato*/
            MPI_Irecv(&idVerificador, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &found_request);
            MPI_Wait(&found_request, &status);
        }while(idVerificador == 0);
        element = i;
        MPI_Send(&element, 1, MPI_INT, idVerificador, 0, MPI_COMM_WORLD);
        /*Verifica que el recolector no termino la tarea*/
        MPI_Recv(&finish, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD, &status);
    }
    element = 0;
    /*Para terminar los procesos verificadores les envio un 0 para cortar*/
    for(int i = 2; i < cores; i++){
        MPI_Isend(&element, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &found_request);
    }
    printf("Termino Generador\n");
}
void verificador(){
    MPI_Status status;
    MPI_Request found_request;
    int element;
    int dividers = 0;
    int finish = 0;
    int myId;
    MPI_Comm_rank(MPI_COMM_WORLD,&myId);
    while(!finish){
        /*Envia su ID al generador*/
        MPI_Isend(&myId,1,MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD, &found_request);
        MPI_Wait(&found_request,&status);
        /*Recive un dato del generador si es 0 se termina sino verifica el numero*/
        MPI_Recv(&element, 1, MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD, &status);
        if(element == 0){
            finish = 1;
            break;
        }
        for( int i = 1; i <= element; i++){
            if(element % i == 0){
                dividers++;
            }
        }

        /*Envia 0 si es primo para contar en el recolector y si es primo manda el valor*/
        if(dividers == 2){

            MPI_Send(&element, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD);
        }else {
            element = 0;
            MPI_Send(&element, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD);
//            MPI_Isend(&element, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD, &found_request);
        }
        dividers = 0;
    }
    printf("Termino Verificador\n");
}
void recolector(int request){
    int value = -1;
    int finish = 0;
    int cores;
    MPI_Status status;
    MPI_Request found_request;
    MPI_Comm_size(MPI_COMM_WORLD,&cores);
    int found = 0;
    int* prime_numbers = calloc(request,sizeof(int));
    int count;

    while(found < request){
        do {
            /*Espera que los verificadores le envian informacion*/
            MPI_Irecv(&value, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &found_request);
            MPI_Wait(&found_request, &status);
        }while(value == -1);
        count++;
        if(value != 0){
            prime_numbers[found] = value;
            printf("El numero %d es primo\n",prime_numbers[found]);
            found++;
        }
        MPI_Send(&finish, 1, MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD);
        value = -1;
    }

    /*Cuando termina envia finish al generador*/
    printf("Se encontro los %d primos en %d verificaciones\n",request, count);
    finish = 1;
    MPI_Send(&finish, 1, MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD);
    printf("Termino recolector\n");
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


