#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comandos.h"
#define MAXLON 200

char* token;
char cadena[MAXLON+1];

int main()
{
    while(1){
        gets(cadena);
        for(int i = 0; cadena[i]; i++)
            cadena[i] = tolower(cadena[i]); //<--convierte a minusculas
        token = strtok(cadena, " ");
        while (token) {
                switch(opcion(token)){
                    case 1:
                        mkdisk(token);
                        break;
                    case 2:
                        printf("Comando Rmdisk invocado\n");
                        break;
                    case 3:
                        printf("Comando fdisk invocado\n");
                        break;
                    case 4:
                        printf("Comando Mount invocado\n");
                        break;
                    case 5:
                        printf("Comando UMount invocado\n");
                        break;
                    case 6:
                        printf("Comando Rep invocado\n");
                        break;
                    case 7:
                        printf("Comando Exec invocado\n");
                        break;
                    default:
                        printf("ERROR: el comando \"%s\" no reconocido\n",token);
                        break;
                }
                while (token)
                    token = strtok(NULL, " ");
        }
    }
}
