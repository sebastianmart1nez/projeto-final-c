#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <curl/curl.h>

#define MAX_TENTATIVAS 6
#define MAX_PALAVRAS 100
#define MAX_LETRAS 50
#define FICHEIRO_PALAVRAS "palavras.txt"

struct Memoria {
    char *dados;
    size_t tamanho;
};

void boneco_forca(int erros);
void limpar_tela(void);
void atualizar_placar(int ganhou);
int carregar_palavras(const char *filename, char palavras[][MAX_LETRAS]);
void salvar_palavras(const char *filename, char palavras[][MAX_LETRAS], int total);
int criar_ficheiro(const char *filename, char palavras[][MAX_LETRAS]);
void remover_acentos(char *str);
void normalizar_palavra(char *dest, const char *src);
int palavra_existe(const char *palavra, char palavras[][MAX_LETRAS], int total);
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream);
int buscar_palavras_internet(char palavras[][MAX_LETRAS], int *total);
void jogar(char palavras[][MAX_LETRAS], int num_palavras);
void limpar_buffer(void);

int main() {
      char palavras[MAX_PALAVRAS][MAX_LETRAS];
    int num_palavras;

    num_palavras = carregar_palavras(FICHEIRO_PALAVRAS, palavras);
    if (num_palavras <= 0) {
        num_palavras = criar_ficheiro(FICHEIRO_PALAVRAS, palavras);
        printf("\nFicheiro '%s' criado com %d palavras padrao.\n", FICHEIRO_PALAVRAS, num_palavras);
    }

    srand(time(NULL));

    int opcao;
    do {
        limpar_tela();
        printf("\n === JOGO DA FORCA ===\n");
        printf("\n [1] Jogar");
        printf("\n [2] Descarregar Novas Palavras");
        printf("\n [3] Sair");
        printf("\n\n Palavras disponiveis: %d/%d", num_palavras, MAX_PALAVRAS);
        printf("\n\n Escolhe uma opcao: ");

        scanf("%d", &opcao);
        limpar_buffer();

        switch (opcao) {
            case 1:
                if (num_palavras > 0) {
                    jogar(palavras, num_palavras);
                } else {
                    printf("\nNao ha palavras carregadas!\n");
                    printf("\nPrima Enter para continuar...");
                    getchar();
                }
                break;

            case 2: {
                printf("\nA descarregar palavras da internet...\n");
                int novas = buscar_palavras_internet(palavras, &num_palavras);
                if (novas > 0) {
                    salvar_palavras(FICHEIRO_PALAVRAS, palavras, num_palavras);
                    printf("\nAdicionadas %d palavras novas! (Total: %d)\n", novas, num_palavras);
                } else if (num_palavras >= MAX_PALAVRAS) {
                    printf("\nO array ja esta cheio (%d palavras).\n", MAX_PALAVRAS);
                } else {
                    printf("\nNao foram encontradas palavras novas.\n");
                }
                printf("\nPrima Enter para continuar...");
                getchar();
                break;
            }

            case 3:
                printf("\nObrigado por jogar!\n");
                break;

            default:
                printf("\nOpcao invalida! Tenta de novo.\n");
                printf("\nPrima Enter para continuar...");
                getchar();
        }
    } while (opcao != 3);

    return 0;
}

void jogar(char palavras[][MAX_LETRAS], int num_palavras) {
    char jogar_novamente;

    do {
        char *palavra_secreta = palavras[rand() % num_palavras];
        int tamanho = strlen(palavra_secreta);
        char palavra_descoberta[MAX_LETRAS];
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
            for (int i = 0; i < tamanho; i++) printf("%c ", palavra_descoberta[i]);

            printf("\nErros: %d/%d", erros, MAX_TENTATIVAS);
            printf("\nLetras tentadas: %s", letras_tentadas);
            printf("\nEscreve uma letra: ");

            char palpite;
            scanf(" %c", &palpite);
            palpite = toupper(palpite);

            int ja_tentou = 0;
            for (int i = 0; i < num_tentativas; i++) {
                if (letras_tentadas[i] == palpite) {
                    ja_tentou = 1;
                    break;
                }
            }

            if (ja_tentou) continue;
            letras_tentadas[num_tentativas++] = palpite;

            int acertou = 0;
            for (int i = 0; i < tamanho; i++) {
                if (palavra_secreta[i] == palpite) {
                    palavra_descoberta[i] = palpite;
                    acertou = 1;
                }
            }

            if (!acertou) erros++;
            if (strcmp(palavra_secreta, palavra_descoberta) == 0) ganhaste = 1;
        }

        limpar_tela();
        boneco_forca(erros);

        if (ganhaste) {
            printf("\nParabens!, ganhaste\n");
            atualizar_placar(1);
        } else {
            printf("\nPerdeste!, a palavra era: %s\n", palavra_secreta);
            atualizar_placar(0);
        }

        printf("\nQueres jogar de novo? (S/N): ");
        scanf(" %c", &jogar_novamente);
        limpar_buffer();

    } while (toupper(jogar_novamente) == 'S');
}

