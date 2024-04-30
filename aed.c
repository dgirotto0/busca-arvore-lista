#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO 1000

typedef struct Palavra {
    char *palavra;
    int *linha;
    int repeticoes;
    struct Palavra *proximo;
} Palavra;

typedef struct noArvore {
    char *palavra;
    int *linhas;
    int ocorrencias;
    struct noArvore *esquerda;
    struct noArvore *direita;
} noArvore;

typedef struct {
    char *linha;
    int indice;
} Linhas;

void liberaArvore(noArvore *raiz) {
    if (raiz != NULL) {
        liberaArvore(raiz->esquerda);
        liberaArvore(raiz->direita);
        free(raiz->palavra);
        free(raiz->linhas);
        free(raiz);
    }
}

void liberaLista(Palavra *inicio) {
    Palavra *att = inicio;
    while (att != NULL) {
        Palavra *temp = att;
        att = att->proximo;
        free(temp->palavra);
        free(temp->linha);
        free(temp);
    }
}

void corrige(char *str) {

  for (size_t i = 0; i < strlen(str); i++) {
    str[i] = tolower((unsigned char)str[i]);
  }
}

void corrigeLinhas(char **linhas) {
  for (int i = 0; i < TAMANHO - 1; i++) {
    corrige(linhas[i]);
  }
}

Palavra *criaPalavraLigada(const char *palavra, int *linha) {
  Palavra *nova = (Palavra *)malloc(sizeof(Palavra));
  if (nova == NULL) {
    printf("Erro ao alocar memória.\n");
    exit(EXIT_FAILURE);
  }

  nova->palavra = strdup(palavra);
  if (nova->palavra == NULL) {
    printf("Erro ao alocar memória.\n");
    exit(EXIT_FAILURE);
  }

  nova->linha = (int *)malloc(sizeof(int));
  if (nova->linha == NULL) {
    printf("Erro ao alocar memória.\n");
    exit(EXIT_FAILURE);
  }

  nova->linha[0] = *linha;
  nova->repeticoes = 1;

  return nova;
}

Palavra *adicionaPalavraLista(Palavra *inicio, const char *palavra, int *linha) {
    Palavra *novo = criaPalavraLigada(palavra, linha);

    if (inicio == NULL || strcmp(palavra, inicio->palavra) < 0) {
        novo->proximo = inicio;
        return novo;
    }

    Palavra *atual = inicio;
    Palavra *anterior = NULL;

    while (atual != NULL && strcmp(palavra, atual->palavra) > 0) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual != NULL && strcmp(palavra, atual->palavra) == 0) {
        // Palavra já existe na lista, atualizar ocorrências e linhas
        atual->repeticoes++;
        atual->linha = (int *)realloc(atual->linha, sizeof(int) * atual->repeticoes);
        atual->linha[atual->repeticoes - 1] = *linha;
        free(novo->palavra);
        free(novo->linha);
        free(novo);
    } else {
        // Palavra não existe na lista, adicionar novo nó
        novo->proximo = atual;
        if (anterior != NULL) {
            anterior->proximo = novo;
        } else {
            // Se anterior for NULL, o novo nó é o primeiro da lista
            inicio = novo;
        }
    }

    return inicio;
}

Palavra *PalavraExisteLigada(Palavra *inicio, char *palavra, int num_linha) {
  Palavra *copia = inicio;
  int ocorrencias = 1;

  while (copia != NULL) {
    if (strcmp(copia->palavra, palavra) == 0) {
      ocorrencias++;
      copia->repeticoes = ocorrencias;
      copia->linha = (int *)realloc(copia->linha, sizeof(int) * ocorrencias);
      if (copia->linha == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(EXIT_FAILURE);
      }
      copia->linha[ocorrencias - 1] = num_linha;
      return copia;
    }
    copia = copia->proximo;
  }
  return NULL;
}

