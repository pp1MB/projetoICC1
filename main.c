/* 
Trabalho de Introdução à Ciência da Computação I - USP / ICMC / BCC (2024)
Professor: Rudinei Goularte
Autores: 
- Jhonatan Barboza da Silva
- João Gabriel Pieroli da Silva, 
- Pedro Henrique de SouSa Prestes, 15507819 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A estrutura definida pelo tipo "voo" define todas as propriedades do voo. As propriedades qtdAssentos, valEco
e valExe são passadas durante a 'Abertura do Voo (AV)', enquanto as propriedades data, numero, origem e destino
são passadas durante o 'Realizar Reserva (RR)' de cada passageiro. Assume-se que esses dados são padronizados em
um modelo específico e por isso são vetores estáticos, com tamanho estendido até o \0 da string. O booleano 
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

/*A estrutura definida pelo tipo "passageiros" define todas as propriedades referentes a um passageiro em
específico. As propriedades são passadas durante o 'Realizar Reserva (RR)' e podem ser modificadas ou apagadas em
'Modificar Reserva (MR)' e 'Cancelar Reserva (CR)'. Assume-se que nome e sobrenome não são padronizados e por 
isso precisam ser alocados dinamicamente, já CPF e assento são padronizados e por isso são vetores estáticos, com
tamanho estendido até o \0 da string. O booleano classe (bool_classe) define se a pessoa comprou a passagem 
econômica(0) ou executiva(1).*/
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

/* Programa Principal */
int main(void){
    /* Comando fornecido pelo usuário (AV, RR, CR, MR, CA, FD, FV) */
    char inputComando[3]; 

    /* Variáveis que determinam o número de passageiros (n_passageiros) e se é a primeira vez que o programa é
    executado (bool_primeira) */
    int n_passageiros = 0, bool_primeira, memoria = 0; 

    /* Incialização da struct viagem, com o estado inicial descrito como "voo não declarado" */
    voo viagem = {0, 0.0, 0.0, "XX/XX/XX", "VXXX", "XXX", "XXX", -1}; 

    /* Declaração de um ponteiro para o vetor de structs que, por enquanto, aponta para NULL */
    passageiros *passageiro = NULL;

    /* bool_primeira recebe se o arquivo está sendo aberto pela primeira vez para fins de alocação de memória.*/
    if(!(bool_primeira = importarArquivo(&passageiro, &viagem, &n_passageiros))){
        if(n_passageiros + 20 < viagem.qtdAssentos)
            memoria = n_passageiros + 20;
        else
            memoria = viagem.qtdAssentos;
    }

    /* A mensagem de fechamento de voo aparece todo começo de programa caso o voo esteja fechado, porém não fecha o programa. */
    if(viagem.bool_fechado == 1)
        fechamentoVoo(passageiro, viagem, n_passageiros);

    /* O loop abaixo é feito com o comando do-while e, dentro dele, analisamos indeterminadas vezes os comandos 
    dados pelo usuário. Por mais que a sua condicional seja sempre verdadeira (1), os comandos (FD e FV) e também
    a verificação para o voo já estar lotado (n_passageiros == viagem.qtdAssentos) quebram o loop (break;) */
    do{
        /* Pede o comando ao usuário */
        scanf("%s", inputComando); 

        /* Comando Abertura de Voo (AV). Ele só pode mudar as informações do voo caso o voo esteja não declarado
        (-1), caso contrário, o comando é chamado sem retorno, apenas para limpar o buffer do teclado. */
        if(strcmp(inputComando, "AV") == 0){
            if(viagem.bool_fechado == -1){
                viagem = aberturaVoo();
                viagem.bool_fechado = 0;
                if(bool_primeira){
                    if(n_passageiros + 20 < viagem.qtdAssentos)
                        memoria = n_passageiros + 20;
                    else
                        memoria = viagem.qtdAssentos;
                        
                    passageiro = (passageiros *) alocarMemoria(memoria, sizeof(passageiros));
                }
            } else 
                aberturaVoo();
        }
        
        /* Comando Consultar Reserva (CR) */
        else if(strcmp(inputComando, "CR") == 0){
            consultarReserva(passageiro, viagem, n_passageiros);
        } 
        
        /* Comando Realizar Reserva (RR). Há, ainda, a verificação se o voo está aberto (0), caso contrário, o
        comando é chamado sem nenhum retorno para limpar o buffer do teclado. */
        else if(strcmp(inputComando, "RR") == 0){
            if(viagem.bool_fechado == 0){
                passageiro[n_passageiros] = realizarReserva(&viagem);
                n_passageiros++;
                if(memoria == n_passageiros){
                    if(memoria + 20 < viagem.qtdAssentos){
                        memoria += 20;
                        passageiro = realloc(passageiro, memoria * sizeof(passageiros));
                    } else
                        passageiro = realloc(passageiro, viagem.qtdAssentos * sizeof(passageiros));
                }
            }
            else
                realizarReserva(&viagem);
        }

        /* Comando Modificar Reserva (MR) */
        else if(strcmp(inputComando, "MR") == 0){
            modificarReserva(passageiro, viagem, n_passageiros);
        }
        
        /* Comando Cancelar Reserva (CA) */    
        else if(strcmp(inputComando, "CA") == 0){
            cancelarReserva(passageiro, viagem, &n_passageiros);
        }

        /* Comando Fechamento de Voo (FV).  */ 
        else if(strcmp(inputComando, "FV") == 0 || viagem.bool_fechado == 1){
            fechamentoVoo(passageiro, viagem, n_passageiros);
            viagem.bool_fechado = 1;
            break;
        }

        /* Comando Fechamento de Dia (FD) */ 
        else if(strcmp(inputComando, "FD") == 0){
            fechamentoDia(passageiro, viagem, n_passageiros);
            break;
        } 
        
        /* Verificar se o voo está lotado e fazer fechamento de voo */ 
        if(n_passageiros == viagem.qtdAssentos && viagem.bool_fechado == 0){
            fechamentoVoo(passageiro, viagem, n_passageiros);
            viagem.bool_fechado = 1;
            break;
        }

    } while(1);

    /* Chama a função de exportar os dados do programa para o arquivo binário */ 
    exportarArquivo(passageiro, viagem, n_passageiros);

    /* Desaloca toda a memória dinâmica utilizada */
    freeMemoria(passageiro, n_passageiros);

    return 0;
}

