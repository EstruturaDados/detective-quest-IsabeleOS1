#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ============================================================
// DEFINIÇÕES E ESTRUTURAS
// ============================================================

// 🧠 Nível Mestre: Configuração da Hash
#define TAM_HASH 10

// Estrutura para Suspeito (Elemento da Hash)
typedef struct Suspeito {
    char nome[50];
    int contagemPistas;     // Quantas pistas apontam para ele
    struct Suspeito *prox;  // Tratamento de colisão (Lista Encadeada)
} Suspeito;

// 🔍 Nível Aventureiro: Estrutura da Árvore de Busca (BST) para Pistas
typedef struct NoPista {
    char texto[100];
    struct NoPista *esq;
    struct NoPista *dir;
} NoPista;

// 🌱 Nível Novato: Estrutura da Árvore Binária para Salas
typedef struct Sala {
    char nome[50];
    struct Sala *esq;
    struct Sala *dir;
    int temPista;         // Flag para saber se há algo aqui
    char pistaTexto[100]; // O texto da pista, se houver
    char suspeitoAlvo[50]; // Quem essa pista incrimina
} Sala;

// Variáveis Globais
Suspeito* tabelaHash[TAM_HASH]; // Tabela de suspeitos
NoPista* raizPistas = NULL;     // Raiz da árvore de pistas

// ============================================================
// FUNÇÕES AUXILIARES (Visual e Util)
// ============================================================

void limparTela() {
    // Simula limpeza de tela
    for(int i=0; i<30; i++) printf("\n");
}

// ============================================================
// 🧠 NÍVEL MESTRE: TABELA HASH (Suspeitos)
// ============================================================

// Função de Hash Simples (Soma ASCII)
int funcaoHash(char *nome) {
    int soma = 0;
    for (int i = 0; nome[i] != '\0'; i++) {
        soma += toupper(nome[i]);
    }
    return soma % TAM_HASH;
}

void inicializarHash() {
    for (int i = 0; i < TAM_HASH; i++) {
        tabelaHash[i] = NULL;
    }
}

// Registra uma suspeita contra alguém
void registrarSuspeita(char *nomeSuspeito) {
    int indice = funcaoHash(nomeSuspeito);
    
    Suspeito *atual = tabelaHash[indice];
    
    // Procura se o suspeito já existe na lista encadeada deste índice
    while (atual != NULL) {
        if (strcmp(atual->nome, nomeSuspeito) == 0) {
            atual->contagemPistas++; // Incrementa evidência
            return;
        }
        atual = atual->prox;
    }

    // Se não existe, cria novo suspeito
    Suspeito *novo = (Suspeito*)malloc(sizeof(Suspeito));
    strcpy(novo->nome, nomeSuspeito);
    novo->contagemPistas = 1;
    novo->prox = tabelaHash[indice]; // Insere no início da lista (mais rápido)
    tabelaHash[indice] = novo;
}

void analisarCulpado() {
    printf("\n--- RELATORIO FINAL DO DETETIVE ---\n");
    char culpado[50] = "Ninguem";
    int maiorEvidencia = -1;

    for (int i = 0; i < TAM_HASH; i++) {
        Suspeito *atual = tabelaHash[i];
        while (atual != NULL) {
            printf("Suspeito: %-15s | Evidencias: %d\n", atual->nome, atual->contagemPistas);
            
            if (atual->contagemPistas > maiorEvidencia) {
                maiorEvidencia = atual->contagemPistas;
                strcpy(culpado, atual->nome);
            }
            atual = atual->prox;
        }
    }

    if (maiorEvidencia > 0) {
        printf("\n🕵️  CONCLUSAO: Tudo indica que o culpado e: ** %s **\n", culpado);
    } else {
        printf("\n🕵️  Inconclusivo. Voce nao coletou pistas suficientes!\n");
    }
}

// ============================================================
// 🔍 NÍVEL AVENTUREIRO: ÁRVORE DE BUSCA (Pistas)
// ============================================================

NoPista* inserirPistaBST(NoPista *raiz, char *texto) {
    if (raiz == NULL) {
        NoPista *novo = (NoPista*)malloc(sizeof(NoPista));
        strcpy(novo->texto, texto);
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }

    // Comparação alfabética para organizar
    if (strcmp(texto, raiz->texto) < 0) {
        raiz->esq = inserirPistaBST(raiz->esq, texto);
    } else if (strcmp(texto, raiz->texto) > 0) {
        raiz->dir = inserirPistaBST(raiz->dir, texto);
    }
    // Se for igual, ignora (evita duplicatas exatas)
    return raiz;
}

