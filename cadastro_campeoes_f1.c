#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ARQUIVO     "campeoes_f1.txt"
#define TAM_NOME    40
#define TAM_TEXTO   30
#define ANO_MINIMO  1950

typedef struct No {
    int ano;
    char piloto[TAM_NOME];
    char equipe[TAM_TEXTO];
    char pais[TAM_TEXTO];
    struct No *esq;
    struct No *dir;
} No;

static int entradaAcabou = 0;

void aparar(char *texto)
{
    size_t inicio = 0;
    size_t fim = strlen(texto);

    while (fim > 0 && isspace((unsigned char) texto[fim - 1])) {
        fim--;
    }
    texto[fim] = '\0';
    while (texto[inicio] != '\0' && isspace((unsigned char) texto[inicio])) {
        inicio++;
    }
    if (inicio > 0) {
        memmove(texto, texto + inicio, strlen(texto + inicio) + 1);
    }
}

int lerLinha(char *destino, int tamanho)
{
    size_t n;
    int c;

    if (fgets(destino, tamanho, stdin) == NULL) {
        entradaAcabou = 1;
        return 0;
    }
    n = strlen(destino);
    if (n > 0 && destino[n - 1] == '\n') {
        destino[n - 1] = '\0';
    } else {

        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }
    return 1;
}

int lerTexto(const char *pergunta, char *destino, int tamanho)
{
    while (1) {
        printf("%s", pergunta);
        if (!lerLinha(destino, tamanho)) {
            return 0;
        }
        aparar(destino);
        if (destino[0] == '\0') {
            printf("  Preciso de alguma resposta aqui.\n");
        } else if (strchr(destino, ';') != NULL) {
            printf("  Nao use ponto e virgula, ele separa os campos no arquivo.\n");
        } else {
            return 1;
        }
    }
}

int lerInteiro(const char *pergunta, int *valor)
{
    char linha[50];
    char *fim;
    long numero;

    while (1) {
        printf("%s", pergunta);
        if (!lerLinha(linha, sizeof(linha))) {
            return 0;
        }
        aparar(linha);
        numero = strtol(linha, &fim, 10);
        if (linha[0] != '\0' && *fim == '\0' && numero >= -100000L && numero <= 100000L) {
            *valor = (int) numero;
            return 1;
        }
        printf("  Isso nao parece um numero valido. Tenta de novo?\n");
    }
}

