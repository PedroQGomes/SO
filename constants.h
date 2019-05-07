#define path "/temp/"
#define serverPipe "/temp/serverPipe"
#define PATHARTIGOS "artigos"
#define PATHTSTRINGS "strings"
#define PATHSTOCKS "stocks"
#define PATHVENDAS "vendas"
#define TAM_ARTIGO_STR_WITH_NEWLINE 23
#define TAM_ARTIGO_STR 22
#define CACHE_SIZE 10;
typedef struct action* Action;
typedef struct answer* Answer;



typedef struct _artigo{
    int ID;
    off_t stringRef;
    int price;
} Artigo;


typedef struct sale {
    int ID;
    int qnt;
    int price;
} Sale;


struct action{
		pid_t pid; // um pipe com o nome da pid do cliente que o esta a pedir
		int codigo; // código do artigo
		int quantidade; //montate do artigo para as vendas, 0 caso seja instrução de consulta
    };

struct answer
{
    int stock; // quantidade atualizada em stock
    int preco; // preco do produto
};

