#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A estrutura definida pelo tipo "voo" define todas as propriedades do voo. As propriedades
qtdAssentos, valEco e valExe são passadas durante a 'Abertura do Voo (AV)', enquanto as propriedades
data, numero, origem e destino são passadas durante o 'Realizar Reserva (RR)' de cada passageiro. Assume-se
que esses dados são padronizados em um modelo específico e por isso são vetores estáticos. O booleano 
fechado (bool_fechado), embora tenha o nome bool, na verdade é uma variável de controle com 3 estados possíveis:
voo não declarado (-1), voo aberto (0) e voo fechado (1)*/
typedef struct{
    int qtdAssentos;
    float valEco;
    float valExe;
    char data[11];
    char numero[5];
    char origem[4];
    char destino[4];
    int bool_fechado;
} voo;

/*A estrutura definida pelo tipo "passageiros" define todas as propriedades referentes à um passageiro específico. As
propriedades são passadas durante o 'Realizar Reserva (RR)' e podem ser modificadas ou apagadas em 
'Modificar Reserva (MR)' e 'Cancelar Reserva (CR)'. Assume-se que nome e sobrenome não são padronizados e por
isso precisam ser alocados dinamicamente, já CPF e assento são padronizados e por isso são vetores
estáticos. O booleano classe (bool_classe) define se a pessoa comprou a passagem econômica(0) ou executiva(1).*/
typedef struct{
    char *nome;
    char *sobrenome;
    char cpf[15];
    char assento[4];
    int bool_classe;
} passageiros;

/* Protótipo das funções*/
/* Funções referentes ao próprio programa (alocação de memória e arquivo)*/
void *alocarMemoria(int size_vet, int size_type);
int importarArquivo(passageiros **p, voo *v, int *n_passageiros);
void exportarArquivo(passageiros *p, voo v, int n_passageiros);
void freeMemoria(passageiros *p, int n_passageiros);

/* Funções referentes às funções que podem ser utilizadas pelo usuário (AV, RR, CR, MR, CA, FD, FV)*/
voo aberturaVoo(void);
passageiros realizarReserva(voo *v);

void consultarReserva(passageiros *p, voo v, int n_passageiros);
void modificarReserva(passageiros *p, voo v, int n_passageiros);
void cancelarReserva(passageiros *p, voo v, int *n_passageiros);
void fechamentoDia(passageiros *p, voo v, int n_passageiros);
void fechamentoVoo(passageiros *p, voo v, int n_passageiros);

/* Funções adjuntas às funções principais executadas pelo usuário*/
void printarReserva(passageiros p, voo v);

int main(void){
    char inputComando[3]; 
    int n_passageiros = 0, bool_primeira;
    voo viagem = {0, 0.0, 0.0, "XX/XX/XX", "VXXX", "XXX", "XXX", -1}; // Incialização da struct viagem, o estado inicial é "voo não declarado".
    passageiros *passageiro = NULL;

    // Bool_primeira verifica se o arquivo está sendo aberto pela primeira vez para fins de alocação de memória.
    bool_primeira = importarArquivo(&passageiro, &viagem, &n_passageiros);

    // A mensagem de fechamento de voo aparece todo começo de programa caso o voo esteja fechado.
    if(viagem.bool_fechado == 1)
        fechamentoVoo(passageiro, viagem, n_passageiros);

    /* O looping abaixo é referente ao funcionamento do programa baseado nos comandos fornecidos pelo usuário. Apesar da condicional do loop
    ser sempre verdadeira, comandos que encerram o programa (FD e FV) quebram o loop.*/
    do{
        scanf("%s", inputComando); // Pede o comando ao usuário

        /* O comando AV só pode ser executado caso o voo esteja não declarado, caso isso não ocorra, o comando de aberturaVoo é chamado 
        sem nenhum retorno para limpar o buffer do teclado*/
        if(strcmp(inputComando, "AV") == 0){
            if(viagem.bool_fechado == -1){
                viagem = aberturaVoo();
                viagem.bool_fechado = 0;
                if(bool_primeira)
                    passageiro = (passageiros *) alocarMemoria(viagem.qtdAssentos, sizeof(passageiros));
            } else 
                aberturaVoo();
        }
        
        else if(strcmp(inputComando, "CR") == 0){
            consultarReserva(passageiro, viagem, n_passageiros);
        } 
        
        else if(strcmp(inputComando, "RR") == 0){
            if(viagem.bool_fechado == 0){
                passageiro[n_passageiros] = realizarReserva(&viagem);
                n_passageiros++;
            }
            else
                realizarReserva(&viagem);
        }

        else if(strcmp(inputComando, "MR") == 0){
            modificarReserva(passageiro, viagem, n_passageiros);
        }
            
        else if(strcmp(inputComando, "CA") == 0){
            cancelarReserva(passageiro, viagem, &n_passageiros);
        }

        else if(strcmp(inputComando, "FV") == 0 || viagem.bool_fechado == 1){
            fechamentoVoo(passageiro, viagem, n_passageiros);
            viagem.bool_fechado = 1;
            break;
        }

        else if(strcmp(inputComando, "FD") == 0){
            fechamentoDia(passageiro, viagem, n_passageiros);
            break;
        } 
        
        if(n_passageiros == viagem.qtdAssentos){
            fechamentoVoo(passageiro, viagem, n_passageiros);
            viagem.bool_fechado = 1;
            break;
        }

    } while(1);

    exportarArquivo(passageiro, viagem, n_passageiros);
    freeMemoria(passageiro, n_passageiros);

    return 0;
}

