#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erros.h"

#define SIZE 512 				// Tamanho da p�gina.
#define PAGES 1024 				// N�mero de p�ginas.
#define TAMANHO_NOME_CAMPO 40 	// Tamanho do nome dos campos de uma tabela.
#define TAMANHO_NOME_TABELA 20 	// Tamanho do nome da tabela.
#define TAMANHO_NOME_ARQUIVO 20 // Tamanho do nome do arquivo.

struct fs_objects { // Estrutura usada para carregar fs_objects.dat
	char nome[TAMANHO_NOME_TABELA];		//  Nome da tabela.
	int cod;							// C�digo da tabela.
	char nArquivo[TAMANHO_NOME_ARQUIVO];// Nome do arquivo onde est�o armazenados os dados da tabela.
	int qtdCampos;						// Quantidade de campos da tabela.
};

typedef struct tp_table{ // Estrutura usada para carregar fs_schema.dat
	char nome[TAMANHO_NOME_CAMPO];	// Nome do Campo.
	char tipo;						// Tipo do Campo.
	int tam;						// Tamanho do Campo.
	struct tp_table *next;			// Encadeamento para o pr�ximo campo.
	int pky;                        // Chave prim�ria sim ou n�o "("1 ou 0")"
}tp_table;

typedef struct column{ // Estrutura utilizada para inserir em uma tabela, excluir uma tupla e retornar valores de uma p�gina.
	char tipoCampo;						// Tipo do Campo.
	char nomeCampo[TAMANHO_NOME_CAMPO];	//Nome do Campo.
	char *valorCampo;					// Valor do Campo.
	struct column *next;				// Encadeamento para o pr�ximo campo.
}column;

typedef struct table{ // Estrutura utilizada para criar uma tabela.
	char nome[TAMANHO_NOME_TABELA]; // Nome da tabela.
	tp_table *esquema;				// Esquema de campos da tabela.
}table;

typedef struct tp_buffer{ // Estrutura utilizada para armazenar o buffer.
   unsigned char db; 		//Dirty bit
   unsigned char pc; 		//Pin counter
   unsigned int nrec; 		//N�mero de registros armazenados na p�gina.
   char data[SIZE]; 		// Dados
   unsigned int position; 	// Pr�xima posi��o v�lida na p�gina.
}tp_buffer;

// Union's utilizados na convers�o de vari�veis do tipo inteiro e double.

union c_double{

	double dnum;
	char double_cnum[sizeof(double)];
};

union c_int{

	int  num;
	char cnum[sizeof(int)];
};

/*
	Esta fun��o tem por objetivo criar e inicializar uma estrutura do tipo tp_buffer
	que ser� usada para carregar tuplas na mem�ria

*/
tp_buffer * initbuffer();

/*
	Esta fun��o busca, no arquivo fs_object.dat, pelo nome da tabela retornando as informa��es que est�o no dicion�rio em uma estrutura fs_objects
	Caso o nome da tabela n�o exista, o programa aborta
	*nTabela - Nome da tabela a ser buscado no dicion�rio de dados
*/
struct fs_objects leObjeto(char *nTabela);

/*
	Esta fun��o busca, no arquivo fs_schema.dat, pelas informa��es do objeto, carregando o esquema da tabela que � retornadado em tp_table
	Caso o nome da tabela n�o exista, o programa aborta
	*objeto - Objeto, j� previamente inicializado em leObjeto(nTabela), que cont�m as informa��es sobre uma determinada tabela
*/
tp_table *leSchema (struct fs_objects objeto);

/*
	Esta fun��o calcula, usando o esquema e o dicion�rio de dados, o tamanho da tupla de uma tabela, retornando o mesmo.
	*esquema - Estrutura que cont�m o esquema da tabela (nome de campo, tipo de campo, etc)
	*objeto - Estrutura que cont�m informa��es sobre a tabela (nome da tabela, nome do arquivo da tabela, etc)

*/
int tamTupla(tp_table *esquema, struct fs_objects objeto);

