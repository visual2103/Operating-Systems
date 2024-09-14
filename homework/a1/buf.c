#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int x;
    int y;
}a;

typedef struct{
    int q;
    a** cv;
}b;

void print(b * rr){
    for(int i = 0; i < rr->q; i++){
        printf("%d ", rr->cv[i]->x);
    }
    printf("\n");
}

int main(void){
    b* brqq = (b*)malloc(sizeof(b));
    brqq->q = 5;
    brqq->cv = (a**)malloc(sizeof(a*) * 5); 
    for(int i = 0; i < 5; i++){
        brqq->cv[i] = (a*)malloc(sizeof(a));
        brqq->cv[i]->x = i; 
    }

    print(brqq);

    for(int i = 0; i < brqq->q - 1; i++){
        for(int j = 0; j < brqq->q - i - 1; j++){
            if(brqq->cv[j]->x < brqq->cv[j + 1]->x){
                a* aux = brqq->cv[j];
                brqq->cv[j] = brqq->cv[j + 1];
                brqq->cv[j + 1] = aux;
            }
        }
    }

    print(brqq);

    return(0);
}