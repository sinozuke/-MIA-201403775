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
int extendida_exist(MBR mbr);

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
        printf("ERROR:al cargar la data del disco\n");
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
            partnueva.part_start=sizeof(MBR);
            leido.mbr_partition_1=partnueva;
            break;
        case 2:
            partnueva.part_start=leido.mbr_partition_1.part_start+leido.mbr_partition_1.part_size;
            leido.mbr_partition_2=partnueva;
            break;
        case 3:
            partnueva.part_start=leido.mbr_partition_2.part_start+leido.mbr_partition_2.part_size;
            leido.mbr_partition_3=partnueva;
            break;
        case 4:
            partnueva.part_start=leido.mbr_partition_3.part_start+leido.mbr_partition_3.part_size;
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
        printf("ERROR:al cargar la data del disco\n");
        fclose(disco);
        return;
    }

    printf("Particion 1:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_1.part_name,master.mbr_partition_1.part_start,master.mbr_partition_1.part_size);
    printf("Particion 2:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_2.part_name,master.mbr_partition_2.part_start,master.mbr_partition_2.part_size);
    printf("Particion 3:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_3.part_name,master.mbr_partition_3.part_start,master.mbr_partition_3.part_size);
    printf("Particion 4:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_4.part_name,master.mbr_partition_4.part_start,master.mbr_partition_4.part_size);

    fclose(disco);*/
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
        printf("ERROR:al cargar la data del disco\n");
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

    if(extendida_exist(leido)!=0){
        printf("ERROR: ya existe una particion Extendida en el disco.\n");
        fclose(disco);
        return;
    }

    particion partnueva;

    partnueva.part_status='s';
    partnueva.part_type='e';
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
            partnueva.part_start=sizeof(MBR);
            leido.mbr_partition_1=partnueva;
            break;
        case 2:
            partnueva.part_start=leido.mbr_partition_1.part_start+leido.mbr_partition_1.part_size;
            leido.mbr_partition_2=partnueva;
            break;
        case 3:
            partnueva.part_start=leido.mbr_partition_2.part_start+leido.mbr_partition_2.part_size;
            leido.mbr_partition_3=partnueva;
            break;
        case 4:
            partnueva.part_start=leido.mbr_partition_3.part_start+leido.mbr_partition_3.part_size;
            leido.mbr_partition_4=partnueva;
            break;
    }

    fseek(disco,0,SEEK_SET);
    fwrite(&leido,sizeof(MBR),1,disco);

    EBR nuevoebr;

    nuevoebr.part_fit='n';
    strcpy(nuevoebr.part_name,"-vacio-");
    nuevoebr.part_next=-1;
    nuevoebr.part_status='n';
    nuevoebr.part_size=0;
    nuevoebr.part_start=-1;
    switch(posicion){
        case 1:
            nuevoebr.part_size=leido.mbr_partition_1.part_size-sizeof(EBR);
            fseek(disco,leido.mbr_partition_1.part_start,SEEK_SET);
            fwrite(&nuevoebr,sizeof(EBR),1,disco);
            break;
        case 2:
            nuevoebr.part_size=leido.mbr_partition_2.part_size-sizeof(EBR);
            fseek(disco,leido.mbr_partition_2.part_start,SEEK_SET);
            fwrite(&nuevoebr,sizeof(EBR),1,disco);
            break;
        case 3:
            nuevoebr.part_size=leido.mbr_partition_3.part_size-sizeof(EBR);
            fseek(disco,leido.mbr_partition_3.part_start,SEEK_SET);
            fwrite(&nuevoebr,sizeof(EBR),1,disco);
            break;
        case 4:
            nuevoebr.part_size=leido.mbr_partition_4.part_size-sizeof(EBR);
            fseek(disco,leido.mbr_partition_4.part_start,SEEK_SET);
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
        printf("ERROR:al cargar la data del disco\n");
        fclose(disco);
        return;
    }

    printf("Particion 1:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_1.part_name,master.mbr_partition_1.part_start,master.mbr_partition_1.part_size);
    printf("Particion 2:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_2.part_name,master.mbr_partition_2.part_start,master.mbr_partition_2.part_size);
    printf("Particion 3:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_3.part_name,master.mbr_partition_3.part_start,master.mbr_partition_3.part_size);
    printf("Particion 4:\n\tNombre:%s\n\tInicio: %i\n\tTamano: %i\n",master.mbr_partition_4.part_name,master.mbr_partition_4.part_start,master.mbr_partition_4.part_size);

    fclose(disco);*/
}

void crear_pl(char nombre[200], char path[200], int tamano, char fit,int unit){
    printf("-------------CREANDO PARTICION-------------\n");
    printf("%s\n",path);
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
        printf("ERROR:al cargar la data del disco\n");
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
    printf("Nombre: %s\nPath: %s\nTamano: %i\nFit: %c\nUnit: %i\nTipo: Logica\n",nombre,path,tamano_real,fit,unit);

    int parti = extendida_exist(leido);

    if(parti==0){
        printf("ERROR: no exite una particion extendida.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

    if(name_exist(disco,leido,nombre)==1){
        printf("ERROR: El nombre ya existe como particion.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        return;
    }

    EBR leido2;
    int tamano_particion = 0;
    int inicio = 0;

    switch(parti){
        case 1:
            fseek(disco,leido.mbr_partition_1.part_start,SEEK_SET);
            tamano_particion=leido.mbr_partition_1.part_size;
            inicio = leido.mbr_partition_1.part_start;
            break;
        case 2:
            fseek(disco,leido.mbr_partition_2.part_start,SEEK_SET);
            tamano_particion=leido.mbr_partition_2.part_size;
            inicio = leido.mbr_partition_2.part_start;
            break;
        case 3:
            fseek(disco,leido.mbr_partition_3.part_start,SEEK_SET);
            tamano_particion=leido.mbr_partition_3.part_size;
            inicio = leido.mbr_partition_2.part_start;
            break;
        case 4:
            fseek(disco,leido.mbr_partition_4.part_start,SEEK_SET);
            tamano_particion=leido.mbr_partition_4.part_size;
            inicio = leido.mbr_partition_2.part_start;
            break;
    }

    if(fread(&leido2,sizeof(EBR),1,disco)!=1){
        printf("ERROR:al cargar la data del disco\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

    int bandera = 1;
    int tamano_recorrido = (int)sizeof(EBR);

    while(1){
        if(leido2.part_status=='n'){
            if(tamano_real<=leido2.part_size){
                leido2.part_fit=fit;
                strcpy(leido2.part_name,nombre);
                leido2.part_size=tamano_real;
                leido2.part_start=inicio+tamano_recorrido;
                leido2.part_status='s';
                fseek(disco,-sizeof(EBR),SEEK_CUR);
                fwrite(&leido2,sizeof(EBR),1,disco);
                bandera = 0;
                break;
            }
        }
        if(leido2.part_next==-1){
            if(tamano_real<=(tamano_particion-(tamano_recorrido+leido2.part_size+sizeof(EBR)))){
                leido2.part_next=leido2.part_start+leido2.part_size;
                EBR nuevoebr;
                nuevoebr.part_fit=fit;
                strcpy(nuevoebr.part_name,nombre);
                nuevoebr.part_next=-1;
                nuevoebr.part_size=tamano_real;
                nuevoebr.part_start=leido2.part_start+leido2.part_size+sizeof(EBR);
                nuevoebr.part_status='s';
                fseek(disco,-sizeof(EBR),SEEK_CUR);
                fwrite(&leido2,sizeof(EBR),1,disco);
                fseek(disco,leido2.part_size,SEEK_CUR);
                fwrite(&nuevoebr,sizeof(EBR),1,disco);
                bandera=0;
                break;
            }else{
                bandera = -1;
                break;
            }
        }
        tamano_recorrido+=leido2.part_size+sizeof(EBR);
        fseek(disco,leido2.part_size,SEEK_CUR);
        fread(&leido2,sizeof(EBR),1,disco);
    }

    if(bandera==0){
        printf("-------------CREACION EXITOSA--------------\n\n");
    }else{
        printf("ERROR: no hay espacio disponible para crear la particion en la particion extendida.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
    }

    fclose(disco);

    /* prueba de datos */

/*    disco = fopen(path,"r+b");

    if(!disco){
        printf("ERROR: el Disco no ha podido Abrirse.\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        return;
    }

    if(fread(&leido,sizeof(MBR),1,disco)!=1){
        printf("ERROR:al cargar la data del disco\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

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

    parti = extendida_exist(leido);

    if(parti==0){
        printf("ERROR: no exite una particion extendida.\n");
        fclose(disco);
        return;
    }

    tamano_particion = 0;
    inicio = 0;

    switch(parti){
        case 1:
            if(fseek(disco,leido.mbr_partition_1.part_start,SEEK_SET)!=0){
                printf("cagada.\n");
                break;
            };
            tamano_particion=leido.mbr_partition_1.part_size;
            inicio = leido.mbr_partition_1.part_start;
            break;
        case 2:
            if(fseek(disco,leido.mbr_partition_2.part_start,SEEK_SET)!=0){
                printf("cagada.\n");
                break;
            };
            tamano_particion=leido.mbr_partition_2.part_size;
            inicio = leido.mbr_partition_2.part_start;
            break;
        case 3:
            if(fseek(disco,leido.mbr_partition_3.part_start,SEEK_SET)!=0){
                printf("cagada.\n");
                break;
            };
            tamano_particion=leido.mbr_partition_3.part_size;
            inicio = leido.mbr_partition_2.part_start;
            break;
        case 4:
            if(fseek(disco,leido.mbr_partition_4.part_start,SEEK_SET)!=0){
                printf("cagada.\n");
                break;
            };
            tamano_particion=leido.mbr_partition_4.part_size;
            inicio = leido.mbr_partition_2.part_start;
            break;
    }

    if(fread(&leido2,sizeof(EBR),1,disco)!=1){
        printf("ERROR:al cargar la data del disco\n");
        printf("-------------CREACION FALLIDA--------------\n\n");
        fclose(disco);
        return;
    }

    while(leido2.part_next!=-1){
        printf("EBR:\n\tFit: %c\n\tNombre: %s\n\tNext: %i\n\tSize: %i\n\tStart: %i\n\tStatus: %c\n",leido2.part_fit,leido2.part_name,leido2.part_next,leido2.part_size,leido2.part_start,leido2.part_status);
        if(fseek(disco,leido2.part_size,SEEK_CUR)!=0){
            printf("cagada.\n");
            break;
        };
        if(fread(&leido2,sizeof(EBR),1,disco)!=1){
            printf("cagada.\n");
            break;
        };
    }

    printf("EBR:\n\tFit: %c\n\tNombre: %s\n\tNext: %i\n\tSize: %i\n\tStart: %i\n\tStatus: %c\n",leido2.part_fit,leido2.part_name,leido2.part_next,leido2.part_size,leido2.part_start,leido2.part_status);

    fclose(disco);
*/
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
            if(fseek(disco,mbr.mbr_partition_1.part_start,SEEK_SET)!=0){
                printf("cagada.\n");
                break;
            };
            break;
        case 2:
            if(fseek(disco,mbr.mbr_partition_2.part_start,SEEK_SET)!=0){
                printf("cagada.\n");
                break;
            };
            break;
        case 3:
            if(fseek(disco,mbr.mbr_partition_3.part_start,SEEK_SET)!=0){
                printf("cagada.\n");
                break;
            };
            break;
        case 4:
            if(fseek(disco,mbr.mbr_partition_4.part_start,SEEK_SET)!=0){
                printf("cagada.\n");
            };
            break;
        }

        if(fread(&leido,sizeof(EBR),1,disco)!=1){
            printf("ERROR:al cargar la data del disco\n");
            fclose(disco);
            return-1;
        }

        while(leido.part_next!=-1){
            if(strcmp(leido.part_name,name)==0)return 1;
            //printf("EBR:\n\tFit: %c\n\tNombre: %s\n\tNext: %i\n\tSize: %i\n\tStart: %i\n\tStatus: %c\n",leido.part_fit,leido.part_name,leido.part_next,leido.part_size,leido.part_start,leido.part_status);
            if(fseek(disco,leido.part_size,SEEK_CUR)!=0){
                printf("cagada.\n");
                break;
            };
            if(fread(&leido,sizeof(EBR),1,disco)!=1){
                printf("cagada.\n");
                break;
            };
        }

        if(strcmp(leido.part_name,name)==0)return 1;
        //printf("EBR:\n\tFit: %c\n\tNombre: %s\n\tNext: %i\n\tSize: %i\n\tStart: %i\n\tStatus: %c\n",leido2.part_fit,leido2.part_name,leido2.part_next,leido2.part_size,leido2.part_start,leido2.part_status);
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

int extendida_exist(MBR mbr){
    int posicion = 0;

    if(mbr.mbr_partition_1.part_status!='n')
        if(mbr.mbr_partition_1.part_type=='e')
            posicion = 1;

    if(mbr.mbr_partition_2.part_status!='n')
        if(mbr.mbr_partition_2.part_type=='e')
            posicion = 2;

    if(mbr.mbr_partition_3.part_status!='n')
        if(mbr.mbr_partition_3.part_type=='e')
            posicion = 3;

    if(mbr.mbr_partition_4.part_status!='n')
        if(mbr.mbr_partition_4.part_type=='e')
            posicion = 4;

    return posicion;
}
