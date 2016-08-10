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

    printf("-------------CREANDO DISCO-------------\n");
    printf("-----------------DATOS-----------------\n");

    if(crear_directorios(path)==0){
        printf("ERROR: el valor ingresado para path es erroneo.\n");
        printf("**********GENERACION FALLIDA***********\n\n");
        return;
    }

    if(tipo==1){
            if(tamano<10){
            printf("ERROR: le valor minimo para un nuevo Disco es de 10 Mb.\n");
            printf("**********GENERACION FALLIDA***********\n\n");
            return;
        }
    }else{
        if(tamano<10240){
            printf("ERROR: el valor minimo para un nuevo Disco Duro es de 10240Kb.\n");
            printf("**********GENERACION FALLIDA***********\n\n");
            return;
        }
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
        printf("**********GENERACION FALLIDA***********\n\n");
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

    particion_vacia.part_status = '-';
    particion_vacia.part_type = 'n';
    particion_vacia.part_fit = 'n';
    particion_vacia.part_start = -1;
    particion_vacia.part_size = 0;
    strcpy(particion_vacia.part_name,"-vacia-");

    nuevo_mbr.mbr_partition_1 = particion_vacia;
    nuevo_mbr.mbr_partition_2 = particion_vacia;
    nuevo_mbr.mbr_partition_3 = particion_vacia;
    nuevo_mbr.mbr_partition_4 = particion_vacia;

    nuevo_mbr.mbr_disk_signature = rand();

    fseek(disco,0,SEEK_SET);

    fwrite(&nuevo_mbr,sizeof(MBR),1,disco);

    fclose(disco);
    printf("************GENERACION COMPLETA************\n\n");
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


void crear_pp(char *nombre, char *path, int tamano, char fit,int unit){
    printf("-------------CREANDO PARTICION-------------\n");

    struct stat st = {0};
    printf("%s\n",path);
    if(stat(path,&st)==-1){
        printf("ERROR: el archivo especificado no existe.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        return;
    }

    FILE *disco;

    disco = fopen(path,"r+b");

    if(!disco){
        printf("ERROR: el Disco no ha podido Abrirse.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        return;
    }

    MBR leido;

    if(fread(&leido,sizeof(MBR),1,disco)!=1){
        printf("ERROR:al cargar la data.del disco\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        return;
    }

    int creado=0;

    if(leido.mbr_partition_1.part_status=='-'){
        leido.mbr_partition_1.part_status='n';
        leido.mbr_partition_1.part_type='p';
        leido.mbr_partition_1.part_fit=fit;
        leido.mbr_partition_1.part_start=sizeof(MBR);
        switch(unit){
            case 1:
                leido.mbr_partition_1.part_size=tamano*1024*1024;
                break;
            case 2:
                leido.mbr_partition_1.part_size=tamano*1024;
                break;
            case 3:
                leido.mbr_partition_1.part_size=tamano;
                break;
        }
        strcpy(leido.mbr_partition_1.part_name,nombre);
        creado = 1;
    }

    if(leido.mbr_partition_2.part_status=='-' && creado!=1){
        leido.mbr_partition_2.part_status='n';
        leido.mbr_partition_2.part_type='p';
        leido.mbr_partition_2.part_fit=fit;
        leido.mbr_partition_2.part_start=sizeof(MBR);
        switch(unit){
            case 1:
                leido.mbr_partition_2.part_size=tamano*1024*1024;
                break;
            case 2:
                leido.mbr_partition_2.part_size=tamano*1024;
                break;
            case 3:
                leido.mbr_partition_2.part_size=tamano;
                break;
        }
        strcpy(leido.mbr_partition_2.part_name,nombre);
        creado = 1;
    }

    if(leido.mbr_partition_3.part_status=='-' && creado!=1){
        leido.mbr_partition_3.part_status='n';
        leido.mbr_partition_3.part_type='p';
        leido.mbr_partition_3.part_fit=fit;
        leido.mbr_partition_3.part_start=sizeof(MBR);
        switch(unit){
            case 1:
                leido.mbr_partition_3.part_size=tamano*1024*1024;
                break;
            case 2:
                leido.mbr_partition_3.part_size=tamano*1024;
                break;
            case 3:
                leido.mbr_partition_3.part_size=tamano;
                break;
        }
        strcpy(leido.mbr_partition_3.part_name,nombre);
        creado = 1;
    }

    if(leido.mbr_partition_4.part_status=='-' && creado!=1){
        leido.mbr_partition_4.part_status='n';
        leido.mbr_partition_4.part_type='p';
        leido.mbr_partition_4.part_fit=fit;
        leido.mbr_partition_4.part_start=sizeof(MBR);
        switch(unit){
            case 1:
                leido.mbr_partition_4.part_size=tamano*1024*1024;
                break;
            case 2:
                leido.mbr_partition_4.part_size=tamano*1024;
                break;
            case 3:
                leido.mbr_partition_4.part_size=tamano;
                break;
        }
        strcpy(leido.mbr_partition_4.part_name,nombre);
        creado = 1;
    }

    if(creado){
        fseek(disco,0,SEEK_SET);
        fwrite(&leido,sizeof(MBR),1,disco);
        printf("-------------CREACION EXITOSA--------------\n");
    }else{
        printf("ERROR: la cantidad maxima de particiones se ha exedido.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
    }
    fclose(disco);

    disco = fopen(path,"r+b");

    if(!disco){
        printf("ERROR: el Disco no ha podido Abrirse.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        return;
    }

    MBR master;

    if(fread(&master,sizeof(MBR),1,disco)!=1){
        printf("ERROR:al cargar la data.del disco\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        return;
    }

    printf("%s\n",master.mbr_partition_1.part_name);
    printf("%s\n",master.mbr_partition_2.part_name);
    printf("%s\n",master.mbr_partition_3.part_name);
    printf("%s\n",master.mbr_partition_4.part_name);


}

void crear_pl(char *nombre, char *path, int tamano, char fit,int unit){
    printf("-------------CREANDO PARTICION-------------\n");
    printf("Nombre: %s\nPath: %s\nTamano: %i\nTipo: Logica\nFit: %c\nUnit: %i\n",nombre,path,tamano,fit,unit);
    printf("-------------CREACION EXITOSA--------------\n");
}

void crear_pe(char *nombre, char *path, int tamano, char fit,int unit){
    printf("-------------CREANDO PARTICION-------------\n");
    printf("Nombre: %s\nPath: %s\nTamano: %i\nTipo: Extendida\nFit: %c\nUnit: %i\n",nombre,path,tamano,fit,unit);
    printf("-------------CREACION EXITOSA--------------\n");
}

void eliminar_particion(char *name,char *path,char tipo){
    printf("-------------ELIMINAR PARTICION------------\n");
    printf("Nombre: %s\nPath: %s\nTipo: %c\n",name,path,tipo);
    printf("-------------ELIMINACION EXITOSA-----------\n\n");
}

void modificar_tamano_particion(char *name, char*path,char unit,int add){
    printf("-------------MODIFICAR PARTICION-----------\n");
    printf("Nombre: %s\nPath: %s\nTamano a Modificar: %i\nUnidades: %c\n",name,path,add,unit);
    printf("-------------ELIMINACION EXITOSA-----------\n\n");
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