void listarPistasEmOrdem(NoPista *raiz) {
    if (raiz != NULL) {
        listarPistasEmOrdem(raiz->esq);
        printf(" - %s\n", raiz->texto);
        listarPistasEmOrdem(raiz->dir);
    }
}

// ============================================================
// 🌱 NÍVEL NOVATO: MAPA DA MANSÃO (Árvore Binária)
// ============================================================

Sala* criarSala(char *nome, char *pista, char *suspeito) {
    Sala *nova = (Sala*)malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    nova->esq = NULL;
    nova->dir = NULL;
    
    if (pista != NULL) {
        nova->temPista = 1;
        strcpy(nova->pistaTexto, pista);
        strcpy(nova->suspeitoAlvo, suspeito);
    } else {
        nova->temPista = 0;
    }
    return nova;
}

void explorarMansao(Sala *atual) {
    if (atual == NULL) {
        printf("\n🚫 Caminho bloqueado ou parede. Voltando...\n");
        return;
    }

    // Loop de navegação
    char escolha;
    do {
        printf("\n========================================\n");
        printf("📍 Voce esta em: %s\n", atual->nome);
        printf("========================================\n");

        // Checa se há pistas (Integração Aventureiro/Mestre)
        if (atual->temPista) {
            printf("🔍 VOCE ENCONTROU UMA PISTA: \"%s\"\n", atual->pistaTexto);
            printf("   (Adicionada ao caderno de anotacoes)\n");
            
            // Adiciona na BST (Organização)
            raizPistas = inserirPistaBST(raizPistas, atual->pistaTexto);
            
            // Adiciona na Hash (Investigação)
            registrarSuspeita(atual->suspeitoAlvo);
            
            // Remove a pista da sala para não pegar 2 vezes
            atual->temPista = 0; 
        }

        // Opções de Navegação
        printf("\nPara onde deseja ir?\n");
        if (atual->esq) printf("[e] Esquerda para: %s\n", atual->esq->nome);
        if (atual->dir) printf("[d] Direita para:  %s\n", atual->dir->nome);
        printf("[i] Ver Inventario de Pistas\n");
        printf("[s] Sair da Mansao (Encerrar caso)\n");
        printf("Opcao: ");
        scanf(" %c", &escolha);
        escolha = tolower(escolha);

        switch(escolha) {
            case 'e':
                if (atual->esq) explorarMansao(atual->esq);
                else printf("\nNao ha passagem a esquerda!\n");
                break;
            case 'd':
                if (atual->dir) explorarMansao(atual->dir);
                else printf("\nNao ha passagem a direita!\n");
                break;
            case 'i':
                printf("\n📖 --- CADERNO DE PISTAS (Ordem Alfabetica) ---\n");
                if (raizPistas == NULL) printf("(Vazio)\n");
                else listarPistasEmOrdem(raizPistas);
                break;
            case 's':
                return; // Sai da recursão
            default:
                printf("Opcao invalida!\n");
        }
    } while (escolha != 's');
}

// Configura o mapa estático da mansão
Sala* montarMapa() {
    // Criação das salas (Nós)
    Sala *hall = criarSala("Hall de Entrada", NULL, NULL);
    Sala *biblioteca = criarSala("Biblioteca", "Livro de Venenos rasgado", "Mordomo");
    Sala *cozinha = criarSala("Cozinha", "Faca suja de sangue", "Cozinheiro");
    Sala *jardim = criarSala("Jardim de Inverno", "Pegadas de lama tamanho 42", "Jardineiro");
    Sala *sotao = criarSala("Sotao Escuro", "Carta de ameaca antiga", "Mordomo"); // Mordomo de novo!
    
    // Conexões (Arestas da Árvore)
    //       [Hall]
    //      /      \
    // [Biblioteca] [Cozinha]
    //    /            \
    // [Sotao]       [Jardim]

    hall->esq = biblioteca;
    hall->dir = cozinha;
    
    biblioteca->esq = sotao;
    cozinha->dir = jardim;

    return hall;
}

// ============================================================
// MAIN
// ============================================================

int main() {
    inicializarHash();
    
    printf("\n🕵️  BEM-VINDO AO DETECTIVE QUEST 🕵️\n");
    printf("Um crime ocorreu. Explore a mansao, colete pistas e descubra o culpado.\n");
    
    Sala *mansao = montarMapa();
    
    // Inicia o jogo (Nível Novato)
    explorarMansao(mansao);

    // Final do Jogo (Nível Mestre)
    analisarCulpado();

    return 0;
}

