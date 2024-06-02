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
    char data[11];
    char *numero;
    char origem[4];
    char destino[4];
} voo;

// Estrutura para informações do passageiro.
typedef struct{
    char *nome;
    char *sobrenome;
    char cpf[15];
    char assento[4];
    bool classe;
} passageiros;

void *alocarMemoria(int size_vet, int size_type);

voo aberturaVoo(void);
passageiros realizarReserva(voo *v, int n_passageiros);
void consultarReserva(passageiros *p, voo v, int n_passageiros);
void modificarReserva(passageiros *p, voo v, int n_passageiros);
void cancelarReserva(void);
void fechamentoDia(void);
void fechamentoVoo(void);

void printarReserva(passageiros p, voo v);

int main(void){
    char inputComando[3]; 
    int n_passageiros = 0;
    voo viagem; // Eu juro que não sei um nome de variável decente, será que pode usar "voo voo" ???
    passageiros *passageiro;

    while(1){

        scanf("%s", inputComando); // Pede o comando ao usuário

        if(strcmp(inputComando, "AV") == 0){
            viagem = aberturaVoo();
            passageiro = (passageiros *) alocarMemoria(viagem.qtdAssentos, sizeof(passageiros)); // Mesmo problema com nome de variável
        }

        if(strcmp(inputComando, "RR") == 0){
            passageiro[n_passageiros] = realizarReserva(&viagem, n_passageiros);
            n_passageiros++;
        }

        if(strcmp(inputComando, "CR") == 0){
            consultarReserva(passageiro, viagem, n_passageiros);
        }

        if(strcmp(inputComando, "MR") == 0){
            modificarReserva(passageiro, viagem, n_passageiros);
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

    return vet;
}

voo aberturaVoo(void){
    voo v; 

    scanf("%d", &v.qtdAssentos);
    scanf("%f", &v.valEco);
    scanf("%f", &v.valExe);

    return v;
}

passageiros realizarReserva(voo *v, int n_passageiros){
    passageiros p;
    char input[200]; // 200 é suficiente?

    // Não sei se é melhor separar uma string pra nome e outra pra sobrenome ou fazer uma lógica para pular o primeiro espaço e armazenar tudo em uma string.
    scanf(" %[^\n]s", input);

    // Nome
    char *token = strtok(input, " ");
    p.nome = (char *) alocarMemoria(strlen(token) + 1, sizeof(char));
    strcpy(p.nome, token);

    // Sobrennome
    token = strtok(NULL, " ");
    p.sobrenome = (char *) alocarMemoria(strlen(token) + 1, sizeof(char));
    strcpy(p.sobrenome, token);

    // CPF
    token = strtok(NULL, " ");
    strncpy(p.cpf, token, 14);
    p.cpf[14] = '\0';

    // Data (salva uma única vez)
    for(int i=0; i < 3; i++) {
        token = strtok(NULL, " ");
        
        if(n_passageiros == 0){
            strcat((*v).data, token);
            if (i < 2) 
                strcat((*v).data, "/");
        }

    }
    (*v).data[11] = '\0';

    // Número Voo (salva uma única vez)
    token = strtok(NULL, " ");
    if(n_passageiros == 0){
        (*v).numero = (char *) alocarMemoria(strlen(token) + 1, sizeof(char));
        strcpy((*v).numero, token);
    }

    // Assento
    token = strtok(NULL, " ");
    strncpy(p.assento, token, 3);
    p.assento[3] = '\0';

    // Classe
    token = strtok(NULL, " ");
    if(strcmp(token, "economica") == 0) p.classe = false;
    if(strcmp(token, "executiva") == 0) p.classe = true;

    // Valor (não salva)
    token = strtok(NULL, " ");

    // Origem e Destino (salva uma única vez)
    if(n_passageiros == 0){
        token = strtok(NULL, " ");
        strcpy((*v).origem, token);
        token = strtok(NULL, " ");
        strcpy((*v).destino, token);
    }

    return p;
}

void consultarReserva(passageiros *p, voo v, int n_passageiros){
    char checkCPF[14];
    
    scanf("%s", checkCPF);

    for(int i=0; i<n_passageiros; i++)
        if(strcmp(p[i].cpf, checkCPF) == 0)
            printarReserva(p[i], v);

    return;
}

void modificarReserva(passageiros *p, voo v, int n_passageiros){
    char input[100];
    passageiros temp;

    scanf(" %[^\n]s", input);

    char *token = strtok(input, " ");
    temp.nome = (char *) alocarMemoria(strlen(token) + 1, sizeof(char));
    strcpy(temp.nome, token);

    token = strtok(NULL, " ");
    temp.sobrenome = (char *) alocarMemoria(strlen(token) + 1, sizeof(char));
    strcpy(temp.sobrenome, token);

    token = strtok(NULL, " ");
    strcpy(temp.cpf, token);

    token = strtok(NULL, " ");
    strcpy(temp.assento, token);

    for(int i=0; i<n_passageiros; i++){
        if(strcmp(p[i].nome, temp.nome) == 0 || strcmp(p[i].sobrenome, temp.sobrenome) == 0 || strcmp(p[i].cpf, temp.cpf) == 0 || strcmp(p[i].assento, temp.assento) == 0){
            // Todos os dados são reatribuídos.
            strcpy(p[i].nome, temp.nome);
            strcpy(p[i].sobrenome, temp.sobrenome);
            strcpy(p[i].cpf, temp.cpf);
            strcpy(p[i].assento, temp.assento);
        }
    }

    printf("Reserva Modificada:\n");
    printarReserva(temp, v);

    return;
}

void printarReserva(passageiros p, voo v){
    // Printar CPF
    printf("%s\n", p.cpf);

    // Printar Nome e Sobrenome
    printf("%s %s\n", p.nome, p.sobrenome);

    // Printar data
    printf("%s\n", v.data);

    // Printar numero voo
    printf("Voo: %s\n", v.numero);

    // Printar assento
    printf("Assento: %s\n", p.assento);

    // Printar classe e valor baseado no booleano .classe (1=Executiva e 0=Econômica)
    if(p.classe){
        printf("Classe: executiva\n");
        printf("Trecho: %s %s\n", v.origem, v.destino);
        printf("Valor: %.2f\n", v.valExe);
    } else {
        printf("Classe: econômica\n");
        printf("Trecho: %s %s\n", v.origem, v.destino);
        printf("Valor: %.2f\n", v.valEco);
     }

     return;
}
/* Casos testes
AV 200 1200.00 2500.00
RR Carlos Massa 555.555.333-99 12 12 2024 V001 A27 economica 1200.00 CGH RAO
RR Maria Massa 444.555.333-93 12 12 2024 V001 A31 economica 1200.00 CGH RAO
MR Carlos Massa 555.555.333-99 A30
*/