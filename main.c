#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct{
    int qtdAssentos;
    float valEco;
    float valExe;
    char data[11];
    char numero[5];
    char origem[4];
    char destino[4];
    int fechado;
} voo;

// Estrutura para informações do passageiro.
typedef struct{
    char *nome;
    char *sobrenome;
    char cpf[15];
    char assento[4];
    int classe;
} passageiros;


/* Funções referentes ao próprio programa (alocação de memória e arquivo)*/
void *alocarMemoria(int size_vet, int size_type);
int importarArquivo(voo *v, passageiros **p, int *n_passageiros);
void exportarArquivo(voo v, passageiros *p, int n_passageiros);

/* Funções referentes às funções que podem ser utilizadas pelo usuário (AV, RR, CR, MR, CA, FD, FV)*/
voo aberturaVoo(void);
passageiros realizarReserva(voo *v, int n_passageiros);
void consultarReserva(passageiros *p, voo v, int n_passageiros);
void modificarReserva(passageiros *p, voo v, int n_passageiros);
void cancelarReserva(int *n_passageiros, passageiros *p);
void fechamentoDia(int n_passageiros, passageiros *p, voo v);
void fechamentoVoo(int n_passageiros, passageiros *reservas, voo v);

/* Funções adjuntas às funções principais executadas pelo usuário*/
void printarReserva(passageiros p, voo v);

int main(void){
    char inputComando[3]; 
    int n_passageiros = 0, bool_primeira;
    voo viagem;
    passageiros *passageiro;

    viagem.fechado = 0;
    bool_primeira = importarArquivo(&viagem, &passageiro, &n_passageiros);

    do{
        scanf("%s", inputComando); // Pede o comando ao usuário

        if(n_passageiros != viagem.qtdAssentos && !viagem.fechado){
            if(strcmp(inputComando, "AV") == 0){
                viagem = aberturaVoo();

                if(bool_primeira)
                    passageiro = (passageiros *) alocarMemoria(viagem.qtdAssentos, sizeof(passageiros)); 
            }

            else if(strcmp(inputComando, "RR") == 0){
                passageiro[n_passageiros] = realizarReserva(&viagem, n_passageiros);
                n_passageiros++;
            }

            else if(strcmp(inputComando, "CR") == 0){
                consultarReserva(passageiro, viagem, n_passageiros);
            }

            else if(strcmp(inputComando, "MR") == 0){
                modificarReserva(passageiro, viagem, n_passageiros);
            }
            
            else if(strcmp(inputComando, "CA") == 0){
                cancelarReserva(&n_passageiros, passageiro);
            }

            else if(strcmp(inputComando, "FD") == 0){
                fechamentoDia(n_passageiros, passageiro, viagem);
                break;   
            }

            else if(strcmp(inputComando, "FV") == 0){
                fechamentoVoo(n_passageiros, passageiro, viagem);
                viagem.fechado = 1;
            }
        }

        if(n_passageiros == viagem.qtdAssentos || viagem.fechado)
            fechamentoVoo(n_passageiros, passageiro, viagem);

    } while(1);

    exportarArquivo(viagem, passageiro, n_passageiros);

    return 0;
}

void *alocarMemoria(int size_vet, int size_type){
    void *vet;
    vet = calloc(size_vet, size_type);

    assert(vet != NULL);

    return vet;
}