/* Essa função é responsável por alocar memória de vetores utilizando o 'calloc' e produzindo uma saída de erro em caso de problema na alocação.*/
void *alocarMemoria(int size_vet, int size_type){
    void *vet;
    vet = calloc(size_vet, size_type);

    if(vet == NULL){
        printf("Erro em alocar memória, saindo do programa.");
        exit(1);
    }

    return vet;
}

/* Essa função é responsável por importar os dados do arquivo para dentro do programa. Caso o arquivo não exista, ele retorna 1 para o 'bool_primeira'
na main, indicando que o arquivo está sendo aberto pela primeira vez.*/
int importarArquivo(passageiros **p, voo *v, int *n_passageiros){
    FILE *fp;

    if ((fp = fopen("arquivo.bin", "rb")) == NULL)
        return 1;
    else {
        fread(n_passageiros, sizeof(int), 1, fp);
        fread(&(*v).qtdAssentos, sizeof(int), 1, fp);
        fread(&(*v).valEco, sizeof(float), 1, fp);
        fread(&(*v).valExe, sizeof(float), 1, fp);
        fread((*v).data, sizeof(char), 11, fp);
        fread((*v).numero, sizeof(char), 5, fp);
        fread((*v).origem, sizeof(char), 4, fp);
        fread((*v).destino, sizeof(char), 4, fp);
        fread(&(*v).bool_fechado, sizeof(int), 1, fp);

        *p = (passageiros *) alocarMemoria((*v).qtdAssentos, sizeof(passageiros));

        for (int i = 0; i < (*n_passageiros); i++){

            // Nome (recebe o strlen da string de dentro do arquivo para que possa alocar a memória)
            long int len_nome;
            fread(&len_nome, sizeof(long int), 1, fp);
            (*p)[i].nome = alocarMemoria(len_nome, sizeof(char));
            fread((*p)[i].nome, sizeof(char), len_nome, fp);

            // Sobrenome (recebe o strlen da string de dentro do arquivo para que possa alocar a memória)
            long int len_sobrenome;
            fread(&len_sobrenome, sizeof(long int), 1, fp);
            (*p)[i].sobrenome = alocarMemoria(len_sobrenome, sizeof(char));
            fread((*p)[i].sobrenome, sizeof(char), len_sobrenome, fp);

            // CPF
            fread((*p)[i].cpf, sizeof(char), 15, fp);

            // Assento
            fread((*p)[i].assento, sizeof(char), 4, fp);

            // Classe
            fread(&(*p)[i].bool_classe, sizeof(int), 1, fp);
        }
    }
    fclose(fp);

    return 0;
}

void exportarArquivo(passageiros *p, voo v, int n_passageiros){
    FILE *fp;
    if((fp = fopen("arquivo.bin", "wb")) == 0){
        printf("Erro em abrir o arquivo para escrita, saindo do programa.");
        exit(1);
    }

    fwrite(&n_passageiros, sizeof(int), 1, fp);
    fwrite(&v.qtdAssentos, sizeof(int), 1, fp);
    fwrite(&v.valEco, sizeof(float), 1, fp);
    fwrite(&v.valExe, sizeof(float), 1, fp);
    fwrite(v.data, sizeof(char), 11, fp);
    fwrite(v.numero, sizeof(char), 5, fp);
    fwrite(v.origem, sizeof(char), 4, fp);
    fwrite(v.destino, sizeof(char), 4, fp);
    fwrite(&v.bool_fechado, sizeof(int), 1, fp);

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
        fwrite(&p[i].bool_classe, sizeof(int), 1, fp);
    }
    fclose(fp);
    return;
}

void freeMemoria(passageiros *p, int n_passageiros){
    for(int i=0; i < n_passageiros; i++){
        free(p[i].nome);
        p[i].nome = NULL;
        free(p[i].sobrenome);
        p[i].sobrenome = NULL;
    }
    free(p);
    p = NULL;
}

voo aberturaVoo(void){
    voo v = {0, 0.0, 0.0, "XX/XX/XX", "VXXX", "XXX", "XXX", -1};

    scanf("%d %f %f", &v.qtdAssentos, &v.valEco, &v.valExe);

    return v;
}

