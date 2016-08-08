#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "Datos.h"
#include "funciones.h"

int crear_directorios(char *path);

void generar_disco(char *nombre,char *path, int tamano, int tipo){

    char directorio[strlen(path)];
    strcpy(directorio,path);
    if(crear_directorios(path)==0){
        printf("ERROR: el valor ingresado para path es erroneo.\n");
        printf("************GENERACION FALLIDA************\n\n");
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
        printf("************GENERACION FALLIDA************\n\n");
        return;
    }

    if(tipo==1)
        for(int j=0;j<tamano;j++)
            for(int i=0;i<1024;i++)
                fwrite(KiloByte,sizeof(char),1024,disco);
     else
        for(int i=0;i<tamano;i++)
                fwrite(KiloByte,sizeof(char),1024,disco);

    srand((unsigned int)time(NULL));

    MBR nuevo_mbr;
    nuevo_mbr.mbr_fecha_creacion = time(NULL);

    if(tipo==1)
        nuevo_mbr.mbr_tamano = 1024*1024*tamano;
    else
        nuevo_mbr.mbr_tamano = 1024*tamano;

    particion particion_vacia;

    particion_vacia.part_status = 'n';
    particion_vacia.part_type = 'n';
    particion_vacia.part_fit = 'n';
    particion_vacia.part_start = -1;
    particion_vacia.part_size = 0;

    nuevo_mbr.mbr_partition_1 = particion_vacia;
    nuevo_mbr.mbr_partition_2 = particion_vacia;
    nuevo_mbr.mbr_partition_3 = particion_vacia;
    nuevo_mbr.mbr_partition_4 = particion_vacia;

    nuevo_mbr.mbr_disk_signature = rand();

    fseek(disco,0,SEEK_SET);

    fwrite(&nuevo_mbr,sizeof(MBR),1,disco);

    fclose(disco);
    printf("************GENERACION COMPLETA************\n\n");
/*
    disco = fopen(nombred,"r+b");

    if(!disco){
        printf("ERROR: el Disco no ha podido crearse.\n");
        printf("************GENERACION FALLIDA************\n\n");
        return;
    }

    MBR devuelto;

    if(fread(&devuelto,sizeof(MBR),1,disco)==1)
        printf("%i\n",devuelto.mbr_tamano);
    else
        printf("ERROR:al cargar la data.\n");
*/
}

void eliminar_disco(char *path){
    if(access(path,F_OK)==-1){
        printf("ERROR: no exite el directorio o Disco con el nombre \"%s\"\n",path);
        printf("********ELIMINACION DE DISCO FALLIDA********\n\n");
        return;
    }

    if(unlink(path)==0){
        printf("********ELIMINACION DE DISCO EXITOSA********\n\n");
    }else{
        printf("********ELIMINACION DE DISCO FALLIDA********\n\n");
    }
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
