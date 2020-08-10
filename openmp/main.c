#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int verifyNumber(int number);
int printPrimeNumbers(int count, int* numbers,int checked);

int main() {

    clock_t start, stop;

    /* Lock Flags */
    bool generate_number = false;
    bool verify_number = true;
    bool collect_number = false;

    /* Shared Variables  */
    int generated_number = -1;
    int numbers_tested = 0;
    int request = 0;
    int prime_numbers_count = 0;

    printf("Ingrese la cantidad de numeros primos que quiere buscar: ");
    scanf("%d",&request);
    int* prime_numbers = calloc(request,sizeof(int));


    start = clock();

    #pragma omp parallel default(shared)
    {
        #pragma omp sections
        {
            /* Number Generator */
            #pragma omp section
            {
                while(prime_numbers_count < request){ /* Program not finished */

                    while(!generate_number); /* Waiting for next number request */

                    if(prime_numbers_count == request) /* Program Finished */
                        break;

                    generated_number += 2; /* Next Odd Number */
                    generate_number = false; /* Lock next cycle */
                }
            }

            /* Verifier */
            #pragma omp section
            {
                while(prime_numbers_count < request){ /* Program not finished */

                    while(!verify_number); /* Waiting for next number request */
                    verify_number = false; /* Lock next cycle */

                    if(prime_numbers_count == request) /* Program Finished */
                        break;

                    generate_number = true; /* Unlock Number Generator Thread */

                    while(generate_number); /* Wait until next number is generated */

                    numbers_tested++;

                    if(verifyNumber(generated_number) != 0){ /* is a prime number */
                        /* Collect Number */
                        collect_number = true;
                    }else{
                        /* Ignore generated number */
                        verify_number = true;
                    }
                }
            }

            /* Collector */
            #pragma omp section
            {
                while(prime_numbers_count < request){ /* Program not finished */

                    while(!collect_number); /* Waiting for next number request */
                    collect_number = false; /* Lock next cycle */

                    prime_numbers[prime_numbers_count] = generated_number;  /* Store prime number */
                    prime_numbers_count++;
                    verify_number = true; /* Unlock Verifier Thread */

                    if(prime_numbers_count == request){
                        printPrimeNumbers(prime_numbers_count,prime_numbers,numbers_tested);
                        generate_number = true;
                        break;
                    } /* Unlock all threads */
                }
            }
        }
    }

    stop = clock();

    printf("Tiempo de ejecucion: %fs\n",(stop-start)/(double)CLOCKS_PER_SEC);

    return 0;
}

int verifyNumber(int number){
    int dividers = 0;
    #pragma omp parallel default(shared)
    {
        #pragma omp for
            for(int i = 1; i <= number; i++){
                if(number%i == 0){
                    dividers++;
                }
            }
    }

    if(dividers == 2){
        return number;
    }

    return 0;
}

int printPrimeNumbers(int count, int* nubmers,int checked){
    for(int i = 0; i < count; i++)
        printf("El numero %d es primo\n",nubmers[i]);

    printf("Numero Analizados %d\n",checked);
}