/* Essa função é responsável por alocar memória de vetores utilizando o 'calloc' e produzindo uma saída de erro
em caso de problema na alocação.*/
void *alocarMemoria(int size_vet, int size_type){
    void *vet;
    vet = calloc(size_vet, size_type);

    if(vet == NULL){
        printf("Erro em alocar memória, saindo do programa.");
        exit(1);
    }

    return vet;
}

/* Essa função é responsável por importar os dados do arquivo para dentro do programa. Caso o arquivo não exista,
ele retorna 1 para o 'bool_primeira' na main, indicando que o arquivo está sendo aberto pela primeira vez. Sempre
que existir, vamos ler tudo que temos lá. */
int importarArquivo(passageiros **p, voo *v, int *n_passageiros){
    FILE *fp;

    if ((fp = fopen("arquivo.bin", "rb")) == NULL)
        return 1;
    else {

        /* Primeiramente, lemos o número de passageiros e, em seguida, os dados da struct viagem, aqui representada
        por *v. Como v tem informações de tamanhos fixos, basta lê-los com seus respectivos tipos e extensão */ 

        fread(n_passageiros, sizeof(int), 1, fp);
        fread(&(*v).qtdAssentos, sizeof(int), 1, fp);
        fread(&(*v).valEco, sizeof(float), 1, fp);
        fread(&(*v).valExe, sizeof(float), 1, fp);
        fread((*v).data, sizeof(char), 11, fp);
        fread((*v).numero, sizeof(char), 5, fp);
        fread((*v).origem, sizeof(char), 4, fp);
        fread((*v).destino, sizeof(char), 4, fp);
        fread(&(*v).bool_fechado, sizeof(int), 1, fp);

        if(*n_passageiros + 20 < (*v).qtdAssentos)
            *p = (passageiros *) alocarMemoria(*n_passageiros + 20, sizeof(passageiros));
        else
            *p = (passageiros *) alocarMemoria((*v).qtdAssentos, sizeof(passageiros));

        /* Já para o vetor de structs, é preciso ler o tamanho de cada nome e sobrenome (salvos no arquivo) antes
        de efetivamente fazermos a sua leitura, pois fazemos alocação dinâmica. Depois, temos outros dados de
        tamanho fixo, bastando lê-los. Todo esse processo ocorre dentro de um for que é guiado segundo o número de
        passageiros, que já foi lido anteriormente */
        for (int i = 0; i < (*n_passageiros); i++){

            /* Nome (recebe o strlen da string de dentro do arquivo para que possa alocar a memória) */
            long int len_nome;
            fread(&len_nome, sizeof(long int), 1, fp);
            (*p)[i].nome = alocarMemoria(len_nome, sizeof(char));
            fread((*p)[i].nome, sizeof(char), len_nome, fp);

            /* Sobrenome (recebe o strlen da string de dentro do arquivo para que possa alocar a memória) */
            long int len_sobrenome;
            fread(&len_sobrenome, sizeof(long int), 1, fp);
            (*p)[i].sobrenome = alocarMemoria(len_sobrenome, sizeof(char));
            fread((*p)[i].sobrenome, sizeof(char), len_sobrenome, fp);

            /* CPF */
            fread((*p)[i].cpf, sizeof(char), 15, fp);

            /* Assento */
            fread((*p)[i].assento, sizeof(char), 4, fp);

            /* Classe */
            fread(&(*p)[i].bool_classe, sizeof(int), 1, fp);
        }
    }
    fclose(fp);

    return 0;
}

