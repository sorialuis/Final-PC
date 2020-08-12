#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

typedef struct Cursor{
    int prime_number;
    struct Cursor* next;

}Cursor_t;

int nextNumber(int* lastNumber);
int verifyNumber(int number);
int printPrimeNumbers(Cursor_t* numbers,int count,int checked,double elapsedTime);
void saveResult(Cursor_t* numbers, int count,int checked,double elapsedTime);

/* function prototypes */
void MergeSort(Cursor_t** headRef);
Cursor_t* SortedMerge(Cursor_t* a, Cursor_t* b);
void FrontBackSplit(Cursor_t* source,
                    Cursor_t** frontRef, Cursor_t** backRef);

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
    Cursor_t *found_numbers = NULL;
    Cursor_t *head = NULL;

    start = omp_get_wtime();

    #pragma omp parallel default(shared) private(next)
    {
        while (prime_numbers_count < request){

            next = nextNumber(&generated_number);

            if(verifyNumber(next) != 0){

                #pragma omp critical
                {
                    if(found_numbers == NULL){
                        found_numbers = calloc(1,sizeof(Cursor_t));
                        head = found_numbers;
                        found_numbers->prime_number = next;
                    }else{
                        found_numbers->next = (Cursor_t*) calloc(1,sizeof(Cursor_t));
                        found_numbers = found_numbers->next;
                        found_numbers->prime_number = next;
                    }
                    prime_numbers_count++;
                }
            }

            #pragma omp critical
            numbers_tested++;
        }
    }

    stop = omp_get_wtime();

    double elapsedTime = (stop-start);

    MergeSort(&head);
    printPrimeNumbers(head,request,numbers_tested,elapsedTime);
    saveResult(head,request,numbers_tested,elapsedTime);

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
            if(dividers < 3){
                if(number%i == 0){
                    dividers++;
                }
            }
        }
    }

    if(dividers == 2){
        return number;
    }

    return 0;
}

int printPrimeNumbers(Cursor_t* numbers,int count,int checked,double elapsedTime){
    Cursor_t *aux = numbers;
    int i;
    for(i = 0; i < count; i++){
        printf("El numero %d es primo\n",aux->prime_number);
        aux = aux->next;
    }

    printf("Numero Analizados %d\n",checked);
    printf("Primos Encontrados %d\n",count);
    printf("Tiempo de ejecucion: %fs\n",elapsedTime);

}

void saveResult(Cursor_t* numbers, int count,int checked,double elapsedTime){
    Cursor_t *aux = numbers;
    FILE* file;
    file = fopen("result.txt", "wt");
    for (int i = 0; i < count; i++) {
        fprintf(file,"%d\n", aux->prime_number);
        aux = aux->next;
    }
    fprintf(file,"Numeros Analizados: %d\n",checked);
    fprintf(file,"Tiempo Transcurrido: %fs\n",elapsedTime);
    fclose(file);
}


/* https://www.geeksforgeeks.org/c-program-for-merge-sort-for-linked-lists/ */
/* sorts the linked list by changing next pointers (not data) */
void MergeSort(Cursor_t** headRef)
{
    Cursor_t* head = *headRef;
    Cursor_t* a;
    Cursor_t* b;

    /* Base case -- length 0 or 1 */
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    /* Split head into 'a' and 'b' sublists */
    FrontBackSplit(head, &a, &b);

    MergeSort(&a);
    MergeSort(&b);

    /* answer = merge the two sorted lists together */
    *headRef = SortedMerge(a, b);
}

/* See https:// www.geeksforgeeks.org/?p=3622 for details of this
function */
Cursor_t* SortedMerge(Cursor_t* a, Cursor_t* b)
{
    Cursor_t* result = NULL;

    /* Base cases */
    if (a == NULL)
        return (b);
    else if (b == NULL)
        return (a);

    /* Pick either a or b, and recur */
    if (a->prime_number <= b->prime_number) {
        result = a;
        result->next = SortedMerge(a->next, b);
    }
    else {
        result = b;
        result->next = SortedMerge(a, b->next);
    }
    return (result);
}

/* UTILITY FUNCTIONS */
/* Split the nodes of the given list into front and back halves,
    and return the two lists using the reference parameters.
    If the length is odd, the extra node should go in the front list.
    Uses the fast/slow pointer strategy. */
void FrontBackSplit(Cursor_t* source,
                    Cursor_t** frontRef, Cursor_t** backRef)
{
    Cursor_t* fast;
    Cursor_t* slow;
    slow = source;
    fast = source->next;

    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    /* 'slow' is before the midpoint in the list, so split it in two
    at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}