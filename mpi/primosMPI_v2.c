//mpicc para compilar
//mpirun -np 2 para ejecutar con 2 hilos
//Instalar mpi sudo apt install mpich

//#include <mpi.h>
#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NEW_NUMBER 0
#define NEW_PRIME 1
#define NEXT_NUMBER 2
#define FINISH 3

void generador();
void verificador();
void recolector(int request);
void saveResult(int* result, int count);


int main(int argc, char *argv[]) {

    clock_t start, stop;
    int request = atoi(argv[1]);

    int rank, size, error;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(size != 3){
        printf("Necesitas 3 procesos\n");
        return 0;
    }


    //barrier
    if(rank == 0){
        generador();
    }
    if(rank == 1){
        verificador();
    }
    if(rank == 2){
        recolector(request);
    }
    //barrier

//    printf("Hello world! I am %d out of %d.\n",rank,size);
    MPI_Finalize();
    return 0;
}

void generador(){
    int element = 2;
    int finish = 0;
    int message[2];
    message[0] = NEW_NUMBER;
    message[1] = element;
    MPI_Bcast(message, 2, MPI_INT, 0, MPI_COMM_WORLD);

    for(int i = 3; !finish; i = i + 2){
        do {
            if(!message[0] == NEW_NUMBER){
                MPI_Bcast(message, 2, MPI_INT, 0, MPI_COMM_WORLD);
            }

//            printf("Mensaje generador %d - %d\n", message[0], message[1]);
        }while(message[0] != NEXT_NUMBER && message[0] != FINISH);
        if(message[0] == NEXT_NUMBER){
            element = i;
            message[0] = NEW_NUMBER;

            message[1] = element;
            MPI_Bcast(&message, 2, MPI_INT, 0, MPI_COMM_WORLD);
        }
        if(message[0] == FINISH){
            finish = message[1];
        }
    }
    printf("Termino Generador\n");
}
void verificador(){
    int element;
    int dividers = 0;
    int finish = 0;
    int message[2];
    while(!finish){
//        printf("hola desde verificador\n");
        do {
            MPI_Bcast(message, 2, MPI_INT, 0, MPI_COMM_WORLD);

        }while(message[0] != NEW_NUMBER && message[0] != FINISH);
//        printf("Mensaje verificador %d - %d\n", message[0], message[1]);
        if(message[0] == NEW_NUMBER){
            element = message[1];
            for( int i = 1; i <= element; i++){
                if(element % i == 0){
                    dividers++;
                }
            }
            if(dividers == 2){

//                MPI_Send(&element, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                do {
                    message[0] = NEW_PRIME;
                    message[1] = element;
                    MPI_Bcast(message, 2, MPI_INT, 1, MPI_COMM_WORLD);
                    printf("Mensaje verificador %d - %d\n", message[0], message[1]);
                }while(message[0] != NEW_NUMBER);


            }else {
                element = 0;
                message[0] = NEW_PRIME;
                message[1] = element;
//                MPI_Send(&element, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                MPI_Bcast(message, 2, MPI_INT, 1, MPI_COMM_WORLD);
            }
            dividers = 0;
        }
        if(message[0] == FINISH){
            finish = message[1];
        }
    }
    printf("Termino Verificador\n");
}
void recolector(int request){
    int value;
    int message[2];
    int finish = 0;
    int found = 0;
    int* prime_numbers = calloc(request,sizeof(int));

    while(!finish){
        do {
            MPI_Bcast(&message, 2, MPI_INT, 1, MPI_COMM_WORLD);
//            printf("Mensaje recolector %d - %d\n", message[0], message[1]);
        }while(message[0] != NEW_PRIME);
        if(found < request){
            value = message[1];
            if(value != 0){
                prime_numbers[found] = value;
                printf("El numero %d es primo\n",prime_numbers[found]);
                found++;
            }
            message[0] = NEXT_NUMBER;
            do {
                MPI_Bcast(message, 2, MPI_INT, 2, MPI_COMM_WORLD);
            }while(message[0] != NEXT_NUMBER);
        }else{
            finish = 1;
        }
    }

    printf("Termino recolector\n");
    message[0] = FINISH;
    message[1] = finish;
//    do {
//        MPI_Bcast(message, 2, MPI_INT, 2, MPI_COMM_WORLD);
//    }while(message[0] != FINISH);
    MPI_Bcast(message, 2, MPI_INT, 2, MPI_COMM_WORLD);
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