/* Essa função é responsável por exportar os dados do programa para dentro do arquivo. Caso ele não consiga abrir
o arquivo para escrita, retornamos com uma mensagem de erro. Sempre que for bem sucedida, vamos escrever nele tudo
o que temos salvo no programa.*/
void exportarArquivo(passageiros *p, voo v, int n_passageiros){
    FILE *fp;
    if((fp = fopen("arquivo.bin", "wb")) == 0){
        printf("Erro em abrir o arquivo para escrita, saindo do programa.");
        exit(1);
    }

    /* Primeiramente, salvamos o número de passageiros e, em seguida, os dados da struct viagem, aqui representada
    por v. Como v tem informações de tamanhos fixos, basta escrevê-los com seus respectivos tipos e extensão */
    fwrite(&n_passageiros, sizeof(int), 1, fp);
    fwrite(&v.qtdAssentos, sizeof(int), 1, fp);
    fwrite(&v.valEco, sizeof(float), 1, fp);
    fwrite(&v.valExe, sizeof(float), 1, fp);
    fwrite(v.data, sizeof(char), 11, fp);
    fwrite(v.numero, sizeof(char), 5, fp);
    fwrite(v.origem, sizeof(char), 4, fp);
    fwrite(v.destino, sizeof(char), 4, fp);
    fwrite(&v.bool_fechado, sizeof(int), 1, fp);

    /* Já para o vetor de structs, é preciso verificar o tamanho de cada nome (e sobrenome), salvá-lo no arquivo,
    logo antes de cada um deles, para que se tornasse possível fazer a leitura(importarArquivo) de forma adequada,
    alocando-os dinamicamente. Em seguida, temos outros dados com tamanhos fixos, bastando escrevê-los com seus
    respectivos tipos e extensão. Todo esse processo ocorre dentro de um for que é guiado segundo o número de 
    passageiros */
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

/* Essa função é responsável por efetuar a desalocação da memória da váriavel *p, representando o vetor de structs
passageiro da main */
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

/* Referente ao comando Abertura de Voo (AV), a função é responsável por ler as informações dadas pelo usuário,
sejam elas, a quantidade de assentos, o valor da passagem econômica e o valor da passagem executiva, de modo a
retornar a struct obtida a main.
EXEMPLO DE ENTRADA ESPERADA: AV 25 2000.00 4000.00 */
voo aberturaVoo(void){
    voo v = {0, 0.0, 0.0, "XX/XX/XX", "VXXX", "XXX", "XXX", -1};

    scanf("%d %f %f", &v.qtdAssentos, &v.valEco, &v.valExe);

    return v;
}

/* Referente ao comando Realizar Reserva (RR), a função é responsável por ler as informações dadas pelo usuário,
sejam elas, o nome(alocado dinamicamente), o sobrenome(alocado dinamicamente), o CPF, a data, o número do voo, o
assento, o tipo de passagem, o valor pago na passagem, a origem e o destino, de modo a retornar a struct passageiro
para uma dada posição, posição esta determinada na main pela quantidade de passageiros.
EXEMPLO DE ENTRADA ESPERADA: RR Joao Silva 001.001.001-01 02 01 2006 V004 J10 economica 2000.00 CGH RAO */
passageiros realizarReserva(voo *v){
    passageiros p;

    /* Nome */
    char nome[100];
    scanf("%s", nome);
    p.nome = (char *) alocarMemoria(strlen(nome) + 1, sizeof(char));
    strcpy(p.nome, nome);

    /* Sobrenome */
    char sobrenome[100];
    scanf("%s", sobrenome);
    p.sobrenome = (char *) alocarMemoria(strlen(sobrenome) + 1, sizeof(char));
    strcpy(p.sobrenome, sobrenome);

    /* CPF (tamanho definido) */
    scanf("%s", p.cpf);

    /* Data (tamanho definido). Como esse dado é inváriável, não salvamos ele no vetor de struct passageiros, ele
    é armazenado na struct viagem da main, aqui representada por *v. Assim, toda vez, ele é sobreescrito */
    scanf(" %10[^\n]s", (*v).data);
    (*v).data[2] = '/';
    (*v).data[5] = '/';

    /* Número de Voo (tamanho definido). Como esse dado é inváriável, não salvamos ele no vetor de struct 
    passageiros, ele é armazenado na struct viagem da main, aqui representada por *v. Assim, toda vez, ele é
    sobreescrito */
    scanf("%s", (*v).numero);

    /* Assento (tamanho definido) */
    scanf("%s", p.assento);

    /* Classe (Salvo como um booleano: 0 para econômica e 1 para executiva) */
    char classe[10];
    scanf("%s", classe);
    if(strcmp(classe, "economica") == 0) p.bool_classe = 0;
    if(strcmp(classe, "executiva") == 0) p.bool_classe = 1;

    /* Valor (Não precisamos salvar essa informação, visto que já temos ela com a abertura de voo) */
    float valor;
    scanf("%f", &valor);

    /* Origem e Destino (tamanho definido). Como esse dado é inváriável, não salvamos ele no vetor de struct 
    passageiros, ele é armazenado na struct viagem da main, aqui representada por *v. Assim, toda vez, ele é
    sobreescrito*/
    scanf("%s", (*v).origem);
    scanf("%s", (*v).destino);

    return p;
}

/* Referente ao comando Consultar Reserva (CR), a função é responsável por procurar o CPF fornecido pelo usuário
no elemento cpf de cada posição do vetor de struct passageiros, chamando a função para printar a reserva.
EXEMPLO DE ENTRADA ESPERADA: CR 001.001.001-01 */
void consultarReserva(passageiros *p, voo v, int n_passageiros){
    char checkCPF[15];
    
    /* CPF fornecido */
    scanf("%s", checkCPF);

    for(int i=0; i < n_passageiros; i++){
        if(strcmp(p[i].cpf, checkCPF) == 0){
            printarReserva(p[i], v);
            break;
        }
    }
    return;
}

/* Referente ao comando Modificar Reserva (MR), a função é responsável por procurar, desde que o voo ainda esteja
aberto (0), o CPF fornecido pelo usuário no elemento cpf de cada posição do vetor de struct passageiros, a fim de
trocar as informações de nome, sobrenome, cpf e assento de antes pelas novas digitadas pelo usuário.
EXEMPLO DE ENTRADA ESPERADA: MR 000.000.000-00 Leonardo Davinci 000.000.000-01 D30 */
void modificarReserva(passageiros *p, voo v, int n_passageiros){

    /* CPF fornecido */
    char checkCPF[15];
    scanf("%s", checkCPF);

    /* Novo Nome */
    char nome[100];
    scanf("%s", nome);

    /* Novo Sobrenome */
    char sobrenome[100];
    scanf("%s", sobrenome);

    /* Novo CPF */
    char cpf[15];
    scanf("%s", cpf);

    /* Novo Assento */
    char assento[4];
    scanf("%s", assento);

    if(v.bool_fechado == 0)
        for(int i=0; i < n_passageiros; i++){
            if(strcmp(p[i].cpf, checkCPF) == 0){
                /* Todos os dados são reatribuídos */
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

/* Referente ao comando Cancelar Reserva (CA), a função é responsável por procurar, desde que o voo ainda esteja
aberto (0), o CPF fornecido pelo usuário no elemento cpf de cada posição do vetor de struct passageiros, a fim de
limpar os dados guardados dessa reserva. A partir da struct que continha esse cpf, as próximas structs serão
reposicionadas uma posição para trás. Além disso, é decrementado em 1 o número de passageiros. Todo esse processo
é feito por meio de uma variável que funciona como um booleano para saber se coincidiu o CPF fornecido com aquele
de alguma das reservas.
EXEMPLO DE ENTRADA ESPERADA: CA 001.001.001-01 */
void cancelarReserva(passageiros *p, voo v, int *n_passageiros){

    /* Variável controle (houve ou não cancelamento)*/
    int bool_mover = 0;

    /* CPF fornecido */
    char checkCPF[15];    
    scanf("%s", checkCPF);

    /* Se o voo ainda está aberto, percorre todos os CPFs das reservas. Caso encontre um igual, limpa os dados da
    pessoa, como nome e sobrenome, e as próximas structs são passadas uma posição para trás */
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

/* Referente ao comando Fechamento do Dia (FD), a função é responsável por fazer a soma segundo o valor booleano
armazenado no elemento bool_classe de cada posição do vetor de structs passageiro, aqui representado por p. É
sábido que para bool_classe, 0 representa econômica e 1, executiva. Sendo assim, basta que somemos o valor da
ecônomica (guardado na struct viagem, aqui representada por v) e o valor da passagem executiva (guardado na
struct viagem, aqui representada por v) à soma total conforme percorremos o vetor de structs.
EXEMPLO DE ENTRADA ESPERADA: FD */
void fechamentoDia(passageiros *p, voo v, int n_passageiros){

    /* A variável receitaDiaria representa o resultado da soma, por isso, é inicializada como 0*/
    float receitaDiaria = 0; 

    /* Percorremos o vetor de structs analisando o elemento bool_classe, conforme o que achamos,(0) somamos 
    v.valEco (valor da passagem econômica) ou (1) somamos v.valExe (valor da passagem executiva)*/
    for (int i=0; i < n_passageiros; i++){
        if (p[i].bool_classe == 0){
            receitaDiaria += v.valEco; 
        }
        else{
            receitaDiaria += v.valExe;
        }
    }

    /* Imprime a saída de forma formatada conforme o caso teste */
    printf("Fechamento do dia:\nQuantidade de reservas: %d\nPosição: %.2f\n--------------------------------------------------\n", n_passageiros, receitaDiaria);

    return;
}

/* Referente ao comando Fechamento do Voo (FV), a função é responsável por mostrar os dados requeridos para quando
o voo for fechado, sejam eles, o CPF, o nome, o sobrenome e o assento de cada um dos passageiros e o lucro total
obtido com as reservas. 
EXEMPLO DE ENTRADA ESPERADA: FV */
void fechamentoVoo(passageiros *p, voo v, int n_passageiros){
    float receitaTotal = 0; 

    /* Percorremos todo o vetor de structs printando as informações na ordem descrita: o CPF, o nome, o sobrenome
    e o assento de cada um dos passageiros. Assim como no fechamento de dia, a soma é dada analisando o elemento
    bool_classe. Conforme o que achamos,(0) somamos v.valEco (valor da passagem econômica) ou (1) somamos v.valExe
    (valor da passagem executiva) */
    printf("Voo Fechado!\n\n");
    for (int i = 0; i < n_passageiros; i++) {
        printf("%s\n%s %s\n%s\n\n", p[i].cpf, p[i].nome, p[i].sobrenome, p[i].assento);

        if (p[i].bool_classe == 0)
            receitaTotal += v.valEco; 
        else
            receitaTotal += v.valExe;
    }
    /* Imprime a soma total e os hífens conforme o caso teste */
    printf("Valor Total: %.2f\n--------------------------------------------------\n", receitaTotal);

    return;
}

/* A função "printarReserva" é responsável por mostrar os dados requeridos por comandos como Consultar Reserva (CR)
e Modificar Reserva (MR), que têm uma saída padronizada do tipo:
<CPF>
<nome>_<sobrenome>
<data viagem>
Voo:_<número do voo>
Assento:_<assento>
Classe:_<classe>
Trecho:_<origem>_<destino>
Valor:_<valor>
*/
void printarReserva(passageiros p, voo v){

    /* Printar CPF */
    printf("%s\n", p.cpf);

    /* Printar Nome e Sobrenome */
    printf("%s %s\n", p.nome, p.sobrenome);

    /* Printar data */
    printf("%s\n", v.data);

    /* Printar número de voo */
    printf("Voo: %s\n", v.numero);

    /* Printar assento */
    printf("Assento: %s\n", p.assento);

    /* Printar classe e valor baseado no booleano bool_classe (1=Executiva e 0=Econômica) */
    if(p.bool_classe){
        printf("Classe: executiva\n");
        printf("Trecho: %s %s\n", v.origem, v.destino);
        printf("Valor: %.2f\n", v.valExe);
    } else {
        printf("Classe: econômica\n");
        printf("Trecho: %s %s\n", v.origem, v.destino);
        printf("Valor: %.2f\n", v.valEco);
    }
    /* Imprime os hífens conforme o caso teste */
    printf("--------------------------------------------------\n");
    return;
}