#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int nextNumber(int* lastNumber);
int verifyNumber(int number);
int printPrimeNumbers(int* numbers,int count,int checked,double elapsedTime);
void saveResult(int* result, int count,int checked,double elapsedTime);

int main() {
    double start, stop;
    printf("Max Threads %d\n",omp_get_max_threads());
    omp_set_num_threads(12);

    /* Shared Variables  */
    int generated_number = -1;
    int numbers_tested = 0;
    int request = 0;
    int prime_numbers_count = 0;
    int next;

    printf("Ingrese la cantidad de numeros primos que quiere buscar: ");
    scanf("%d",&request);
    int* prime_numbers = calloc(request,sizeof(int));

    start = omp_get_wtime();

    #pragma omp parallel default(shared) private(next)
    {
        while (prime_numbers_count < request){

            next = nextNumber(&generated_number);

            if(verifyNumber(next) != 0){

                #pragma omp critical
                {
                    if(prime_numbers_count < request){
                        prime_numbers[prime_numbers_count] = next;
                        prime_numbers_count++;
                    }
                }
            }

            #pragma omp critical
                numbers_tested++;
        }
    }

    stop = omp_get_wtime();

    double elapsedTime = (stop-start);
    printPrimeNumbers(prime_numbers,prime_numbers_count,numbers_tested,elapsedTime);
    saveResult(prime_numbers,prime_numbers_count,numbers_tested,elapsedTime);

    return 0;
}

int nextNumber(int* lastNumber){
    int aux;
    #pragma omp critical
    {
        if(*lastNumber == -1)
            *lastNumber = 2;
        else if (*lastNumber == 2){
            *lastNumber = 3;
        }else{
            *lastNumber += 2;
        }
        aux = *lastNumber;
    }
    return aux;
}

int verifyNumber(int number){
    int dividers = 0;
    int i;
    #pragma omp parallel default(shared) private(i) reduction(+:dividers)
    {
        #pragma omp for
        for(i = 1; i <= number; i++){
            if(number%i == 0){
                dividers++;
                if(dividers > 2)
                    i = number;
            }
        }
    }

    if(dividers == 2){
        return number;
    }

    return 0;
}

int printPrimeNumbers(int* nubmers,int count,int checked,double elapsedTime){
    int i;
    for(i = 0; i < count; i++)
        printf("El numero %d es primo\n",nubmers[i]);

    printf("Numero Analizados %d\n",checked);
    printf("Tiempo de ejecucion: %fs\n",elapsedTime);

}

void saveResult(int* result, int count,int checked,double elapsedTime){
    FILE* file;
    file = fopen("result.txt", "wt");
    for (int i = 0; i < count; i++) {
        fprintf(file,"%d\n", result[i]);
    }
    fprintf(file,"Numeros Analizados: %d\n",checked);
    fprintf(file,"Tiempo Transcurrido: %fs\n",elapsedTime);
    fclose(file);
}
