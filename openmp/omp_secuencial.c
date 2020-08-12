#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int verifyNumber(int number);
int printPrimeNumbers(int* numbers,int count,int checked,double elapsedTime);
void saveResult(int* result, int count,int checked,double elapsedTime);

int main() {
    clock_t start, stop;

    /* Shared Variables  */
    int generated_number = -1;
    int numbers_tested = 0;
    int request = 0;
    int prime_numbers_count = 0;

    printf("Ingrese la cantidad de numeros primos que quiere buscar: ");
    scanf("%d",&request);
    int* prime_numbers = calloc(request,sizeof(int));

    start = clock();

    while (prime_numbers_count < request){
        if(generated_number == -1)
            generated_number = 2;
        else if (generated_number == 2){
            generated_number = 3;
        }else{
            generated_number += 2;
        }

        if(verifyNumber(generated_number) != 0){
            prime_numbers[prime_numbers_count] = generated_number;
            prime_numbers_count++;
        }

        numbers_tested++;
    }

    stop = clock();

    double elapsedTime = (stop-start)/(double)CLOCKS_PER_SEC;
    printPrimeNumbers(prime_numbers,prime_numbers_count,numbers_tested,elapsedTime);
    saveResult(prime_numbers,prime_numbers_count,numbers_tested,elapsedTime);

    return 0;
}


int verifyNumber(int number){
    int dividers = 0;
    int i;

    for(i = 1; i <= number; i++){
        if(number%i == 0){
            dividers++;

            if(dividers > 2)
                return 0;
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
