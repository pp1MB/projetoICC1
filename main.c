#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void AV(void);
void RR(void);

typedef struct {
    char *key;
    void (*func)();
} comando;

int main(void){
    char inputComando[2]; // Fazer alocação dinâmica ?

    comando options[] = {
        {"AV", AV},
        {"RR", RR},
        {NULL, NULL} // Posteriormente dá pra remover isso e colocar i<n_comandos possíveis
    };

    scanf("%s", inputComando); // Pede o comando ao usuário

    // Esse trecho pega uma lista de comandos e compara com o "inputComando", ao achar um comando correspondente ele chama a função desse comando.
    for(int i=0; options[i].key != NULL; i++){
        if(strcmp(inputComando, options[i].key) == 0){
            options[i].func();
        }
    }

    return 0;
}

void AV(void){
    printf("AV");
}

void RR(void){
    printf("RR");
}