int carregar_palavras(const char *filename, char palavras[][MAX_LETRAS]) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;

    int total = 0;
    char linha[MAX_LETRAS];
    while (total < MAX_PALAVRAS && fgets(linha, sizeof(linha), f)) {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (strlen(linha) > 0) {
            strcpy(palavras[total], linha);
            total++;
        }
    }
    fclose(f);
    return total;
}

void salvar_palavras(const char *filename, char palavras[][MAX_LETRAS], int total) {
    FILE *f = fopen(filename, "w");
    if (!f) return;

    for (int i = 0; i < total; i++) {
        fprintf(f, "%s\n", palavras[i]);
    }
    fclose(f);
}

int criar_ficheiro(const char *filename, char palavras[][MAX_LETRAS]) {
    const char *defaults[] = {"COMPUTADOR", "INVERNO", "PROGRAMACAO", "ESTRELA", "ESCOLA"};
    int total = sizeof(defaults) / sizeof(defaults[0]);

    for (int i = 0; i < total; i++) {
        strcpy(palavras[i], defaults[i]);
    }
    salvar_palavras(filename, palavras, total);
    return total;
}

void remover_acentos(char *str) {
    unsigned char *r = (unsigned char *)str;
    unsigned char *w = (unsigned char *)str;

    while (*r) {
        if (*r == 0xC3 && *(r + 1)) {
            unsigned char n = *(r + 1);
            unsigned char c = 0;

            if      (n >= 0x80 && n <= 0x84) c = 'A';
            else if (n >= 0xA0 && n <= 0xA4) c = 'A';
            else if (n >= 0x88 && n <= 0x8B) c = 'E';
            else if (n >= 0xA8 && n <= 0xAB) c = 'E';
            else if (n >= 0x8C && n <= 0x8F) c = 'I';
            else if (n >= 0xAC && n <= 0xAF) c = 'I';
            else if (n >= 0x92 && n <= 0x96) c = 'O';
            else if (n >= 0xB2 && n <= 0xB6) c = 'O';
            else if (n >= 0x99 && n <= 0x9C) c = 'U';
            else if (n >= 0xB9 && n <= 0xBC) c = 'U';
            else if (n == 0x87 || n == 0xA7) c = 'C';

            if (c) {
                *w++ = c;
                r += 2;
            } else {
                *w++ = *r++;
                *w++ = *r++;
            }
        } else {
            *w++ = *r++;
        }
    }
    *w = '\0';
}

void normalizar_palavra(char *dest, const char *src) {
    char limpa[MAX_LETRAS];
    int j = 0;

    for (int i = 0; src[i] && j < MAX_LETRAS - 2; i++) {
        unsigned char c = (unsigned char)src[i];
        if ((c == 0xC3 || c == 0xC4 || c == 0xC5 || c == 0xC7) && src[i + 1]) {
            limpa[j++] = c;
            limpa[j++] = src[++i];
        } else if (isalpha(c)) {
            limpa[j++] = c;
        }
    }
    limpa[j] = '\0';

    remover_acentos(limpa);

    char final[MAX_LETRAS];
    j = 0;
    for (int i = 0; limpa[i] && j < MAX_LETRAS - 1; i++) {
        char c = limpa[i];
        if (c >= 'A' && c <= 'Z') {
            final[j++] = c;
        } else if (c >= 'a' && c <= 'z') {
            final[j++] = c - 32;
        }
    }
    final[j] = '\0';

    strcpy(dest, final);
}

