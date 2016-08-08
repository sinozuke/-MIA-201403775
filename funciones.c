#include <stdio.h>
#include "Datos.h"
#include "funciones.h"

void generar_disco(char *nombre,char *path, int tamano, int tipo){
/*    MBR mbr1;
    mbr1.mbr_fecha_creacion = time(NULL);
    printf("hecho!\n%");
    printf(ctime(&mbr1.mbr_fecha_creacion));
*/
    char KiloByte[1024];

    for(int i=0;i<1024;i++)
        KiloByte[i]='\0';

    FILE *disco;

    char *nombred = nombre;

    nombred = strcat(nombred,".dsk");

    disco = fopen(nombred,"wb");

    if(tipo==1)
        for(int j=0;j<tamano;j++)
            for(int i=0;i<1024;i++)
                fwrite(KiloByte,sizeof(char),1024,disco);
    else
        for(int i=0;i<tamano;i++)
                fwrite(KiloByte,sizeof(char),1024,disco);

    fclose(disco);
    printf("hecho\n");
}


