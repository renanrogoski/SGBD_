#include "buffend.c"
int main ()
{
	tp_buffer *bp = (tp_buffer*)malloc(sizeof(tp_buffer)*PAGES);
	bp = initbuffer();

	char *nomeTabela=(char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);
	char *nomeCampo=(char *)malloc(sizeof(char)*TAMANHO_NOME_CAMPO);
	int inserir;

	char tipoCampo;
	int pk;

	table *tabela = (table *)malloc(sizeof(table)*1);
	int fimCampos = 0, numTabelas, tamanhoCampo, i,menu;

    struct fs_objects objeto;

    while(menu!=3)
	{
		printf("|--------------------------------------------------------------------|\n");
		printf("|                              Menu                                  |\n");
		printf("|--------------------------------------------------------------------|\n");
		printf(" Para criar tabelas digite 1 \n Para vizualizar as tabelas criadas digite 2 \n Para sair digite 3\n");
		scanf("%d",&menu);
		printf("|--------------------------------------------------------------------|\n");
		if(menu==1)
		{ //cria as tabelas
			printf("|--------------------------------------------------------------------|\n");
			printf("|                        Criar tabelas                               |\n");
			printf("|--------------------------------------------------------------------|\n\n");

            printf("Digite o numero de tabelas a serem inseridas!\n");
            scanf("%i",&numTabelas);

            for(i = 0; i < numTabelas; i++){
                printf("Digite o nome da tabela!\n");
                scanf("%s",nomeTabela);

                printf("\n%s\n", nomeTabela);

                tabela = iniciaTabela(nomeTabela);

                while(fimCampos == 0){
                    printf("Digite o Nome do campo!\n");
                    scanf("%s",nomeCampo);
                    fflush(stdin);

                    printf("Digite o Tipo do campo!(S-C-I-D)\n");
                    scanf("%c",&tipoCampo);
                    fflush(stdin);

                    if(tipoCampo == 'I')
                        tamanhoCampo = sizeof(int);
                    if(tipoCampo == 'C')
                         tamanhoCampo = sizeof(char);
                    if(tipoCampo =='D')
                        tamanhoCampo = sizeof(double);
                    if(tipoCampo == 'S'){
                        printf("Digite o Tamanho do campo!\n");
                        scanf("%d",&tamanhoCampo);
                    }

                    printf("O Campo eh Chave Primaria?(Sim 1 - 0 Nao)\n");
                    scanf("%d",&pk);

                    tabela = adicionaCampo(tabela, nomeCampo, tipoCampo, tamanhoCampo,pk);

                    printf("Deseja inserir um novo campo? (Sim 1 - 0 Nao)\n");
                    scanf("%d",&inserir);

                    if(inserir == 0){
                        fimCampos = 1;
                        finalizaTabela(tabela);
                    }
                }
            }

        }
        if(menu==2)
		{ //mostra as tabelas criadas
			//char *nomeTabela;
			printf("|--------------------------------------------------------------------|\n");
			printf("|                     Visualizar tabelas                             |\n");
			printf("|--------------------------------------------------------------------|\n\n");
			printf("Digite o nome da tabela!\n");
			scanf("%s",nomeTabela);
			objeto=leObjeto(nomeTabela);
			leSchema(objeto);
			printf("a tabela e:\n %s",objeto.nome);

			printf("\n|--------------------------------------------------------------------|\n\n");

		}
	}







	return 0;
}
