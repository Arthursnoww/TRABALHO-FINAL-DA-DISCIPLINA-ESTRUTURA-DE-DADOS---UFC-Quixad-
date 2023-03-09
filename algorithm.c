#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdarg.h> // que permite que funções utilizem um número indefinido de argumentos sem que a quantidade e o tipo sejam especificadas
#include <ctype.h> // essa biblioteca sera usada para declara várias funções que são úteis para testar e mapear caracteres.

// tipos de variaveis que utilizaremos nas formatação de operaçoes
typedef enum { num = 0, par1, par2, ope,  ult, fim, invalido } Variaveis;
// par1 e par2 serao os parenteses; ope serao os operandos; ult sera o \n da operaçao; fim sera quando a operacao nao tiver continuidade
// invalido sera para caracteres invalidos

// estrutura da operação
typedef struct {
	Variaveis calc;
	union {
		double numero;
		char operando;
	};
} operacao;


// biblioteca de funçoes
int ler_operacoes(operacao *);
bool operandos(char);
bool empilhar(char, char);
int aplicacoes(int, int, char, double*);
int precedencia(char);
void reportar_erro(const char*, ...);

// manipulacao de caracteres do vetor de char
int encontrar(){
	int c = getchar();  // getchar: A função retorna o caracter lido, após convertê-lo para int
	if (c != EOF)  // EOF : fim do arquivo ou fim de dados
		ungetc(c, stdin); //ungetc: pega um único caractere e o empurra de volta para pilha
	return c;
}

// encontrar operaçoes de formatação e organizalas - A ALMA DO CODIGO
int ler_operacoes(operacao* nova){
	char vet[100]; 
	int tam; // tamanho da operação
	int ir;  // ultimo operando lido

	while ((ir = getchar()) == ' ' || ir == '\t')
		;

	if (!isdigit(ir) && !(ir == '.' && isdigit(encontrar()))) { // isdigit: O método retorna True se todos os caracteres são dígitos, caso contrário, False
		switch (ir) {
			case '\n':
				nova->calc = ult;
				break;
			case EOF:
				nova->calc = fim;
				break;
			case '(':
				nova->calc = par1;
				break;
			case ')':
				nova->calc = par2;
				break;
			default:
				if (operandos(ir))
					nova->calc = ope;
				else
					nova->calc = invalido;
				break;
		};
		nova->operando = ir;
		return 0;
	} 

    // se operaçao exceder o limite
	for (tam = 0; isdigit(ir); ir = getchar()) {
		if (tam >= 100) {
			reportar_erro("tamanho maximo de operação excedido");
			return 1;
		}
		vet[tam++] = ir;
	}

    // encontrar parte decimal
	if (ir == '.' && isdigit(encontrar())) { 
		if (tam >= 100) {
			reportar_erro("tamanho maximo de operação exedido");
			return 1;
		}
		vet[tam++] = ir;
		while (isdigit(ir = getchar()));
	}

	vet[tam] = '\0'; //o último char lido faz parte da próxima operação
	if (ir != EOF)
		ungetc(ir, stdin);	

	nova->calc = num;
	nova->numero = atof(vet); // atof: converte caractere da string em um número de ponto flutuante
	return 0;
}

// operaçoes aritmedicas suportadas
bool operandos(char op){
	return op == '+' || op == '-' || op == '*' || op == '/' || op == 'm' || op == '^' || op == 'r' || op == 'l';
}

// definir a prioridade de cada operaçao
int precedencia(char op){
	switch (op) {
	case '\n':
		return -1;
	case '(':
		return 0;
	case ')':
		return 1;
	case '+':
	case '-':
		return 2;
	case '*':
	case '/':
	case 'm':
		return 3;
	case '^':
	case 'l':
	case 'r':
		return 4;
	default:
		return -1;
	}
}

// organizar operandos de acordo com a  precedencia
bool empilhar(char op, char novo_op){
	return (precedencia(op) > precedencia(novo_op)) ||
		   ((precedencia(op) == precedencia(novo_op)) &&
	        (op == '-' || op == '/'));
}

// definir formula de cada operando
int aplicacoes(int op1, int op2, char operadores, double *formula){
	switch (operadores) {
	case '+':
		*formula = op1 + op2;
		break;
	case '-':
		*formula = op1 - op2;
		break;
	case '*':
		*formula = op1 * op2;
		break;
	case '/':
		if (op2 == 0) {
			reportar_erro("divisao por 0");
			return 1;
		}
		*formula = op1 / op2;
		break;
	case 'm':
		*formula = op1 % op2;
		break;
	case '^':
		*formula = pow(op1, op2);
		break;
	case 'l':
		*formula = log10(op1);
		break;
	case 'r':
		*formula = sqrt(op1);
		break;
	default:
		break;
	}

	return 0;
}

