#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include "Opciones.h"
#include "Funciones.h"

// Función principal.
int main(){
    initscr();                                          //Inicializa la consola para uso de la libreria ncurses.
    clear();                                            //Limpia la consola.
    refresh();                                          //Refresca la consola para poder ser ecrita.
    short MenuOption;                                   //Variable que almacena la opción del menú elegida por el usuario.
    _Bool quit = FALSE;                                 //Variable que indica si el usuario pidio salir del programa.
    struct List* table = inicializarTabla();            //se crea la tabla hash

    //Menú principal.
    while(!quit){                                       //Mientras el usuario no pida salir, imprimir el menú.
        fflush(stdin);                        
        printw("\nPractica 1 - Sistemas Operativos 2019-II.\n\n");
        printw("1. Ingresar registro.\n");
        printw("2. Ver registro.\n");
        printw("3. Borrar registro.\n");
        printw("4. Buscar registro.\n");
        printw("5. Salir.\n\n");
        printw("Esperando opción: ");
        scanw("%hi",&MenuOption);
        printw("\n");

        //Evaluación de las opciones de menú.
        switch (MenuOption){
        case 1:
            if(IngresarRegistro(table)!=0){                 // Opción de Ingresar nuevo registro.
                endwin();
                exit(-1);
            }
            break;
        case 2:
            if(VerRegistro()!=0){                           // Opción de Ver Registro existente.
                endwin();
                exit(-1);
            }
            break;
        case 3:
            if(BorrarRegistro(table)!=0){                   // Opción de borrar registro del sistema.
                endwin();
                exit(-1);
            }
            break;
        case 4:
            if(BuscarRegistro(table)!=0){                   // Opción de buscar registro en tabla hash.
                endwin();
                exit(-1);
            }
            break;
        case 5:
            Salir(&quit);                                   // Opción Salir.
            break;
        case 31415:
            Generado(table);
            break;
        default: 
            printw("El valor \"%hi\" no es valido.\n\n",MenuOption);
            endwin();
            exit(-1);
            break;
        }
    }
    clear();
    endwin(); //Antes de finalizar, elimina la configuración hecha sobre la consola por parte de la libreria ncurses.
    return 0;
}