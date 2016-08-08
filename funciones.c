#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Datos.h"
#include "funciones.h"

int crear_directorios(char *path);

void generar_disco(char *nombre,char *path, int tamano, char tipo){
/*    MBR mbr1;
    mbr1.mbr_fecha_creacion = time(NULL);
    printf("hecho!\n%");
    printf(ctime(&mbr1.mbr_fecha_creacion));
*/
    char directorio[strlen(path)];
    strcpy(directorio,path);
    if(crear_directorios(path)==0){
        printf("ERROR: el valor ingresado para path es erroneo.\n");
        return;
    }

    char KiloByte[1024];

    for(int i=0;i<1024;i++)
        KiloByte[i]='\0';

    FILE *disco;

    char *nombred = directorio;
    strcat(nombred,nombre);
    strcat(nombred,".dsk");
    disco = fopen(nombred,"wb");

    if(!disco){
        printf("ERROR: el Disco no ha podido crearse.\n");
        return;
    }

    if(tipo=='m')
        for(int j=0;j<tamano;j++)
            for(int i=0;i<1024;i++)
                fwrite(KiloByte,sizeof(char),1024,disco);
    else
        for(int i=0;i<tamano;i++)
                fwrite(KiloByte,sizeof(char),1024,disco);

    fclose(disco);
    printf("-----------------GENERANDO-----------------\n");
    printf("************GENERACION COMPLETA************\n");
}

int crear_directorios(char *path){

    char *direc = strtok(path,"/");

    if(!direc)
        return 0;

    char dirreccion[200]="/";

    strcat(dirreccion,direc);
    strcat(dirreccion,"/");

    struct stat st = {0};

    while(direc){
        if(stat(dirreccion,&st)==-1)
            mkdir(dirreccion,0700);
        direc = strtok(NULL,"/");
        if(direc){
            strcat(dirreccion,direc);
            strcat(dirreccion,"/");
        }
    }
    return 1;
}
