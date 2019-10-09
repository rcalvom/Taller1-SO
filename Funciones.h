#ifndef FUNCIONESH
#define FUNCIONESH

struct List{
    struct Node* node;
};

struct Node{
    long id;
    char name[32];
    struct Node* next;
};

_Bool equals(char *cadena1, char *cadena2);
void toUpperCase(char *word); //Pasar cada caracter de una cadena a mayusculas.
void toLowerCase(char *word); //Pasar cada caracter de una cadena a minússculas.
void copiarCadena(char *original, char *copia);
int hash(char *name); //funcion de hash
void buscarId(struct List *table, char *nombre);
long borrar(struct List *table, long idd);
long insert(struct Node *position, struct Node *element); //insertar un elemento en la lista encadenada
long insertElement(struct List *table, char *nombre); //obtener index y llamar a la funcion insertar
struct List* inicializarTabla();

#endif