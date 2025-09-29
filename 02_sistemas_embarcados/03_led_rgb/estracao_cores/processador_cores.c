#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include "stb_image.h"

int **criar_matriz(int linhas, int colunas)
{
    int **matriz;

    if (!(matriz = malloc(linhas * sizeof(int *))))
    {
        printf("Erro ao alocar memoria\n");
        exit(1);
    }

    for (int i = 0; i < linhas; i++)
    {
        if (!(matriz[i] = malloc(colunas * sizeof(int))))
        {
            printf("Erro ao alocar memoria\n");
            exit(1);
        }
    }

    printf("Matriz criada com sucesso\n");
    return matriz;
}

/**
 * @brief Imprime uma matriz 2D no formato de uma declaração de array em C.
 * * @param matriz A matriz de inteiros a ser impressa.
 * @param linhas O número de linhas na matriz (deve ser 3).
 * @param colunas O número de colunas na matriz (deve ser 144).
 */
void imprimir_matriz_para_declaracao(int** matriz, int linhas, int colunas) {
    if (!matriz) {
        printf("Erro: a matriz é NULL.\n");
        return;
    }

    // Início da declaração do array
    printf("int matriz_cores[3][144] = {\n");

    // Itera sobre cada linha (R, G, B)
    for (int i = 0; i < linhas; i++) {
        // Início da linha
        printf("    {"); 

        // Itera sobre cada coluna (as 144 cores)
        for (int j = 0; j < colunas; j++) {
            // Adiciona uma quebra de linha e indentação a cada 12 valores para legibilidade
            if (j % 12 == 0) {
                printf("\n        ");
            }
            // Imprime o valor do pixel
            printf("%3d", matriz[i][j]);

            // Adiciona uma vírgula, exceto para o último elemento
            if (j < colunas - 1) {
                printf(", ");
            }
        }
        
        // Fim da linha
        printf("\n    }");

        // Adiciona uma vírgula entre as linhas R, G e B
        if (i < linhas - 1) {
            printf(",\n");
        }
    }

    // Fim da declaração do array
    printf("\n};\n");
}

int **liberar_matriz(int **matriz, int linhas, int colunas)
{
    if (matriz)
    {
        for (int i = 0; i < linhas; i++)
        {
            free(matriz[i]);
        }
        free(matriz);
    }
    return NULL;
}

int main()
{
    int largura, altura, canais;
    int **matriz;
    int coluna_atual = 0;

    unsigned char *imagem = stbi_load("./teste.png", &largura, &altura, &canais, 3);

    if (!imagem)
    {
        printf("Erro ao carregar a imagem\n");
        return 1;
    }

    printf("Largura: %d\nAltura: %d\nNumero Canais: %d\n", largura, altura, canais);

    stbi_image_free(imagem);

    matriz = criar_matriz(3, 144);

    if (!matriz)
    {
        printf("Erro ao criar matriz\n");
        return 1;
    }

    for (int i = 0; i < 12; i++)
    { // 'i' representa a linha na grade
        for (int j = 0; j < 12; j++)
        { // 'j' representa a coluna na grade
            int ponto_x = (j * largura / 12) + (largura / 24);
            int ponto_y = (i * altura / 12) + (altura / 24);
            int indice = (ponto_y * largura + ponto_x) * 3;
            unsigned char r = imagem[indice];
            unsigned char g = imagem[indice + 1];
            unsigned char b = imagem[indice + 2];
            matriz[0][coluna_atual] = r; // Linha 0 para Vermelho
            matriz[1][coluna_atual] = g; // Linha 1 para Verde
            matriz[2][coluna_atual] = b; // Linha 2 para Azul
            coluna_atual++;
        }
    }

    imprimir_matriz_para_declaracao(matriz, 3, 144);

    matriz = liberar_matriz(matriz, 3, 144);

    return 0;
}