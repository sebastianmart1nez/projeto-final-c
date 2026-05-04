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