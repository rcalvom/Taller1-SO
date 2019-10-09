#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 32
#define TAMANNOLISTA 1024

struct dogType{
    char name[32];
    char type[32];
    int age;
    char breed[16];
    int height;
    float weight;
    char gender;     
};

struct List{
    struct Node* node;
};

struct Node{
    long id;
    char name[32];
    struct Node* next;
};


_Bool equals(char *cadena1, char *cadena2){
    for(int i = 0; i<SIZE; i++){
        if(*(cadena1 + i) != *(cadena2 + i))
            return 0;
        if((int)(*(cadena1+i)) == 0)
            break;
    }
    return 1;
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
            registros++;
        }
        toPrint = toPrint -> next;
    }
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
    printf("Index: %i\n", index);
    nodeToDelete = (table+index)->node;
    nodoAnt = NULL;
    do{
        if(nodeToDelete == NULL){
            printf("No se ha encontrado ningun nodo\n");
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
        return element->id;
    } else
        return insert(position->next, element);
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
    } else 
        return insert((table+index)->node, nodo);

    free(respaldoNombre);
    return idd;
}

//inicializa la tabla, retorna un apuntador al inicio de la tabla
struct List* inicializarTabla(){
    struct List* tabla = (struct List*)malloc(TAMANNOLISTA*8);
    bzero(tabla, TAMANNOLISTA*8);
    FILE *dataDogs, *end;
    for(int i = 0; i<TAMANNOLISTA; i++)
        (tabla+i)->node = NULL;

    dataDogs = fopen("dataDogs.dat", "r");
    if(dataDogs != NULL){
        do{
            end = fopen("dataDogs.dat", "a");
        } while(end == NULL);
        int read, index;
        do{
            struct Node *nodo;
            do{
                nodo = (struct Node *) malloc(sizeof(struct Node));
            }while(nodo == NULL);
            bzero(nodo, sizeof(struct Node));
            struct dogType *d = (struct dogType*)malloc(sizeof(struct dogType));
            read = 0;
            do{
                read += fread(&nodo->id, sizeof(long), 1, dataDogs);
            }while(read != 1);
            do{
                read += fread(d, sizeof(d), 1, dataDogs);
            } while(read != 2);
            copiarCadena(d->name, nodo->name);
            free(d);
            printf("Id leido: %li\n", nodo->id);
            printf("Nombre leido: %s\n", nodo->name);
            nodo->next = NULL;
            index = (nodo->id)%TAMANNOLISTA;
            if ((tabla+index)->node == NULL){
                //printf("Primer nodo vacio\n");
                (tabla+index)->node = nodo;
            } else if(nodo->id == index){
                //printf("Debo ir en el primer nodo\n");
                nodo->next = (tabla+index)->node;
                (tabla+index)->node = nodo;
            } else {
                //printf("Debo ir en algun lugar de la lista encadenada\n");
                insertById((tabla+index)->node, nodo);
            }
        } while(dataDogs != end);
    }
    return tabla;
}

void copiarCadena(char *original, char *copia){
    for(int i = 0; i<SIZE; i++){
        if((int)(*(original+i)) == 0)
            break;
        *(copia+i) = *(original+i);
    }
}

void leer(char* name, FILE* file){
    int i = 0;
    do{
        fread((name+i),1,1,file);
        i++;
    }while(*(name+i-1)!='\n');
    *(name+i-1) =  '\0';
    file ++;
}

int main(){
    struct List* table = malloc(8*TAMANNOLISTA);
    bzero(table,8*TAMANNOLISTA);
    table = inicializarTabla();
    srand(time(NULL));
    char* Breeds[] = {"Labrador", "Pug", "Bulldog", "Chihuahua", "Golden", "Dalmata", "Pastor", "Labrador", "San Bernardo"};
    char Genders[] = {'M','H'};
    struct dogType* newRegister;                                            //Se crea el struct
    int r;
    long idd;
    FILE *file, *names;
    char* tipo, *values, *mensajeHistoria, *filepath, *filename, * name;
    tipo = (char *) malloc(32);
    file = fopen("dataDogs.dat","a");
    names = fopen("nombresMascotas.txt","r");
    for(int i = 0; i<100000;i++){
        newRegister = (struct dogType*)malloc(sizeof(struct dogType));          //Se libera su espacio.
        name = malloc(32);
        bzero(newRegister, sizeof(struct dogType)); 
        bzero(tipo, 32);
        if(newRegister == NULL){                                                // Se comprueba que este se haya liberado correctamente.
            return -1;
        } 

        //
        int rnames = rand() % 1000;
        rewind(names);
        for(int i = 0; i<rnames; i++){
            leer(name,names);
        }

        //
        strcpy(newRegister->name,name);
        strcpy(tipo,"Perro");
        newRegister->age = rand()%14 +1;
        strcpy(newRegister->breed,Breeds[rand() % 8]);
        newRegister->height = rand() % 14 +1;
        newRegister->weight = rand() % 14 +1;
        newRegister->gender = Genders[rand() % 2];
        idd = insertElement(table, newRegister->name);                          // Se agrega el elemento a la tabla hash.
        copiarCadena(tipo, newRegister->type);
        free(tipo);                                                             // Se abre el archivo "dataDogs.dat" y se coloca el puntero al final.
        if(file == NULL){                                                       // Se comprueba el posible error
            return -1;
        }
        r = 0;
        r += fwrite(&idd, sizeof(long), 1, file);
        r += fwrite(newRegister,sizeof(struct dogType),1,file);                 // Se escriben los datos anteriormente solicitados en el archivo.
    }
    fclose(file);
    return 0;
}