int importarArquivo(voo *v, passageiros **p, int *n_passageiros) {
    FILE *fp;

    if ((fp = fopen("arquivo.bin", "rb")) == NULL)
        return 1;
    else {
        fread(n_passageiros, sizeof(int), 1, fp);
        fread(&(*v).qtdAssentos, sizeof(int), 1, fp);
        fread(&(*v).valEco, sizeof(float), 1, fp);
        fread(&(*v).valExe, sizeof(float), 1, fp);
        fread((*v).data, sizeof(char), 11, fp);
        // (*v).data[10] = '\0'; 
        fread((*v).numero, sizeof(char), 5, fp);
        // (*v).numero[4] = '\0';
        fread((*v).origem, sizeof(char), 4, fp);
        // (*v).origem[3] = '\0';
        fread((*v).destino, sizeof(char), 4, fp);
        // (*v).destino[3] = '\0';

        *p = (passageiros *) alocarMemoria((*v).qtdAssentos, sizeof(passageiros));

        for (int i = 0; i < (*n_passageiros); i++) {
            // Nome
            long int len_nome;
            fread(&len_nome, sizeof(long int), 1, fp);
            (*p)[i].nome = alocarMemoria(len_nome, sizeof(char));
            fread((*p)[i].nome, sizeof(char), len_nome, fp);
            // (*p)[i].nome[len_nome] = '\0';

            // Sobrenome
            long int len_sobrenome;
            fread(&len_sobrenome, sizeof(long int), 1, fp);
            (*p)[i].sobrenome = alocarMemoria(len_sobrenome, sizeof(char));
            fread((*p)[i].sobrenome, sizeof(char), len_sobrenome, fp);
            // (*p)[i].nome[len_nome] = '\0';

            // CPF
            fread((*p)[i].cpf, sizeof(char), 15, fp);
            // (*p)[i].cpf[14] = '\0';  // Null-terminate the string

            // Assento
            fread((*p)[i].assento, sizeof(char), 4, fp);
            // (*p)[i].assento[3] = '\0';  // Null-terminate the string

            // Classe
            fread(&(*p)[i].classe, sizeof(int), 1, fp);
        }
    }
    fclose(fp);

    return 0;
}



void exportarArquivo(voo v, passageiros *p, int n_passageiros){
    FILE *fp;
    fp = fopen("arquivo.bin", "wb");

    fwrite(&n_passageiros, sizeof(int), 1, fp);
    fwrite(&v.qtdAssentos, sizeof(int), 1, fp);
    fwrite(&v.valEco, sizeof(float), 1, fp);
    fwrite(&v.valExe, sizeof(float), 1, fp);
    fwrite(v.data, sizeof(char), 11, fp);
    fwrite(v.numero, sizeof(char), 5, fp);
    fwrite(v.origem, sizeof(char), 4, fp);
    fwrite(v.destino, sizeof(char), 4, fp);

    for(int i=0; i < n_passageiros; i++){
        // Nome
        long int len_nome = strlen(p[i].nome) + 1;
        fwrite(&len_nome, sizeof(long int), 1, fp);
        fwrite(p[i].nome, sizeof(char), len_nome, fp);

        // Sobrenome
        long int len_sobrenome = strlen(p[i].sobrenome) + 1;
        fwrite(&len_sobrenome, sizeof(long int), 1, fp);
        fwrite(p[i].sobrenome, sizeof(char), len_sobrenome, fp);

        // CPF
        fwrite(p[i].cpf, sizeof(char), 15, fp);

        // Assento
        fwrite(p[i].assento, sizeof(char), 4, fp);

        // Classe
        fwrite(&p[i].classe, sizeof(int), 1, fp);
    }
    fclose(fp);
    return;
}

voo aberturaVoo(void){
    voo v; 

    scanf("%d %f %f", &v.qtdAssentos, &v.valEco, &v.valExe);

    return v;
}

passageiros realizarReserva(voo *v, int n_passageiros){
    passageiros p;

    // Nome
    char nome[100];
    scanf("%s", nome);
    p.nome = (char *) alocarMemoria(strlen(nome) + 1, sizeof(char));
    strcpy(p.nome, nome);

    // Sobrenome
    char sobrenome[100];
    scanf("%s", sobrenome);
    p.sobrenome = (char *) alocarMemoria(strlen(sobrenome) + 1, sizeof(char));
    strcpy(p.sobrenome, sobrenome);

    // CPF (tamanho definido)
    scanf("%s", p.cpf);

    // Data (tamanho definido) (salva uma única vez)
    scanf(" %10[^\n]s", (*v).data);
    (*v).data[2] = '/';
    (*v).data[5] = '/';

    // Número Voo (salva uma única vez)
    scanf("%s", (*v).numero);

    // Assento
    scanf("%s", p.assento);

    // Classe
    char classe[10];
    scanf("%s", classe);
    if(strcmp(classe, "economica") == 0) p.classe = 0;
    if(strcmp(classe, "executiva") == 0) p.classe = 1;

    // Valor (não salva)
    float valor;
    scanf("%f", &valor);

    // Origem e Destino (salva uma única vez)
    scanf("%s", (*v).origem);
    scanf("%s", (*v).destino);

    return p;
}

