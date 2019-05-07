#define path "/temp/"
#define serverPipe "/temp/serverPipe"
#define PATHARTIGOS "artigos"
#define PATHTSTRINGS "strings"

typedef struct _artigo{
    int ID;
    off_t stringRef;
    int price;
} Artigo;

