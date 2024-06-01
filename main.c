#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

/*
// Tecnicamente estou declarando variáveis globais então sla, talvez dê pra mover essa struct pra main. (?)
struct {
    char *code;
    void (*func)();
} comandos[] = {
    {"AV", aberturaVoo},
    {"RR", realizarReserva},
    {"CR", consultarReserva},
    {"MR", modificarReserva},
    {"CA", cancelarReserva},
    {"FD", fechamentoDia},
    {"FV", fechamentoVoo}
};
*/

typedef struct{
    int qtdAssentos;
    float valEco;
    float valExe;
} voo;

// Estrutura para informações do passageiro.
typedef struct{
    char *nome;
    char *sobrenome;
    char *cpf;
    char assento[2];
    bool classe;
} passageiros;

void *alocarMemoria(int size_vet, int size_type);

voo aberturaVoo(void);
passageiros realizarReserva(void);
void consultarReserva(passageiros *p, voo v, int n_passageiros);
void modificarReserva(void);
void cancelarReserva(void);
void fechamentoDia(void);
void fechamentoVoo(void);

int main(void){
    char inputComando[2]; 
    int n_passageiro = 0;
    voo viagem; // Eu juro que não sei um nome de variável decente, será que pode usar "voo voo" ???
    passageiros *passageiro;

    while(1){

        scanf("%s", inputComando); // Pede o comando ao usuário

        if(strcmp(inputComando, "AV") == 0){
            viagem = aberturaVoo();
            passageiro = (passageiros *) alocarMemoria(viagem.qtdAssentos, sizeof(passageiros)); // Mesmo problema com nome de variável
        }

        if(strcmp(inputComando, "RR") == 0){
            passageiro[n_passageiro] = realizarReserva();
            n_passageiro++;
        }

        if(strcmp(inputComando, "CR") == 0){
            consultarReserva(passageiro, viagem, n_passageiro);
        }
    }

    /*
    // Esse trecho pega uma lista de comandos e compara com o "inputComando", ao achar um comando correspondente ele chama a função desse comando.
    for(int i=0; i < 7; i++){
        if(strcmp(inputComando, comandos[i].code) == 0){
            comandos[i].func();
        }
    }
    */

    return 0;
}

void *alocarMemoria(int size_vet, int size_type){
    void *vet;
    vet = malloc(size_vet * size_type);

    assert(vet != NULL); // Crashar o programa parece ser meio drástico, podemos trabalhar melhor nisso.
}

voo aberturaVoo(void){
    voo v; 

    scanf("%d", &v.qtdAssentos);
    scanf("%f", &v.valEco);
    scanf("%f", &v.valExe);

    return v;
}

passageiros realizarReserva(void){
    passageiros p;
    char input[200]; // 200 é suficiente?

    // Não sei se é melhor separar uma string pra nome e outra pra sobrenome ou fazer uma lógica para pular o primeiro espaço e armazenar tudo em uma string.
    scanf(" %[^\n]s", input);

    char *token = strtok(input, " ");
    p.nome = (char *) alocarMemoria(strlen(token)+1, sizeof(char));
    strcpy(p.nome, token);
    printf("%s", p.nome);

    token = strtok(NULL, " ");
    p.sobrenome = (char *) alocarMemoria(strlen(token)+1, sizeof(char));
    strcpy(p.sobrenome, token);
    printf("%s", p.sobrenome);

    token = strtok(NULL, " ");
    p.cpf = (char *) alocarMemoria(strlen(token)+1, sizeof(char));
    strcpy(p.cpf, token);
    printf("%s", p.cpf);

    token = strtok(NULL, " ");
    strcpy(p.assento, token);
    printf("%s", p.assento);

    token = strtok(NULL, " ");
    if(strcmp(token, "economica") == 0) p.classe = false;
    if(strcmp(token, "executiva") == 0) p.classe = true;

    return p;
}

void consultarReserva(passageiros *p, voo v, int n_passageiros){
    char checkCPF[14];
    
    scanf("%s", checkCPF);

    for(int i=0; i<n_passageiros; i++){
        if(strcmp(p[i].cpf, checkCPF) == 0){
            // Printar CPF
            printf("%s\n", p[i].cpf);

            // Printar Nome e Sobrenome
            printf("%s %s\n", p[i].nome, p[i].sobrenome);

            // Printar data (?)
            printf("XX/XX/XXXX\n");

            // Printar voo (?)
            printf("Voo:\n");

            // Printar assento
            printf("Assento: %s\n", p[i].assento);

            // Printar classe e valor baseado no booleano .classe (1=Executiva e 0=Econômica)
            if(p[i].classe){
                printf("Classe: executiva\n");
                printf("Trecho: \n");
                printf("Valor: %f", v.valExe);
            } else {
                printf("Classe: econômica\n");
                printf("Trecho: \n");
                printf("Valor: %f", v.valEco);
            }
        }
    }

    return;
}