void consultarReserva(passageiros *p, voo v, int n_passageiros){
    char checkCPF[14];
    
    scanf("%s", checkCPF);

    for(int i=0; i < n_passageiros; i++){
        if(strcmp(p[i].cpf, checkCPF) == 0){
            printarReserva(p[i], v);
            break;
        }
    }
    return;
}

void modificarReserva(passageiros *p, voo v, int n_passageiros){
    passageiros temp;

    char checkCPF[14];
    scanf("%s", checkCPF);

    for(int i=0; i < n_passageiros; i++){
        if(strcmp(p[i].cpf, checkCPF) == 0){
            // Todos os dados são reatribuídos.
            char nome[100];
            scanf("%s", nome);
            p[i].nome = (char *) alocarMemoria(strlen(nome) + 1, sizeof(char));
            strcpy(p[i].nome, nome);

            char sobrenome[100];
            scanf("%s", sobrenome);
            p[i].sobrenome = (char *) alocarMemoria(strlen(sobrenome) + 1, sizeof(char));
            strcpy(p[i].sobrenome, sobrenome);

            scanf("%s", p[i].cpf);

            scanf("%s", p[i].assento);

            printf("Reserva Modificada:\n");
            printarReserva(p[i], v);

            break;
        }
    }

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

    printf("--------------------------------------------------\n");
    return;
}

void cancelarReserva(int *n_passageiros, passageiros *p){
    char checkCPF[14];
    int mover = 0;
    
    scanf("%s", checkCPF);

    for(int i=0; i < (*n_passageiros); i++){

        if(strcmp(p[i].cpf, checkCPF) == 0)
            mover = 1;

        else if(mover)
            p[i-1] = p[i];
    }

    if(mover)
        (*n_passageiros)--;

    return;
}

void fechamentoDia(int n_passageiros, passageiros *p, voo v){
    float receitaDiaria = 0; 

    for (int i=0; i < n_passageiros; i++){
        if (p[i].classe == 0){
            receitaDiaria += v.valEco; 
        }
        else{
            receitaDiaria += v.valExe;
        }
    }

    printf("Fechamento do dia:\nQuantidade de reservas: %d\nPosição: %.2f\n--------------------------------------------------\n", n_passageiros, receitaDiaria);

    return;
}

void fechamentoVoo(int n_passageiros, passageiros *reservas, voo v) {
    float receitaTotal = 0; 

    printf("Voo Fechado!\n\n");
    for (int i = 0; i < n_passageiros; i++) {
        printf("%s\n%s %s\n%s\n\n", reservas[i].cpf, reservas[i].nome, reservas[i].sobrenome, reservas[i].assento);

        if (reservas[i].classe == 0)
            receitaTotal += v.valEco; 
        else
            receitaTotal += v.valExe;
    }

    printf("Valor Total: %.2f\n--------------------------------------------------\n", receitaTotal);

    return;
}
  


/* Casos testes
AV 200 1200.00 2500.00
RR Carlos Massa 555.555.333-99 12 12 2024 V001 A27 economica 1200.00 CGH RAO
RR Maria Massa 444.555.333-93 12 12 2024 V001 A31 economica 1200.00 CGH RAO
RR Roberto Carlos 555.333.333-89 12 12 2024 V001 P12 executiva 2500.00 CGH RAO
MR 555.555.333-99 Carlos Massa 555.555.333-99 A30   
CA 444.555.333-93
*/