No *criarNo(int ano, const char *piloto, const char *equipe, const char *pais)
{
    No *novo = (No *) malloc(sizeof(No));

    if (novo == NULL) {
        printf("Erro: sem memoria para um novo campeao.\n");
        return NULL;
    }
    novo->ano = ano;
    strncpy(novo->piloto, piloto, TAM_NOME - 1);
    novo->piloto[TAM_NOME - 1] = '\0';
    strncpy(novo->equipe, equipe, TAM_TEXTO - 1);
    novo->equipe[TAM_TEXTO - 1] = '\0';
    strncpy(novo->pais, pais, TAM_TEXTO - 1);
    novo->pais[TAM_TEXTO - 1] = '\0';
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

No *inserir(No *raiz, No *novo, int *inseriu)
{
    if (raiz == NULL) {
        *inseriu = 1;
        return novo;
    }
    if (novo->ano < raiz->ano) {
        raiz->esq = inserir(raiz->esq, novo, inseriu);
    } else if (novo->ano > raiz->ano) {
        raiz->dir = inserir(raiz->dir, novo, inseriu);
    } else {
        *inseriu = 0;
    }
    return raiz;
}

No *buscar(No *raiz, int ano)
{
    while (raiz != NULL) {
        if (ano < raiz->ano) {
            raiz = raiz->esq;
        } else if (ano > raiz->ano) {
            raiz = raiz->dir;
        } else {
            return raiz;
        }
    }
    return NULL;
}

No *menorNo(No *raiz)
{
    while (raiz->esq != NULL) {
        raiz = raiz->esq;
    }
    return raiz;
}

No *remover(No *raiz, int ano, int *removeu)
{
    No *aux;
    No *sucessor;
    int ignorar;

    if (raiz == NULL) {
        *removeu = 0;
        return NULL;
    }
    if (ano < raiz->ano) {
        raiz->esq = remover(raiz->esq, ano, removeu);
    } else if (ano > raiz->ano) {
        raiz->dir = remover(raiz->dir, ano, removeu);
    } else {
        *removeu = 1;
        if (raiz->esq == NULL) {
            aux = raiz->dir;
            free(raiz);
            return aux;
        }
        if (raiz->dir == NULL) {
            aux = raiz->esq;
            free(raiz);
            return aux;
        }

        sucessor = menorNo(raiz->dir);
        raiz->ano = sucessor->ano;
        strcpy(raiz->piloto, sucessor->piloto);
        strcpy(raiz->equipe, sucessor->equipe);
        strcpy(raiz->pais, sucessor->pais);
        raiz->dir = remover(raiz->dir, raiz->ano, &ignorar);
    }
    return raiz;
}

int contar(No *raiz)
{
    if (raiz == NULL) {
        return 0;
    }
    return 1 + contar(raiz->esq) + contar(raiz->dir);
}

int altura(No *raiz)
{
    int he;
    int hd;

    if (raiz == NULL) {
        return 0;
    }
    he = altura(raiz->esq);
    hd = altura(raiz->dir);
    return 1 + (he > hd ? he : hd);
}

void destruir(No *raiz)
{
    if (raiz == NULL) {
        return;
    }
    destruir(raiz->esq);
    destruir(raiz->dir);
    free(raiz);
}

void listarEmOrdem(No *raiz)
{
    if (raiz == NULL) {
        return;
    }
    listarEmOrdem(raiz->esq);
    printf("  %d  |  %-17s %-9s %s\n", raiz->ano, raiz->piloto, raiz->equipe, raiz->pais);
    listarEmOrdem(raiz->dir);
}

void exibirArvore(No *raiz, int nivel, const char *rotulo)
{
    int i;

    if (raiz == NULL) {
        return;
    }
    printf("  ");
    for (i = 0; i < nivel * 4; i++) {
        printf(" ");
    }
    printf("%s: %d - %s\n", rotulo, raiz->ano, raiz->piloto);
    exibirArvore(raiz->esq, nivel + 1, "esq");
    exibirArvore(raiz->dir, nivel + 1, "dir");
}

void gravarNo(No *raiz, FILE *arq)
{
    if (raiz == NULL) {
        return;
    }
    fprintf(arq, "%d;%s;%s;%s\n", raiz->ano, raiz->piloto, raiz->equipe, raiz->pais);
    gravarNo(raiz->esq, arq);
    gravarNo(raiz->dir, arq);
}

int gravarArquivo(No *raiz)
{
    FILE *arq = fopen(ARQUIVO, "w");

    if (arq == NULL) {
        printf("Erro: nao consegui abrir %s para gravar.\n", ARQUIVO);
        return 0;
    }
    gravarNo(raiz, arq);
    fclose(arq);
    return 1;
}

No *carregarArquivo(No *raiz, int *lidos)
{
    FILE *arq;
    char linha[200];
    char *campoAno;
    char *campoPiloto;
    char *campoEquipe;
    char *campoPais;
    No *novo;
    int inseriu;
    int ano;

    *lidos = 0;
    arq = fopen(ARQUIVO, "r");
    if (arq == NULL) {
        printf("Arquivo %s nao encontrado. Comecando com a galeria vazia.\n", ARQUIVO);
        return raiz;
    }
    while (fgets(linha, sizeof(linha), arq) != NULL) {
        aparar(linha);
        if (linha[0] == '\0') {
            continue;
        }
        campoAno    = strtok(linha, ";");
        campoPiloto = strtok(NULL, ";");
        campoEquipe = strtok(NULL, ";");
        campoPais   = strtok(NULL, ";");
        if (campoAno == NULL || campoPiloto == NULL || campoEquipe == NULL || campoPais == NULL) {
            printf("Linha ignorada (formato errado).\n");
            continue;
        }
        ano = atoi(campoAno);
        novo = criarNo(ano, campoPiloto, campoEquipe, campoPais);
        if (novo == NULL) {
            break;
        }
        inseriu = 0;
        raiz = inserir(raiz, novo, &inseriu);
        if (inseriu) {
            (*lidos)++;
        } else {
            free(novo);
        }
    }
    fclose(arq);
    return raiz;
}

No *acaoCadastrar(No *raiz)
{
    int ano;
    int inseriu = 0;
    char piloto[TAM_NOME];
    char equipe[TAM_TEXTO];
    char pais[TAM_TEXTO];
    No *novo;

    if (!lerInteiro("  Em que ano foi o titulo? ", &ano)) {
        return raiz;
    }
    if (ano < ANO_MINIMO) {
        printf("  O Mundial comecou em %d, tenta um ano a partir dai.\n", ANO_MINIMO);
        return raiz;
    }
    if (buscar(raiz, ano) != NULL) {
        printf("  Ja existe um campeao em %d. Remova o antigo se quiser trocar.\n", ano);
        return raiz;
    }
    if (!lerTexto("  Nome do piloto: ", piloto, sizeof(piloto)) ||
        !lerTexto("  Equipe: ", equipe, sizeof(equipe)) ||
        !lerTexto("  Pais: ", pais, sizeof(pais))) {
        return raiz;
    }
    novo = criarNo(ano, piloto, equipe, pais);
    if (novo == NULL) {
        return raiz;
    }
    raiz = inserir(raiz, novo, &inseriu);
    if (inseriu) {
        printf("  Pronto! %s agora faz parte da galeria.\n", piloto);
    } else {
        free(novo);
    }
    return raiz;
}

void acaoBuscar(No *raiz)
{
    int ano;
    No *achado;

    if (!lerInteiro("  De qual ano voce quer saber? ", &ano)) {
        return;
    }
    achado = buscar(raiz, ano);
    if (achado != NULL) {
        printf("  Em %d o campeao foi %s, pela %s (%s).\n",
               ano, achado->piloto, achado->equipe, achado->pais);
    } else {
        printf("  Nao tenho ninguem cadastrado em %d.\n", ano);
    }
}

void acaoAlterarEquipe(No *raiz)
{
    int ano;
    char equipe[TAM_TEXTO];
    No *achado;

    if (!lerInteiro("  De qual ano? ", &ano)) {
        return;
    }
    achado = buscar(raiz, ano);
    if (achado == NULL) {
        printf("  Nao tenho ninguem cadastrado em %d.\n", ano);
        return;
    }
    printf("  Equipe atual: %s\n", achado->equipe);
    if (!lerTexto("  Nova equipe: ", equipe, sizeof(equipe))) {
        return;
    }
    strcpy(achado->equipe, equipe);
    printf("  Equipe de %d alterada para %s.\n", ano, achado->equipe);
}

No *acaoRemover(No *raiz)
{
    int ano;
    int removeu = 0;

    if (!lerInteiro("  Qual ano voce quer remover? ", &ano)) {
        return raiz;
    }
    raiz = remover(raiz, ano, &removeu);
    if (removeu) {
        printf("  Feito, o ano %d saiu da galeria.\n", ano);
    } else {
        printf("  Nao encontrei %d, entao nada mudou.\n", ano);
    }
    return raiz;
}

void acaoListar(No *raiz)
{
    if (raiz == NULL) {
        printf("  A galeria esta vazia.\n");
        return;
    }
    printf("\n  ANO   |  PILOTO           EQUIPE    PAIS\n");
    printf("  ----------------------------------------------\n");
    listarEmOrdem(raiz);
}

void acaoExibirArvore(No *raiz)
{
    if (raiz == NULL) {
        printf("  A arvore esta vazia.\n");
        return;
    }
    printf("\n  Estrutura da arvore (pre-ordem, com recuo):\n\n");
    exibirArvore(raiz, 0, "raiz");
}

void mostrarMenu(void)
{
    printf("\n------------- O que vamos fazer? -------------\n");
    printf("  1) Cadastrar um campeao\n");
    printf("  2) Buscar o campeao de um ano\n");
    printf("  3) Alterar a equipe de um campeao\n");
    printf("  4) Remover um campeao\n");
    printf("  5) Listar em ordem de ano\n");
    printf("  6) Exibir a arvore\n");
    printf("  7) Ver total de campeoes e altura da arvore\n");
    printf("  8) Gravar no arquivo agora\n");
    printf("  0) Gravar e sair\n");
}

int main(void)
{
    No *raiz = NULL;
    int lidos = 0;
    int opcao = -1;

    printf("Bem-vindo a Galeria de Campeoes da Formula 1!\n");
    raiz = carregarArquivo(raiz, &lidos);
    printf("Registros carregados de %s: %d\n", ARQUIVO, lidos);

    while (opcao != 0 && !entradaAcabou) {
        mostrarMenu();
        if (!lerInteiro("Sua escolha: ", &opcao)) {
            break;
        }
        switch (opcao) {
            case 1: raiz = acaoCadastrar(raiz); break;
            case 2: acaoBuscar(raiz); break;
            case 3: acaoAlterarEquipe(raiz); break;
            case 4: raiz = acaoRemover(raiz); break;
            case 5: acaoListar(raiz); break;
            case 6: acaoExibirArvore(raiz); break;
            case 7:
                printf("  Campeoes: %d  |  Altura da arvore: %d\n", contar(raiz), altura(raiz));
                break;
            case 8:
                if (gravarArquivo(raiz)) {
                    printf("  Dados gravados em %s.\n", ARQUIVO);
                }
                break;
            case 0: break;
            default: printf("  Essa opcao nao existe, tenta uma da lista.\n");
        }
    }

    if (gravarArquivo(raiz)) {
        printf("\nDados gravados em %s. Ate a proxima corrida!\n", ARQUIVO);
    }
    destruir(raiz);
    return 0;
}