/*
	Esta fun��o imprime todos os dados carregados numa determinada p�gina do buffer
	*buffer - Estrutura para armazenar tuplas na mem�ria
	*s - Estrutura que armazena esquema da tabela para ler os dados do buffer
	*objeto - Estrutura que armazena dados sobre a tabela que est� no buffer
	*num_page - N�mero da p�gina a ser impressa
*/
int printbufferpoll(tp_buffer *buffpoll, tp_table *s,struct fs_objects objeto, int num_page);
/*
	Esta fun��o insere uma tupla em uma p�gina do buffer em que haja espa�o suficiente. Retorna ERRO_BUFFER_CHEIO caso n�o haja espe�o para a tupla
	*buffer - Estrutura para armazenar tuplas na mem�ria
	*from - N�mero da tupla a ser posta no buffer. Este n�mero � relativo a ordem de inser��o da tupla na tabela em disco
	*campos - Estrutura que armazena esquema da tabela para ler os dados do buffer
	*objeto - Estrutura que armazena dados sobre a tabela que est� no buffer
*/
int colocaTuplaBuffer(tp_buffer *buffer, int from, tp_table *campos, struct fs_objects objeto);
/*
	Esta fun��o conta quantas tabelas j� est�o inseridas dentro do dicionario, para poder colocar um c�digo v�lido para a pr�xima tabela. Retorna a quantidade exata de tabelas.
*/
int quantidadeTabelas();
/*
	Esta fun��o verifica se um nome de tabela j� est� inserido no dicionario.
	Retorna:
		-> 1 se o nome existe no dicionario;
		-> 0 se existe no dicion�rio.
	*nomeTabela - Nome de uma tabela,  a qual deseja-se saber se existe no dicionario.
*/
int verificaNomeTabela(char *nomeTabela);
/*
	Esta fun��o inicia um estrutura do tipo table, como nome de tabela passado.
	Retorna:
		-> a estrutura do tipo table iniciada;
		-> ERRO_NOME_TABELA_INVALIDO se o nome passado j� existir no dicion�rio.
	*nomeTabela - Nome de uma tabela, a qual deseja-se inserir.
*/
table *iniciaTabela(char *nomeTabela);
/*
	Esta fun��o encadeia a lista de campos na estrutura de uma tabela que vai ser criada.
	Retorna:
		-> a estrutura com a coluna inserida na lista.
	*t - Estrutura da tabela � ser criada.
	*nomeCampo - Nome do campo que ir� ser inserido na lista de campos.
	tipoCampo - Tipo do campo que ir� ser inserido na lista de campos.
	tamanhoCampo - Tamanho do campo que ir� ser inserido na lista de campos.
*/
table *adicionaCampo(table *t,char *nomeCampo, char tipoCampo, int tamanhoCampo, int pk);
/*
	Esta fun��o finaliza a tabela preveamente estrutura pelas funcoes iniciaTabela() e adicionaCampo().
	Escreve nos arquivos fs_object.dat e fs_schema.dat, a estrutura passada.
	Retorna:
		-> SUCCESS quando teve sucesso na sua opera��o�;
		-> ERRO_ABRIR_ARQUIVO quando teve problemas ao abrir os arquivos fs_object.dat e fs_schema.dat;
		-> ERRO_PARAMETRO quando a estrutura passada � inv�lida.
	*t - Estrutura da tabela � ser criada.
*/
int finalizaTabela(table *t);
/*
	Esta fun��o inicia e aloca din�micamente uma lista de valores que v�o ser inseridos em uma tabela.
	Retorna:
		-> estrutura iniciada e alocad com o valor passado por par�metro.
		-> ERRO_DE_PARAMETRO, quando a estrutura enviada for inv�lida.
	*c - Estrutura de valores que v�o ser inseridos em uma tabela.
	*nomeCampo - Nome do campo que o usu�rio vai inserir um valor.
	*valorCampo - Valor do campo que vai ser inserido.
*/
column *insereValor(column *c, char *nomeCampo, char *valorCampo);
/*
	Esta fun��o finaliza a inser��o de valores em uma tabela. Assume que o usu�rio entrou com todos os campos de uma tupla completa.
	Retorna:
		-> ERRO_ABRIR_ARQUIVO, quando ocorreu um erro ao abrir o arquivo fs_object.dat ou fs_schema.dat;
		-> ERRO_NO_TAMANHO_STRING, quando ocorreu um erro no tamanho da string inserida;
		-> ERRO_NOME_CAMPO, quando o nome do campo passado na estrutura;
		-> ERRO_NO_TIPO_INTEIRO, quando o valor passado n�o � um inteiro;
		-> ERRO_NO_TIPO_DOUBLE, quando o valor passado n�o � um double;
		-> ERRO_NO_TIPO_CHAR, quando o valor passado � maior que 1byte;
		-> SUCCESS, quando a fun��o teve sucesso em sua opera��o de inser��o na tabela.
	*nome - Nome da tabela que vai ser inserido os valores da estrutura *c.
	*c - Estrutura com o valores que v�o ser inseridos na tabela *nome.
*/
int finalizaInsert(char *nome, column *c);
/*
	Esta fun��o recupera uma p�gina do buffer e retorna a mesma em uma estrutura do tipo column
	A estrutura column possui informa��es de como manipular os dados
	*buffer - Estrutura para armazenar tuplas na mem�ria
	*campos - Estrutura que armazena esquema da tabela para ler os dados do buffer
	*objeto - Estrutura que armazena dados sobre a tabela que est� no buffer
	*page - N�mero da p�gina a ser recuperada (0 a PAGES)
*/
column * getPage(tp_buffer *buffer, tp_table *campos, struct fs_objects objeto, int page);
/*
	Esta fun��o uma determinada tupla do buffer e retorna a mesma em uma estrutura do tipo column;
	A estrutura column possui informa��es de como manipular os dados
	*buffer - Estrutura para armazenar tuplas na mem�ria
	*campos - Estrutura que armazena esquema da tabela para ler os dados do buffer
	*objeto - Estrutura que armazena dados sobre a tabela que est� no buffer
	*page - N�mero da p�gina a ser recuperada uma tupla (0 a PAGES)
	*nTupla - N�mero da tupla a ser excluida, este n�mero � relativo a p�gina do buffer e n�o a todos os registros carregados
*/
column * excluirTuplaBuffer(tp_buffer *buffer, tp_table *campos, struct fs_objects objeto, int page, int nTupla);
