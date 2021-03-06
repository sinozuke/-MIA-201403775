#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "comandos.h"
#include "Datos.h"
#define MAXLON 300

char* token;
char cadena[MAXLON+1];

int main()
{
   while(1){
        gets(cadena);
        token = strtok(cadena, " ");
        while (token) {
                switch(opcion(token)){
                    case 1:
                        mkdisk(token);
                        break;
                    case 2:
                        rmdisk(token);
                        break;
                    case 3:
                        fdisk(token);
                        break;
                    case 4:
                        mount(token);
                        break;
                    case 5:
                        printf("Comando UMount invocado\n");
                        break;
                    case 6:
                        rep(token);
                        break;
                    case 7:
                        exec(token);
                        break;
                    case 8:
                        printf("----------Saliendo del sistema----------\n");
                        return 0;
                    default:
                        printf("ERROR: el comando \"%s\" no reconocido\n",token);
                        break;
                }
                while (token)
                    token = strtok(NULL, " ");
        }
    }
/*
    printf("%i\n",(int)strlen(cadena));
    printf("%i\n",(int)sizeof(MBR));
    printf("%i\n",(int)sizeof(particion));
    printf("%i\n",(int)sizeof(EBR));
*/
}
