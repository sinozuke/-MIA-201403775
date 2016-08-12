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
int lugar(MBR mbr,int tamano);
int name_exist(FILE *disco,MBR mbr,char *name);

void generar_disco(char *nombre,char *path, int tamano, int tipo){

    char directorio[strlen(path)];
    strcpy(directorio,path);

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

    particion_vacia.part_status = 'n';
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
        fclose(disco);
        return;
    }

    int tamano_real;

    switch(unit){
        case 1:
            tamano_real=tamano*1024*1024;
            break;
        case 2:
            tamano_real=tamano*1024;
            break;
        case 3:
            tamano_real=tamano;
            break;
    }

    printf("-------------------DATOS-------------------\n");
    printf("Nombre: %s\nPath: %s\nTamano: %i\nFit: %c\nUnit: %i\nTipo: Primaria\n",nombre,path,tamano_real,fit,unit);


    int posicion = lugar(leido,tamano_real);

    if(posicion==0){
        printf("ERROR: ya no es posible crear particion devido a que ya se a sobrepadaso el numero de particiones posibles.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

    if(posicion==-1){
        printf("ERROR: ya no es posible crear particion devido a que ya no espacio disponible.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

    if(name_exist(disco,leido,nombre)){
        printf("ERROR: el nombre ya existen entre las particiones.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

    particion partnueva;

    partnueva.part_status='s';
    partnueva.part_type='p';
    partnueva.part_fit=fit;
    switch(unit){
        case 1:
            partnueva.part_size=tamano*1024*1024;
            break;
        case 2:
            partnueva.part_size=tamano*1024;
            break;
        case 3:
            partnueva.part_size=tamano;
            break;
    }
    strcpy(partnueva.part_name,nombre);

    switch(posicion){
        case 1:
            partnueva.part_start=sizeof(MBR)+1;
            leido.mbr_partition_1=partnueva;
            break;
        case 2:
            partnueva.part_start=leido.mbr_partition_1.part_start+leido.mbr_partition_1.part_size+1;
            leido.mbr_partition_2=partnueva;
            break;
        case 3:
            partnueva.part_start=leido.mbr_partition_2.part_start+leido.mbr_partition_2.part_size+1;
            leido.mbr_partition_3=partnueva;
            break;
        case 4:
            partnueva.part_start=leido.mbr_partition_3.part_start+leido.mbr_partition_3.part_size+1;
            leido.mbr_partition_4=partnueva;
            break;
    }

    fseek(disco,0,SEEK_SET);
    fwrite(&leido,sizeof(MBR),1,disco);
    printf("-------------CREACION EXITOSA--------------\n");

    fclose(disco);
/*
    disco = fopen(path,"r+b");

    if(!disco){
        printf("ERROR: el Disco no ha podido Abrirse.\n");
        return;
    }

    MBR master;

    if(fread(&master,sizeof(MBR),1,disco)!=1){
        printf("ERROR:al cargar la data.del disco\n");
        fclose(disco);
        return;
    }

    printf("Particion 1:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_1.part_name,master.mbr_partition_1.part_start,master.mbr_partition_1.part_size);
    printf("Particion 2:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_2.part_name,master.mbr_partition_2.part_start,master.mbr_partition_2.part_size);
    printf("Particion 3:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_3.part_name,master.mbr_partition_3.part_start,master.mbr_partition_3.part_size);
    printf("Particion 4:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_4.part_name,master.mbr_partition_4.part_start,master.mbr_partition_4.part_size);

    fclose(disco);*/
}

void crear_pl(char *nombre, char *path, int tamano, char fit,int unit){
    printf("-------------CREANDO PARTICION-------------\n");
    printf("Nombre: %s\nPath: %s\nTamano: %i\nTipo: Logica\nFit: %c\nUnit: %i\n",nombre,path,tamano,fit,unit);
    printf("-------------CREACION EXITOSA--------------\n");
}

void crear_pe(char *nombre, char *path, int tamano, char fit,int unit){
    printf("-------------CREANDO PARTICION-------------\n");

    struct stat st = {0};
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
        fclose(disco);
        return;
    }

    int tamano_real;

    switch(unit){
        case 1:
            tamano_real=tamano*1024*1024;
            break;
        case 2:
            tamano_real=tamano*1024;
            break;
        case 3:
            tamano_real=tamano;
            break;
    }

    printf("-------------------DATOS-------------------\n");
    printf("Nombre: %s\nPath: %s\nTamano: %i\nFit: %c\nUnit: %i\nTipo: Extendida\n",nombre,path,tamano_real,fit,unit);


    int posicion = lugar(leido,tamano_real);

    if(posicion==0){
        printf("ERROR: ya no es posible crear particion devido a que ya se a sobrepadaso el numero de particiones posibles.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

    if(posicion==-1){
        printf("ERROR: ya no es posible crear particion devido a que ya no espacio disponible.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

    if(name_exist(disco,leido,nombre)){
        printf("ERROR: el nombre ya existen entre las particiones.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

    particion partnueva;

    partnueva.part_status='s';
    partnueva.part_type='e';
    partnueva.part_fit=fit;
    partnueva.part_start=sizeof(MBR);
    switch(unit){
        case 1:
            partnueva.part_size=tamano*1024*1024;
            break;
        case 2:
            partnueva.part_size=tamano*1024;
            break;
        case 3:
            partnueva.part_size=tamano;
            break;
    }
    strcpy(partnueva.part_name,nombre);

    switch(posicion){
        case 1:
            leido.mbr_partition_1=partnueva;
            break;
        case 2:
            leido.mbr_partition_2=partnueva;
            break;
        case 3:
            leido.mbr_partition_3=partnueva;
            break;
        case 4:
            leido.mbr_partition_4=partnueva;
            break;
    }

    fseek(disco,0,SEEK_SET);
    fwrite(&leido,sizeof(MBR),1,disco);

    EBR nuevoebr;

    nuevoebr.part_fit='n';
    strcpy(nuevoebr.part_name,"-vacio-");
    nuevoebr.part_next=-1;
    nuevoebr.part_size=0;
    nuevoebr.part_start=-1;
    nuevoebr.part_status='n';

    switch(posicion){
        case 1:
            fseek(disco,0,leido.mbr_partition_1.part_start);
            fwrite(&nuevoebr,sizeof(EBR),1,disco);
            break;
        case 2:
            fseek(disco,0,leido.mbr_partition_1.part_start);
            fwrite(&nuevoebr,sizeof(EBR),1,disco);
            break;
        case 3:
            fseek(disco,0,leido.mbr_partition_1.part_start);
            fwrite(&nuevoebr,sizeof(EBR),1,disco);
            break;
        case 4:
            fseek(disco,0,leido.mbr_partition_1.part_start);
            fwrite(&nuevoebr,sizeof(EBR),1,disco);
            break;
    }

    printf("-------------CREACION EXITOSA--------------\n");

    fclose(disco);
/*
    disco = fopen(path,"r+b");

    if(!disco){
        printf("ERROR: el Disco no ha podido Abrirse.\n");
        return;
    }

    MBR master;

    if(fread(&master,sizeof(MBR),1,disco)!=1){
        printf("ERROR:al cargar la data.del disco\n");
        fclose(disco);
        return;
    }

    printf("Particion 1:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_1.part_name,master.mbr_partition_1.part_start,master.mbr_partition_1.part_size);
    printf("Particion 2:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_2.part_name,master.mbr_partition_2.part_start,master.mbr_partition_2.part_size);
    printf("Particion 3:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_3.part_name,master.mbr_partition_3.part_start,master.mbr_partition_3.part_size);
    printf("Particion 4:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_4.part_name,master.mbr_partition_4.part_start,master.mbr_partition_4.part_size);

    fclose(disco);*/
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

int name_exist(FILE *disco,MBR mbr,char *name){

    if(mbr.mbr_partition_1.part_status!='n')
        if(strcmp(mbr.mbr_partition_1.part_name,name)==0)
            return 1;

    if(mbr.mbr_partition_2.part_status!='n')
        if(strcmp(mbr.mbr_partition_2.part_name,name)==0)
            return 1;

    if(mbr.mbr_partition_3.part_status!='n')
        if(strcmp(mbr.mbr_partition_3.part_name,name)==0)
            return 1;

    if(mbr.mbr_partition_4.part_status!='n')
        if(strcmp(mbr.mbr_partition_4.part_name,name)==0)
            return 1;

    int extendida =0;

    if(mbr.mbr_partition_1.part_status!='n')
        if(mbr.mbr_partition_1.part_type=='e')
            extendida = 1;

    if(mbr.mbr_partition_1.part_status!='n')
        if(mbr.mbr_partition_1.part_type=='e')
            extendida = 2;

    if(mbr.mbr_partition_1.part_status!='n')
        if(mbr.mbr_partition_1.part_type=='e')
            extendida = 3;

    if(mbr.mbr_partition_1.part_status!='n')
        if(mbr.mbr_partition_1.part_type=='e')
            extendida = 4;

    if(extendida!=0){
        EBR leido;

        switch(extendida){
            case 1:
                fseek(disco,0,mbr.mbr_partition_1.part_start-1);
                fread(&leido,sizeof(EBR),1,disco);
                break;
            case 2:
                fseek(disco,0,mbr.mbr_partition_2.part_start-1);
                fread(&leido,sizeof(EBR),1,disco);
                break;
            case 3:
                fseek(disco,0,mbr.mbr_partition_3.part_start-1);
                fread(&leido,sizeof(EBR),1,disco);
                break;
            case 4:
                fseek(disco,0,mbr.mbr_partition_4.part_start-1);
                fread(&leido,sizeof(EBR),1,disco);
                break;
        }

        if(leido.part_status!='n'){
            while(leido.part_next!=-1){
                if(strcmp(leido.part_name,name)==0)
                    return 1;
                fseek(disco,0,leido.part_start+leido.part_size-2);
                fread(&leido,sizeof(EBR),1,disco);
            }
            if(strcmp(leido.part_name,name)==0)
                return 1;
        }
    }

    return 0;
}

int lugar(MBR mbr,int tamano){
    int bloques[4] = {1,1,1,1};

    for(int i=0;i<4;i++)
        switch(i){
            case 0:
                if(mbr.mbr_partition_1.part_status=='n')
                    bloques[i]=0;
                break;
            case 1:
                if(mbr.mbr_partition_2.part_status=='n')
                    bloques[i]=0;
                break;
            case 2:
                if(mbr.mbr_partition_3.part_status=='n')
                    bloques[i]=0;
                break;
            case 3:
                if(mbr.mbr_partition_4.part_status=='n')
                    bloques[i]=0;
                break;
        }

    if(bloques[0]==1 && bloques[1]==1 && bloques[2]==1 && bloques[3]==1){
        return 0;
    }

    if(bloques[0]==1 && bloques[1]==1 && bloques[2]==1 && bloques[3]==0){
        if((tamano+mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)-1<mbr.mbr_tamano)
            return 2;
    }

    if(bloques[0]==1 && bloques[1]==1 && bloques[2]==0 && bloques[3]==1){
        if((tamano+mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)-1<mbr.mbr_partition_4.part_start)
            return 3;
    }

    if(bloques[0]==1 && bloques[1]==0 && bloques[2]==1 && bloques[3]==1){
        if((tamano+mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)-1<mbr.mbr_partition_3.part_start)
            return 2;
    }

    if(bloques[0]==0 && bloques[1]==1 && bloques[2]==1 && bloques[3]==1){
        if((tamano+sizeof(MBR))<mbr.mbr_partition_2.part_start)
            return 1;
    }

    if(bloques[0]==0 && bloques[1]==1 && bloques[2]==1 && bloques[3]==0){
        if((tamano+sizeof(MBR))<mbr.mbr_partition_2.part_start)
            return 1;
        else if(tamano+(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)-1<mbr.mbr_tamano)
            return 4;
    }

    if(bloques[0]==0 && bloques[1]==1 && bloques[2]==0 && bloques[3]==1){
        if((tamano+sizeof(MBR))<mbr.mbr_partition_2.part_start)
            return 1;
        else if(tamano+(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)-1<mbr.mbr_partition_4.part_start)
            return 3;
    }

    if(bloques[0]==0 && bloques[1]==0 && bloques[2]==1 && bloques[3]==1){
        if(tamano+sizeof(MBR)<mbr.mbr_partition_3.part_start)
            return 1;
    }

    if(bloques[0]==1 && bloques[1]==0 && bloques[2]==1 && bloques[3]==0){
        if((tamano+mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)<mbr.mbr_partition_3.part_start)
            return 2;
        else if(tamano+(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)-1<mbr.mbr_tamano)
            return 4;
    }

    if(bloques[0]==1 && bloques[1]==1 && bloques[2]==0 && bloques[3]==0){
        if(tamano+(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)-1<mbr.mbr_tamano)
            return 3;
    }

    if(bloques[0]==1 && bloques[1]==0 && bloques[2]==0 && bloques[3]==1){
        if(tamano+(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)-1<mbr.mbr_partition_4.part_start)
            return 2;
    }

    if(bloques[0]==1 && bloques[1]==0 && bloques[2]==0 && bloques[3]==0){
        if(tamano+(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)-1<mbr.mbr_tamano)
            return 2;
    }

    if(bloques[0]==0 && bloques[1]==1 && bloques[2]==0 && bloques[3]==0){
        if((tamano+sizeof(MBR))<mbr.mbr_partition_2.part_start)
            return 1;
        else if(tamano+(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)-1<mbr.mbr_tamano)
            return 3;
    }

    if(bloques[0]==0 && bloques[1]==0 && bloques[2]==1 && bloques[3]==0){
        if((tamano+sizeof(MBR))<mbr.mbr_partition_3.part_start)
            return 1;
        else if(tamano+(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)-1<mbr.mbr_tamano)
            return 4;
    }

    if(bloques[0]==0 && bloques[1]==0 && bloques[2]==0 && bloques[3]==1){
        if((tamano+sizeof(MBR))<mbr.mbr_partition_4.part_start)
            return 1;
    }

    if(bloques[0]==0 && bloques[1]==0 && bloques[2]==0 && bloques[3]==0){
        if((tamano+sizeof(MBR))<mbr.mbr_tamano)
            return 1;
    }

    return -1;
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