int palavra_existe(const char *palavra, char palavras[][MAX_LETRAS], int total) {
    for (int i = 0; i < total; i++) {
        if (strcmp(palavras[i], palavra) == 0)
            return 1;
    }
    return 0;
}

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t total = size * nmemb;
    struct Memoria *mem = (struct Memoria *)stream;

    char *temp = realloc(mem->dados, mem->tamanho + total + 1);
    if (!temp) return 0;

    mem->dados = temp;
    memcpy(mem->dados + mem->tamanho, ptr, total);
    mem->tamanho += total;
    mem->dados[mem->tamanho] = '\0';

    return total;
}

int buscar_palavras_internet(char palavras[][MAX_LETRAS], int *total) {
    CURL *curl;
    CURLcode res;
    int novas = 0;

    const char *prefixos[] = {
        "des", "pro", "res", "tra", "pre", "ind", "por",
        "imp", "sob", "sub", "int", "apa", "com", "con",
        "cor", "car", "cas", "cam", "cal", "can", "col",
        "abs", "aco", "age", "agu", "ala", "ali", "ame",
        "amo", "ani", "apo", "art", "ass", "atu", "aut",
        "avi", "bar", "bas", "bra", "cav", "cer", "cid",
        "cin", "cir", "cit", "cla", "cli", "coa", "coe",
        "cos", "cra", "cre", "cri", "cro", "cru", "cua",
        "cub", "cui", "cum", "cun", "cur"
    };
    int num_prefixos = sizeof(prefixos) / sizeof(prefixos[0]);
    int max_por_prefixo = 15;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        curl_global_cleanup();
        return 0;
    }

    for (int p = 0; p < num_prefixos && *total < MAX_PALAVRAS; p++) {
        char url[256];
        snprintf(url, sizeof(url), "https://api.dicionario-aberto.net/prefix/%s", prefixos[p]);

        struct Memoria chunk = {NULL, 0};

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "forca-game/1.0");

        res = curl_easy_perform(curl);

        if (res == CURLE_OK && chunk.dados) {
            int neste_prefixo = 0;
            char *pos = chunk.dados;
            while ((pos = strstr(pos, "\"word\":\"")) && *total < MAX_PALAVRAS
                   && neste_prefixo < max_por_prefixo) {
                pos += 8;
                char *fim = strchr(pos, '"');
                if (!fim || fim - pos >= MAX_LETRAS) break;

                int len = fim - pos;
                if (len >= 2) {
                    char palavra_bruta[MAX_LETRAS];
                    strncpy(palavra_bruta, pos, len);
                    palavra_bruta[len] = '\0';

                    char palavra_limpa[MAX_LETRAS];
                    normalizar_palavra(palavra_limpa, palavra_bruta);

                    if (strlen(palavra_limpa) >= 3 &&
                        !palavra_existe(palavra_limpa, palavras, *total)) {
                        strcpy(palavras[*total], palavra_limpa);
                        (*total)++;
                        novas++;
                        neste_prefixo++;
                        printf(".");
                        fflush(stdout);
                    }
                }
                pos = fim + 1;
            }
        }

        free(chunk.dados);
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return novas;
}

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void atualizar_placar(int resultado) {
    static int vitorias = 0;
    static int derrotas = 0;

    if (resultado == 1) vitorias++;
    else if (resultado == 0) derrotas++;

    printf("\n-----------------------");
    printf("\n Placar: %d VIT | %d DER", vitorias, derrotas);
    printf("\n-----------------------\n");
}

void boneco_forca(int erros) {
    printf("  +---+\n");
    printf("  |   |\n");
    printf("  |   %c\n", (erros >= 1 ? '0' : ' '));
    printf("  |  %c%c%c\n", (erros >= 3 ? '/' : ' '), (erros >= 2 ? '|' : ' '), (erros >= 4 ? '\\' : ' '));
    printf("  |  %c %c\n", (erros >= 5 ? '/' : ' '), (erros >= 6 ? '\\' : ' '));
    printf(" _|_  \n");
}

void limpar_tela(void) {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}