// explicita erros e apresentar pro usuario de forma compacta
void reportar_erro(const char* a, ...) {  //const especifica que um valor de variável é constante e informa 
	va_list args;                         // o compilador para impedir que o programador a modifique
	va_start(args, a);
    // va_list: usado como um parâmetro para as macros definidas em para recuperar os argumentos adicionais de uma funçao
    // va_start: Inicializa para recuperar os argumentos adicionais após o parâmetro
	fprintf(stderr, "ERROR: "); 
	vfprintf(stderr, a, args);   // Stderr é usado para imprimir o erro na tela de saída ou no terminal da janela
	fprintf(stderr, "\n");
}

int main(void){
	double numero[100];   
	char operando[100]; 

	int aux1 = 0;               
	int aux2 = 0;               
	bool falso_aux;
	char c;
	double op1, op2; // operandos
	operacao nova;
	bool num_aux;
	falso_aux = false;
	num_aux = 1;
    
	printf("REDIJA SUA OPERAÇAO: ");
	printf("\n> ");

	while (!falso_aux) {
		if (ler_operacoes(&nova)){ // ler operação
			goto controle; // goto: transfere o controle para um rótulo e  fornece uma construção para fazer um salto incondicional.
        }
		if (num_aux){ // analisar casos de possiveis erros
			switch(nova.calc) {
			case num:
				if (aux1 == 100) {
					reportar_erro("quantidade excede limite da pilha");
					goto controle;
				}
				numero[aux1++] = nova.numero;
				num_aux = 0;
				break;
			case par1:
				if (aux2 == 100) {
					reportar_erro("quantidade excede limite da pilha");
					goto controle;
				}
				operando[aux2++] = nova.operando;
				num_aux = 1;
				break;
			case ope: 
				if (nova.operando == '-') {
					if (aux1 == 100) {
						reportar_erro("quantidade excede limite da pilha");
						goto controle;
					}
					numero[aux1++] = 0;

					if (aux2 == 100) {
						reportar_erro("quantidade excede limite da pilha");
						goto controle;
					}
					operando[aux2++] = '-';
				} 
				break;
			case fim:
				falso_aux = true;
				break;
			default:
				reportar_erro("operaçao invalida ou incompleta");
				goto controle;
				break;
			}
		} else { // caso em que a operação deve esta incompleta
			if (nova.calc != ope && nova.calc != par2 && nova.calc != ult) {
				reportar_erro("operaçao invalida ou incompleta");
				goto controle;
			}

			// operadores com precedencia maior que operando 
			while (aux2 > 0 && empilhar(operando[aux2 - 1], nova.operando)) {
				if (aux1 < 2) {
					reportar_erro("operaçao invalida ou incompleta");
					goto controle;
				}

				op2 = numero[--aux1];
				op1 = numero[--aux1];

				if (aplicacoes(op1, op2, operando[--aux2], &numero[aux1++]))
					goto controle;
			}

            // empurrar o operador incondicionalmente
			if (operandos(nova.operando)) {
				if (aux2 == 100) {
					reportar_erro("operaçao invalida ou incompleta");
					goto controle;
				}

				operando[aux2++] = nova.operando;
				num_aux = 1;
			} 
            // verificar operando no topo da pilha
            else if (nova.calc == par2) {
				if (aux2 < 1) {
					reportar_erro("incomparável')'");
					goto controle;
				}
				aux2 -= 1;
				num_aux = 0;
			} else if (nova.calc == ult) {
				if (aux1 == 1) {
					printf("RESULTADO: %.2f\n", numero[--aux1]);
				} else if (aux1 != 0) {
					/* aux1 != 0 porque a última expressão poderia ter retornado
					   nada e isso não seria um erro */
					reportar_erro("pilha numérica mal formatada");
					goto controle;
				}

				num_aux = 1;
				printf("\n> ");
			}
		}

		continue;
		controle:
		if (nova.calc != ult) // se ainda houver entradas
			while ((c = getchar()) != '\n' && c != EOF)
				;

		aux2 = aux1 = 0;
		num_aux = false;
		ungetc('\n', stdin); 
    }
	putchar('\n'); // putchar: é usado para escrever um caractere, do tipo char não assinado

	return 0;
}