void buscaLigada(Palavra *inicio, char *palavraBuscada) {
  int linhas[TAMANHO];
  int numOcorrencias = 0;

  for (Palavra *copia = inicio; copia != NULL; copia = copia->proximo) {
    if (strcmp(copia->palavra, palavraBuscada) == 0) {
      linhas[numOcorrencias++] = copia->linha[0];
    }
  }

  if (numOcorrencias == 0) {
    printf("Palavra %s nao encontrada.\n", palavraBuscada);
  } else {
    for (int i = 0; i < numOcorrencias; i++) {
      int linha = linhas[i];
      
      // Declaração da variável 'copia' dentro do for
      Palavra *copia = inicio; 
      
      if (linhas[i] == linhas[i - 1]) {
        continue;
      }
      printf("Palavra '%s' encontrada %d vezes na linha: ", palavraBuscada, copia->repeticoes);
      printf("%d\n", linhas[i]);
    }
  }
}

noArvore *raiz = NULL;

noArvore *criarNo(const char *palavra, int linha) {
  noArvore *novoNo = (noArvore *)malloc(sizeof(noArvore));
  novoNo->palavra = strdup(palavra);
  novoNo->ocorrencias = 1;
  novoNo->linhas = (int *)malloc(sizeof(int));
  novoNo->linhas[0] = linha;
  // novoNo->proxima = NULL;
  return novoNo;
}

noArvore *inserePalavra(noArvore *raiz, const char *palavra, int linha) {

  if (raiz == NULL) {
    noArvore *novoNo = criarNo(palavra, linha); //////////////
    novoNo->linhas = (int *)malloc(sizeof(int));
    novoNo->linhas[0] = linha;
    novoNo->ocorrencias = 1;
    return novoNo;
  }

  int comparaResultado = strcmp(raiz->palavra, palavra);
  if (comparaResultado == 0) {
    // A palavra já existe, incrementa as ocorrências e adiciona a linha
    raiz->ocorrencias++;
    raiz->linhas =
        (int *)realloc(raiz->linhas, sizeof(int) * raiz->ocorrencias);
    raiz->linhas[raiz->ocorrencias - 1] = linha;
  } else if (comparaResultado < 0) {
    raiz->esquerda = inserePalavra(raiz->esquerda, palavra, linha);
  } else {
    raiz->direita = inserePalavra(raiz->direita, palavra, linha);
  }

  return raiz;
}

noArvore *buscaArvore(noArvore *raiz, const char *palavra) {
  if (raiz == NULL || strcmp(palavra, raiz->palavra) == 0) {
    return raiz;
  }

  int comparaResultado = strcmp(raiz->palavra, palavra);
  if (comparaResultado < 0) {
    return buscaArvore(raiz->esquerda, palavra);
  } else {
    return buscaArvore(raiz->direita, palavra);
  }
}

