#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TENTATIVAS 6
#define NUM_PALAVRAS 5

void boneco_forca(int erros);
void limpar_tela(void);
void atualizar_placar(int ganhou);

int main() {
    char palavras[NUM_PALAVRAS][20] = {"COMPUTADOR", "INVERNO", "PROGRAMACAO", "ESTRELA", "ESCOLA"};
    char jogar_novamente;

    srand(time(NULL));

    do {
        char *palavra_secreta = palavras[rand() % NUM_PALAVRAS];
        int tamanho = strlen(palavra_secreta);
        char palavra_descoberta[20];
        char letras_tentadas[26] = {0};
        int num_tentativas = 0;
        int erros = 0;
        int ganhaste = 0;

        for (int i = 0; i < tamanho; i++) palavra_descoberta[i] = '-';
        palavra_descoberta[tamanho] = '\0';

        while (erros < MAX_TENTATIVAS && !ganhaste) {
            limpar_tela();
            atualizar_placar(-1);

            printf("\n --- JOGO DA FORCA ---\n");
            boneco_forca(erros);

            printf("\nPalavra: ");
            for(int i = 0; i < tamanho; i++) printf("%c ", palavra_descoberta[i]);

            printf("\nErros: %d/%d", erros, MAX_TENTATIVAS);
            printf("\nLetras tentadas: %s", letras_tentadas);
            printf("\nEscreve uma letra: ");

            char palpite;
            scanf(" %c", &palpite);
            palpite = toupper(palpite);

            int ja_tentou = 0;
            for(int i = 0; i < num_tentativas; i++){
                if (letras_tentadas[i] == palpite){
                    ja_tentou = 1;
                    break;
                }
            }

            if (ja_tentou) continue;
            letras_tentadas[num_tentativas++] = palpite;
            
            int acertou = 0;
            for (int i = 0; i < tamanho; i++){
                if (palavra_secreta[i] == palpite){
                    palavra_descoberta[i] = palpite;
                    acertou = 1;
                }
            }

            if (!acertou) erros++;
            if (strcmp(palavra_secreta, palavra_descoberta) == 0) ganhaste = 1;
        }

        limpar_tela();
        boneco_forca(erros);

        if(ganhaste){
            printf("\nParabens!, ganhaste\n");
            atualizar_placar(1);
        } else {
            printf("\nPerdeste!, a palavra era: %s\n", palavra_secreta);
            atualizar_placar(0);
        }

        printf("\nQueres jogar de novo? (S/N): ");
        scanf(" %c", &jogar_novamente);

    } while (tolower(jogar_novamente) == 's');

    printf("\nObrigado por jogar\n");
    return 0;
}

    void atualizar_placar(int resultado) {
        static int vitorias = 0;
        static int derrotas = 0;

        if(resultado == 1) vitorias++;
        else if(resultado == 0) derrotas++;

        printf("\n-----------------------");
        printf("\n Placar: %d VIT | %d DER", vitorias, derrotas);
        printf("\n-----------------------\n");
    }

    void boneco_forca(int erros){
        printf("  +---+\n");
        printf("  |   |\n");
        printf("  |   %c\n", (erros >= 1 ? '0' : ' '));
        printf("  |  %c%c%c\n", (erros >= 3 ? '/' : ' '), (erros >= 2 ? '|' : ' '), (erros >= 4 ? '\\' : ' '));
        printf("  |  %c %c\n", (erros >= 5 ? '/' : ' '), (erros >=6 ? '\\' : ' '));
        printf(" _|_  \n");
    }

    void limpar_tela(void){
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
    }