passageiros realizarReserva(voo *v){
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
    if(strcmp(classe, "economica") == 0) p.bool_classe = 0;
    if(strcmp(classe, "executiva") == 0) p.bool_classe = 1;

    // Valor (não salva)
    float valor;
    scanf("%f", &valor);

    // Origem e Destino (salva uma única vez)
    scanf("%s", (*v).origem);
    scanf("%s", (*v).destino);

    return p;
}

void consultarReserva(passageiros *p, voo v, int n_passageiros){
    char checkCPF[15];
    
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
    char checkCPF[15];
    scanf("%s", checkCPF);

    char nome[100];
    scanf("%s", nome);

    char sobrenome[100];
    scanf("%s", sobrenome);

    char cpf[15];
    scanf("%s", cpf);

    char assento[4];
    scanf("%s", assento);

    if(v.bool_fechado == 0)
        for(int i=0; i < n_passageiros; i++){
            if(strcmp(p[i].cpf, checkCPF) == 0){
                // Todos os dados são reatribuídos.
                free(p[i].nome);
                p[i].nome = NULL;
                p[i].nome = (char *) alocarMemoria(strlen(nome) + 1, sizeof(char));
                strcpy(p[i].nome, nome);

                free(p[i].sobrenome);
                p[i].sobrenome = NULL;
                p[i].sobrenome = (char *) alocarMemoria(strlen(sobrenome) + 1, sizeof(char));
                strcpy(p[i].sobrenome, sobrenome);

                strcpy(p[i].cpf, cpf);

                strcpy(p[i].assento, assento);

                printf("Reserva Modificada:\n");
                printarReserva(p[i], v);

                break;
            }
        }

    return;
}

void cancelarReserva(passageiros *p, voo v, int *n_passageiros){
    char checkCPF[15];
    int bool_mover = 0;
    
    scanf("%s", checkCPF);

    if(v.bool_fechado == 0){
        for(int i=0; i < (*n_passageiros); i++){

            if(strcmp(p[i].cpf, checkCPF) == 0){
                bool_mover = 1;
                free(p[i].nome);
                p[i].nome = NULL;
                free(p[i].sobrenome);
                p[i].sobrenome = NULL;
            }

            else if(bool_mover)
                p[i-1] = p[i];
        }

        if(bool_mover)
            (*n_passageiros)--;
    }

    return;
}

void fechamentoDia(passageiros *p, voo v, int n_passageiros){
    float receitaDiaria = 0; 

    for (int i=0; i < n_passageiros; i++){
        if (p[i].bool_classe == 0){
            receitaDiaria += v.valEco; 
        }
        else{
            receitaDiaria += v.valExe;
        }
    }

    printf("Fechamento do dia:\nQuantidade de reservas: %d\nPosição: %.2f\n--------------------------------------------------\n", n_passageiros, receitaDiaria);

    return;
}

void fechamentoVoo(passageiros *p, voo v, int n_passageiros){
    float receitaTotal = 0; 

    printf("Voo Fechado!\n\n");
    for (int i = 0; i < n_passageiros; i++) {
        printf("%s\n%s %s\n%s\n\n", p[i].cpf, p[i].nome, p[i].sobrenome, p[i].assento);

        if (p[i].bool_classe == 0)
            receitaTotal += v.valEco; 
        else
            receitaTotal += v.valExe;
    }

    printf("Valor Total: %.2f\n--------------------------------------------------\n", receitaTotal);

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

    // Printar classe e valor baseado no booleano bool_classe (1=Executiva e 0=Econômica)
    if(p.bool_classe){
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
  


/* Casos testes
AV 200 1200.00 2500.00
RR Carlos Massa 555.555.333-99 12 12 2024 V001 A27 economica 1200.00 CGH RAO
RR Maria Massa 444.555.333-93 12 12 2024 V001 A31 economica 1200.00 CGH RAO
RR Roberto Carlos 555.333.333-89 12 12 2024 V001 P12 executiva 2500.00 CGH RAO
MR 555.555.333-99 Carlos Massa 555.555.333-99 A30   
CA 444.555.333-93
CA 555.555.333-99
CA 555.333.333-89
FD
*/

/*Caso teste do PDF
AV 200 1200.00 2500.00
RR Carlos Massa 555.555.333-99 12 12 2024 V001 A27 economica 1200.00 CGH RAO
RR Maria Massa 444.555.333-93 12 12 2024 V001 A31 economica 1200.00 CGH RAO
RR Roberto Carlos 555.333.333-89 12 12 2024 V001 P12 executiva 2500.00 CGH RAO
MR 555.555.333-99 Carlos Massa 555.555.333-99 A30
FD

RR Euclides Simon 222.111.333-12 12 12 2024 V001 B01 economica 1200.00 CGH RAO
RR Marta Rocha 999.888.222-21 12 12 2024 V001 C02 executiva 2500.00 CGH RAO
CR 555.333.333-89
RR Clara Nunes 111.000.123-45 12 12 2024 V001 C09 executiva 2500.00 CGH RAO
FV
*/