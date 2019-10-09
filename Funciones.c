#include "Funciones.h"
#include "Opciones.h"
#include <stdlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <strings.h>

#define SIZE 32
#define TAMANNOLISTA 1024

_Bool equals(char *cadena1, char *cadena2){
    for(int i = 0; i<SIZE; i++){
        if(*(cadena1 + i) != *(cadena2 + i))
            return 0;
        if((int)(*(cadena1+i)) == 0)
            break;
    }
    return 1;
}

//copia el cada caracter de la cadena original en la cadena copia
void copiarCadena(char *original, char *copia){
    for(int i = 0; i<SIZE; i++){
        if((int)(*(original+i)) == 0)
            break;
        *(copia+i) = *(original+i);
    }
}

//Pasar cada caracter de una cadena a mayúsculas.
void toUpperCase(char *word){
    for(int i = 0; i<SIZE; i++){
        if(*(word+i) >= 'a' && *(word+i)<='z')
            *(word+i) = (char)(*(word+i) - 32);
    }
}

//Pasar cada caracter de una cadena a minusculas.
void toLowerCase(char *word){
    for(int i = 0; i<SIZE; i++){
        if(*(word+i) >= 'A' && *(word+i)<='Z')
            *(word+i) = (char)(*(word+i) + 32);
    }
}

//funcion de hash
int hash(char *name){
    int index = 0;
    for(int i = 0; i<SIZE; i++){
        if((int)(*(name+i)) == 0)
            break;
        index += (*(name+i)-'A'+1)*(i+1);
    }
    index--;
    if(index < 0)
        index *= -1;
    return index%TAMANNOLISTA;
}

//busca e imprime todos los registros que coincidan con un nombre dado
void buscarId(struct List *table, char *nombre){
    int index;
    char *n;
    do{
        n = (char *) malloc(SIZE);
    } while(n == NULL);
    bzero(n, SIZE);
    copiarCadena(nombre, n);
    toUpperCase(n);
    index = hash(n);
    free(n);
    struct Node *toPrint = (table+index)->node;
    int registros = 0;
    while(toPrint != NULL){
        if(equals(toPrint->name, nombre)){
            printw("Nombre: %s, ", toPrint->name);
            printw("Id: %li.\n", toPrint->id);
            registros++;
        }
        toPrint = toPrint -> next;
    }
    printw("\nRegistros encontrados: %i\n\n", registros);
}

//borra el elemento en la tabla hash que coincida con el id dado en idd y reencadena los demas 
//elementos que esten en la misma posicion de la tabla
long borrar(struct List *table, long idd){
    int index;
    long id;
    struct Node *nodoAnt;
    struct Node *nodeToDelete;
    _Bool find = 0;
    index = idd%TAMANNOLISTA;
    printw("Index: %i\n", index);
    nodeToDelete = (table+index)->node;
    nodoAnt = NULL;
    do{
        if(nodeToDelete == NULL){
            printw("No se ha encontrado ningun nodo\n");
            return -1;
        }
        if(nodeToDelete->id == idd)
            find = 1;
        else {
            nodoAnt = nodeToDelete;
            nodeToDelete = nodeToDelete->next;
        }
    }while(!find);
    if(nodoAnt != NULL)
        nodoAnt->next = nodeToDelete->next;
    else
        (table+index)->node = nodeToDelete->next;
    id = nodeToDelete->id;
    free(nodeToDelete);
    return id;
}

/*
Definicion recursiva para encadenar elementos en la lista de una posicion en la tabla
*/
long insert(struct Node *position, struct Node *element){
    if(position->next == NULL){
        element->id = (position->id)+TAMANNOLISTA;
        position->next = element;
    } else if(position->id + TAMANNOLISTA != position->next->id){
        element->id = position->id + TAMANNOLISTA;
        element->next = position->next;
        position->next = element;
    } else
        return insert(position->next, element);

    return element->id;
}

int insertById(struct Node *position, struct Node *nodo){
    if(position->next == NULL){
        position->next = nodo;
    } else if((position->id + TAMANNOLISTA) == nodo->id){
        nodo->next = position->next;
        position->next = nodo;
    } else {
        insertById(position->next, nodo);
    }
}

/*
Inserta un elemento en la tabla hash
Parametros:
table es la tabla hash donde se va a insertar en elemento
nombre es el nombre del nuevo registro

Devuelve la id con que fue registrado
*/
long insertElement(struct List *table, char *nombre){
    int index;
    long idd;
    struct Node *nodo = (struct Node*) malloc(sizeof(struct Node));
    char *respaldoNombre = (char *)malloc(SIZE);
    if(nodo == NULL || respaldoNombre == NULL){
        return -1;
    }
    bzero(nodo, sizeof(struct Node));
    bzero(respaldoNombre, SIZE);
    copiarCadena(nombre, nodo->name);
    nodo->next = NULL;
    copiarCadena(nombre, respaldoNombre);
    toUpperCase(respaldoNombre);
    index = hash(respaldoNombre);
    idd = index;
    if(index < 0)
        return -1;
    else if(TAMANNOLISTA < index)
        return -2;
    
    if ((table+index)->node == NULL){
        nodo->id = idd;
        (table+index)->node = nodo;
    } else if((table+index)->node->id != index){
        nodo->id = idd;
        nodo->next = (table+index)->node;
        (table+index)->node = nodo;
    } else 
        return insert((table+index)->node, nodo);

    free(respaldoNombre);
    return idd;
}

//inicializa la tabla, retorna un apuntador al inicio de la tabla
struct List* inicializarTabla(){
    struct List* tabla = (struct List*)malloc(TAMANNOLISTA*8);
    bzero(tabla, TAMANNOLISTA*8);
    FILE *dataDogs;
    for(int i = 0; i<TAMANNOLISTA; i++)
        (tabla+i)->node = NULL;

    dataDogs = fopen("dataDogs.dat", "r");
    if(dataDogs != NULL){
        int read, index, attempts = 0;
        while(attempts < 5){
            attempts = 0;
            struct Node *nodo;
            do{
                nodo = (struct Node *) malloc(sizeof(struct Node));
            }while(nodo == NULL);
            bzero(nodo, sizeof(struct Node));
            struct dogType *d = (struct dogType*)malloc(sizeof(struct dogType));
            read = 0;
            attempts == 0;
            do{
                read += fread(&nodo->id, sizeof(long), 1, dataDogs);
                attempts++;
            }while(read != 1 && attempts<5);
            do{
                read += fread(d, sizeof(struct dogType), 1, dataDogs);
            } while(read != 2 && attempts<5);
            if(attempts < 5){
                copiarCadena(d->name, nodo->name);
                nodo->next = NULL;
                index = (nodo->id)%TAMANNOLISTA;
                if ((tabla+index)->node == NULL){
                    (tabla+index)->node = nodo;
                } else if(nodo->id == index){
                    nodo->next = (tabla+index)->node;
                    (tabla+index)->node = nodo;
                } else {
                    insertById((tabla+index)->node, nodo);
                }
            }
            free(d);
        }
        fclose(dataDogs);
    }
    return tabla;
}