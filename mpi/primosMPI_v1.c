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
//    printf("inicio\n");
    clock_t start, stop;
    int request = atoi(argv[1]);
//    printf("Ingrese la cantidad de numeros primos que quiere buscar: ");
//    scanf("%d",&request);



    int rank, size, error;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    if(rank == 0){
        //start
        generador();
    }
    if(rank == 1){
        recolector(request);
    }
    if(rank >= 2){
        verificador();
    }

//    printf("Hello world! I am %d out of %d.\n",rank,size);

    if(rank == 0){
        //stop
    }
    MPI_Finalize();
    return 0;
}

void generador(){
    MPI_Status status;
    MPI_Request found_request;
//    int element = 1;
    int finish = 0;
    int cores;
    MPI_Comm_size(MPI_COMM_WORLD,&cores);
    int idVerificador = 0;
    int element = 2;
    MPI_Send(&element, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    for(int i = 3; !finish; i = i + 2){
        do {
            MPI_Irecv(&idVerificador, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &found_request);
            MPI_Wait(&found_request, &status);
        }while(idVerificador == 0);
        element = i;
        MPI_Send(&element, 1, MPI_INT, idVerificador, 0, MPI_COMM_WORLD);
        MPI_Recv(&finish, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD, &status);
        idVerificador = 0;
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
        MPI_Isend(&myId,1,MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD, &found_request);
        MPI_Wait(&found_request,&status);
        MPI_Recv(&element, 1, MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD, &status);
        printf("Verificando = %d\n",element);
        for( int i = 1; i <= element; i++){
            if(element % i == 0){
                dividers++;
            }
        }
        if(dividers == 2){
            MPI_Send(&element, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD);
//            printf("El numero %d es primo soy %d\n",element, myId);
        }else {
            element = 0;
            MPI_Send(&element, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD);
        }

        dividers = 0;

        MPI_Recv(&finish, 1, MPI_INT, PROC_RECOLECTOR, 0, MPI_COMM_WORLD, &status);
//        MPI_Recv(&finish, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
//        printf("finish %d - proc %d - elemento %d\n",finish, myId, element);

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

    while(found < request){

        do {
            MPI_Irecv(&value, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &found_request);
            MPI_Wait(&found_request, &status);
        }while(value == -1);
        if(value != 0){
            prime_numbers[found] = value;
            printf("El numero %d es primo\n",prime_numbers[found]);
            found++;
        }
        MPI_Send(&finish, 1, MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD);
        for(int i = 2; i < cores; i++){
            MPI_Send(&finish, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        value = -1;
    }

    finish = 1;
    MPI_Send(&finish, 1, MPI_INT, PROC_GENERATE, 0, MPI_COMM_WORLD);

    for(int i = 2; i < cores; i++){

        MPI_Send(&finish, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        printf("Cerrando %d proc = %d\n",finish,i);
    }
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


