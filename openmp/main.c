#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int verifyNumber(int number);
int* collectResults();

int main() {
    int request_count = 100;
    int numbers_found = 0;
    int numbers_checked = 0;
    int res = 0;

    printf("Ingrese la cantidad de numeros primos que quiere buscar: ");
    scanf("%d",&request_count);
    int* found = calloc(request_count,sizeof(int));

    int waiting_request = 1;

    int generated_number = -1;
    int number_generated = 0;

    int collect_numbers = 0;
    int number_collected = 0;

    #pragma omp parallel default(shared) private(res)
    {
        #pragma omp sections nowait
        {
            /* Generador */
            #pragma omp section
            {
                while(numbers_found < request_count){

                    while(waiting_request || (numbers_found >= request_count));
                    if(numbers_found >= request_count)
                        break;
                    generated_number = generated_number + 2;
                    number_generated = 1;
                    waiting_request = 1;

                }
            }

            /* Verificador */
            #pragma omp section
            {
                while(numbers_found < request_count){
                    number_generated = 0;
                    waiting_request = 0;

                    while(!number_generated|| (numbers_found >= request_count));
                    if(numbers_found >= request_count)
                        break;

                    numbers_checked++;
                    res = verifyNumber(generated_number);

                    if(res != 0){
                        #pragma omp critical
                            collect_numbers = 1;

                        while (!number_collected);

                        #pragma omp critical
                        collect_numbers = 0;
                        numbers_found++;
                    }
                }
            }

            /* Recolector */
            #pragma omp section
            {
                while(numbers_found < request_count){
                    while(!collect_numbers);
                    printf("Collected Numbmr: %d\n",generated_number);
                    #pragma omp critical
                    {
                        number_collected = 1;
                    }

                }
            }

        }


    }

    return 0;
}

int verifyNumber(int number){
    int dividers = 0;
    #pragma omp parallel
    {
        #pragma omp for
            for(int i = 1; i <= number; i++){
                if(number%i == 0){
                    dividers++;
                }
            }
    }

    if(dividers == 2){
        printf("El numero %d es primo\n",number);
        return number;
    }

    return 0;
}