void resultadoArvore(noArvore *raiz, const char *palavra, Linhas *linhas) {
  if (raiz != NULL) {
    int comparaResultado = strcmp(raiz->palavra, palavra);
    if (comparaResultado == 0) {
      printf("Existem %d ocorrências da palavra '%s' na(s) seguinte(s) "
             "linha(s):\n",
             raiz->ocorrencias, palavra);
      for (int i = 0; i < raiz->ocorrencias; i++) {
        int linha = raiz->linhas[i];
        printf("%05d: %s\n", linha, linhas[linha - 1].linha);
      }
    } else if (comparaResultado < 0) {
      resultadoArvore(raiz->esquerda, palavra, linhas);
    } else {
      resultadoArvore(raiz->direita, palavra, linhas);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Uso: %s <nome_arquivo> <tipo_indice>\n", argv[0]);
    return 1;
  }

  char *linha_aux;
  FILE *in;
  char *linha;
  char *quebra_de_linha;
  Linhas *linhas = NULL;
  int contador_linha;
  Palavra *novaPalavra = NULL;
  Palavra *indice = NULL;
  clock_t tempo = clock();
  char *guardaPonteiro;

  in = fopen(argv[1], "r");

  if (!in) {
    perror("Erro ao abrir o arquivo");
    return 1;
  }

  contador_linha = 0;
  linha = (char *)malloc((TAMANHO + 1) * sizeof(char));

  // Contando o número de linhas no arquivo
  while (fgets(linha, TAMANHO, in)) {
    if ((quebra_de_linha = strrchr(linha, '\n')))
      *quebra_de_linha = 0;

    contador_linha++;
  }

  // Movendo o cursor de volta para o início do arquivo
  rewind(in);

  linhas = (Linhas *)malloc(sizeof(Linhas) * contador_linha);

  // Lendo e armazenando as linhas e índices
  for (int i = 0; i < contador_linha; i++) {

    if (!fgets(linha, TAMANHO, in)) {
      perror("Erro ao ler uma linha do arquivo");
      fclose(in);
      free(linha);
      free(linhas);
      return 1;
    }

    if ((quebra_de_linha = strrchr(linha, '\n')))
      *quebra_de_linha = 0;

    linhas[i].linha = strdup(linha);
    linhas[i].indice = i + 1;

    printf("linha %03d: %s\n", linhas[i].indice, linhas[i].linha);
  }

  printf("\n\n");

  printf("Tipo de indice: '%s'\n", argv[2]);
  printf("Arquivo texto: <%s>\n", argv[1]);

  tempo = clock() - tempo;

  printf("tempo: %lf ms\n", ((double)tempo) / CLOCKS_PER_SEC);

  linha_aux = (char *)malloc((TAMANHO + 1) * sizeof(char));

  rewind(in);

  // Lê o arquivo linha por linha
  printf("%d\n", contador_linha);

  for (int i = 0; i < contador_linha; i++) {

    guardaPonteiro = strdup(linhas[i].linha);

    // insere palavra na arvore
    char *palavra_aux;
    char *resto_da_linha = guardaPonteiro;

    printf("%s", guardaPonteiro);

    while ((palavra_aux = strtok(guardaPonteiro, " "))) {

      raiz = inserePalavra(raiz, palavra_aux, i + 1);

      Palavra *contador = indice;
      Palavra *anterior = NULL;

      while (contador != NULL && strcmp(contador->palavra, palavra_aux) != 0) {
        anterior = contador;
        contador = contador->proximo;
      }
      if (contador == NULL) {

        indice = adicionaPalavraLista(indice, palavra_aux, &i);

        if (novaPalavra != NULL) {
          if (anterior != NULL) {
            anterior->proximo = novaPalavra;
          } else {
            anterior = novaPalavra;
          }
        }
      } else {
        contador->repeticoes++;
        contador->linha =
            (int *)realloc(contador->linha, sizeof(int) * contador->repeticoes);
        contador->linha[contador->repeticoes - 1] = i + 1;
      }

      printf("\t\t'%s'\n", palavra_aux);
      guardaPonteiro = NULL;
      printf("%s", indice->palavra);
    }

    free(guardaPonteiro);
  }

  char tipoIndice[TAMANHO];
  strcpy(tipoIndice, argv[2]);

      char comando[TAMANHO];
      char palavraBuscada[TAMANHO];

while (1) {
    printf("> ");

    char comando[TAMANHO];
    char palavraBuscada[TAMANHO];

    // Use fgets para ler a linha completa, incluindo espaços
    if (fgets(comando, TAMANHO, stdin) == NULL) {
      printf("Erro ao ler o comando.\n");
      break;
    }

    // Remova o caractere de nova linha no final da string
    comando[strcspn(comando, "\n")] = '\0';

    if (strstr(comando, "busca") != NULL) {
      printf("Digite a palavra para busca: ");

      if (fgets(palavraBuscada, TAMANHO, stdin) == NULL) {
        printf("Erro ao ler a palavra de busca.\n");
        break;
      }

      // Remova o caractere de nova linha no final da string
      palavraBuscada[strcspn(palavraBuscada, "\n")] = '\0';

      tempo = clock();
      if (strcmp(tipoIndice, "arvore") == 0) {
        resultadoArvore(raiz, palavraBuscada, linhas);
      } else if (strcmp(tipoIndice, "lista") == 0) {
        buscaLigada(indice, palavraBuscada);
      } else {
        printf("Tipo de indice invalido!\n");
      }
      tempo = clock() - tempo;
      printf("Tempo de busca: %f ms\n", ((double)tempo) / CLOCKS_PER_SEC);
    } else if (strcmp(comando, "fim") == 0) {
      break;
    } else {
      printf("Comando invalido!\n");
    }
}

  // Liberando a memória alocada
  free(linha);
  free(linhas);
  liberaArvore(raiz);
  liberaLista(indice);

  // Fechando o arquivo
  fclose(in);

  return 0;
}
