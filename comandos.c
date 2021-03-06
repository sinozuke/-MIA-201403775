#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "comandos.h"
#include "funciones.h"


int parametros(char *token);
int longitud_real(char *a);
char *valor_real(int a, char *b);
char *ncomillas(char *a);
int k_m(char *valor);
int pasa(char *token,int opcino1,int opcion2);
char BF_FF_WF(char *valor);
char Fast_Full(char *valor);
int isnumero(char *token);
int name_rep(char *token);

void mkdisk(char *token)
{
    char valor[500];
    char *valor_r;
    char *path;
    char *nombre;
    int valor_size;
    int p_size=0,p_unit=0,p_path=0,p_name=0;
    int kilo_mega=1;
    int i;
    token = strtok(NULL, " ");
    while (token)
    {
        switch(parametros(token)){
            case 1:
                if(!p_size){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'size' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    valor_r = valor_real(longitud_real(valor),valor);
                    if(!isnumero(valor_r)){
                        printf("ERROR el Valor ingresado para size no es un numero.\n");
                        return;
                    }
                    sscanf(valor_r,"%d",&valor_size);
                    if(valor_size<=0){
                        printf("ERROR: el valor \"%s\" para el aprametro size no es aceptado.\n",valor_r);
                        return;
                    }
                }else{
                    printf("ERROR: Parametro 'size' declarado mas de una vez.\n");
                    return;
                }
                p_size = 1;
                break;
            case 2:
                if(!p_unit){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'unit' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    valor_r = valor_real(longitud_real(valor),valor);
                    kilo_mega = k_m(valor_r);
                    if(kilo_mega==0 || kilo_mega==3){
                        printf("el valor \"%s\" para el parametro unit no es aceptado.\n",valor_r);
                        return;
                    }
                }else{
                    printf("ERROR: Parametro 'unit' declarado mas de una vez.\n");
                    return;
                }
                p_unit = 1;
                break;
            case 3:
                if(!p_path){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'path' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    while(!pasa(valor,0,1)){
                        token = strtok(NULL," ");
                        if(!token){
                            printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                            return;
                        }
                        strcat(valor," ");
                        strcat(valor,token);
                    }
                    valor_r = valor_real(longitud_real(valor),valor);
                    valor_r = ncomillas(valor_r);
                    path = malloc(sizeof(char)*strlen(valor_r));
                    strcpy(path,valor_r);
                }else{
                    printf("ERROR: Parametro 'path' declarado mas de una vez.\n");
                    return;
                }
                p_path = 1;
                break;
            case 4:
                if(!p_name){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'name' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    while(!pasa(valor,0,1)){
                        token = strtok(NULL," ");
                        if(!token){
                            printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                            return;
                        }
                        strcat(valor," ");
                        strcat(valor,token);
                    }
                    valor_r = valor_real(longitud_real(valor),valor);
                    valor_r = ncomillas(valor_r);
                    nombre = malloc(sizeof(char)*strlen(valor_r));
                    strcpy(nombre,valor_r);
                }else{
                    printf("ERROR: Parametro 'name' esta declarado mas de una vez.\n");
                    return;
                }
                p_name = 1;
                break;
            default:
                printf("Error: el parametro \"%s\" no es reconocido\n",token);
                return;
        }
        token = strtok(NULL, " ");
    }
    if(p_size*p_path*p_name){
        printf("-------------CREANDO DISCO-------------\n");
        printf("-----------------DATOS-----------------\n");
        if(kilo_mega==1)
            printf("Tamaño:\t%i\nMedida:\t%s\nPath:\t%s\nNombre:\t%s\n",valor_size,"Mb",path,nombre);
        else
            printf("Tamaño:\t%i\nMedida:\t%s\nPath:\t%s\nNombre:\t%s\n",valor_size,"Kb",path,nombre);
        printf("-----------------GENERANDO-----------------\n");
        if(kilo_mega==1)
            generar_disco(nombre,path,valor_size,1);
        else
            generar_disco(nombre,path,valor_size,0);
    }else{
        if(!p_size)
            printf("el parametro 'size' no se ha encontrado\n");
        if(!p_name)
            printf("el parametro 'name' no se ha encontrado\n");
        if(!p_path)
            printf("el parametro 'path' no se ha encontrado\n");
        return;
    }
    free(valor_r);
    free(path);
    free(nombre);
}

void rmdisk(char *token){
    char valor[500];
    char *valor_r;
    char *path;
    int p_path=0;
    token = strtok(NULL, " ");
    int i;
    while (token)
    {
        if(parametros(token)==3){
            if(!p_path){
                if(!pasa(token,1,0)){
                    printf("ERROR: no se presenta el separador '::' entre el parametro 'path' y el valor.\n");
                    return;
                }
                if(!pasa(token,0,1)){
                    printf("ERROR: el valor del parametro 'path' no esta encerrado entre comillas.\n");
                    return;
                }
                for(i=0;token[i+7]!='\0';i++)
                    valor[i]=token[i+7];
                valor[i]='\0';
                valor_r = valor_real(longitud_real(valor),valor);
                valor_r = ncomillas(valor_r);
                path = malloc(sizeof(char)*strlen(valor_r));
                strcpy(path,valor_r);
            }else{
                printf("ERROR: Parametro 'path' declarado mas de una vez.\n");
                return;
            }
            p_path = 1;
        }else{
            printf("Error: el parametro \"%s\" no es reconocido\n",token);
            return;
        }
        token = strtok(NULL, " ");
    }
    if(p_path){
        printf("---------------BORRANDO DISCO---------------\n");
        eliminar_disco(path);
    }else{
            printf("el parametro 'path' no se ha encontrado\n");
        return;
    }
}

void fdisk(char *token)
{
    int p_size=0;
    int p_unit=0;
    int p_path=0;
    int p_name=0;
    int p_type=0;
    int p_fit=0;
    int p_delete=0;
    int p_add=0;

    int add=0;
    int size;
    int unit = 2;
    char path[200];
    char *name;
    char type = 'p';
    char fit = 'w';
    char vdelete;

    char valor[500];
    char *valor_r;
    int i;

    token = strtok(NULL, " ");
    while (token)
    {
        switch(parametros(token)){
            case 1:
                if(!p_size){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'size' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    valor_r = valor_real(longitud_real(valor),valor);
                    if(!isnumero(valor_r)){
                        printf("ERROR el Valor ingresado para size no es un numero.\n");
                        return;
                    }
                    sscanf(valor_r,"%d",&size);
                    if(size<=0){
                        printf("ERROR: el valor \"%s\" para el aprametro size no es aceptado.\n",valor_r);
                        return;
                    }
                    p_size = 1;
                }else{
                    printf("ERROR: el parametro 'size' esta declarado mas de una vez.\n");
                    return;
                }
                break;
            case 2:
                if(!p_unit){
                    if(!p_unit){
                        if(!pasa(token,1,0)){
                            printf("ERROR: no se presenta el separador '::' entre el parametro 'unit' y el valor.\n");
                            return;
                        }
                        for(i=0;token[i+7]!='\0';i++)
                            valor[i]=token[i+7];
                        valor[i]='\0';
                        valor_r = valor_real(longitud_real(valor),valor);
                        unit = k_m(valor_r);
                        if(unit==0){
                            printf("el valor \"%s\" para el parametro unit no es aceptado.\n",valor_r);
                            return;
                        }
                    }else{
                        printf("ERROR: Parametro 'unit' declarado mas de una vez.\n");
                        return;
                    }
                    p_unit = 1;
                }else{
                    printf("ERROR: el parametro 'unit' esta declarado mas de una vez.\n");
                    return;
                }
                break;
            case 3:
                if(!p_path){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'path' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    while(!pasa(valor,0,1)){
                        token = strtok(NULL," ");
                        if(!token){
                            printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                            return;
                        }
                        strcat(valor," ");
                        strcat(valor,token);
                    }
                    valor_r = valor_real(longitud_real(valor),valor);
                    valor_r = ncomillas(valor_r);
                    strcpy(path,valor_r);
                    printf("%s\n",path);
                }else{
                    printf("ERROR: el parametro 'path' esta declarado mas de una vez.\n");
                    return;
                }
                p_path=1;
                break;
            case 4:
                if(!p_name){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'name' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    while(!pasa(valor,0,1)){
                        token = strtok(NULL," ");
                        if(!token){
                            printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                            return;
                        }
                        strcat(valor," ");
                        strcat(valor,token);
                    }
                    valor_r = valor_real(longitud_real(valor),valor);
                    valor_r = ncomillas(valor_r);
                    name = malloc(sizeof(char)*strlen(valor_r));
                    strcpy(name,valor_r);
                }else{
                    printf("ERROR: el parametro 'name' esta declarado mas de una vez.\n");
                    return;
                }
                p_name =1;
                break;
            case 5:
                if(!p_type){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'type' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    valor_r = valor_real(longitud_real(valor),valor);
                    if(strlen(valor_r)!=1){
                        printf("ERROR: valor \"%s\" no permitido para el parametro 'type'.\n",valor_r);
                        return;
                    }
                    type=valor_r[0];
                    if(!(type=='P' || type=='E' || type=='L'||type=='p' || type=='e' || type=='l')){
                        printf("ERROR: valor \"%s\" no permitido para el parametro 'type'.\n",valor_r);
                        return;
                    }
                }else{
                    printf("ERROR: el parametro 'type' esta declarado mas de una vez.\n");
                    return;
                }
                p_type=1;
                break;
            case 6:
                if(!p_fit){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'type' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+6]!='\0';i++)
                        valor[i]=token[i+6];
                    valor[i]='\0';
                    valor_r = valor_real(longitud_real(valor),valor);
                    if(strlen(valor_r)!=2){
                        printf("ERROR: valor \"%s\" no permitido para el parametro 'fit'.\n",valor_r);
                        return;
                    }
                    if((fit=BF_FF_WF(valor_r))=='n'){
                        printf("ERROR: valor \"%s\" no permitido para el parametro 'fit'.\n",valor_r);
                        return;
                    }
                }else{
                    printf("ERROR: el parametro 'fit' esta declarado mas de una vez.\n");
                    return;
                }
                p_fit = 1;
                break;
            case 7:
                if(!p_delete){
                    if(p_add){
                        printf("ERROR: declaracion previa de parametro incopatible 'add'.\n");
                        return;
                    }
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'delete' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+9]!='\0';i++)
                        valor[i]=token[i+9];
                    valor[i]='\0';
                    valor_r = valor_real(longitud_real(valor),valor);
                    vdelete = Fast_Full(valor_r);

                    if(vdelete=='n'){
                        printf("ERROr el valor \"%s\" para el parametro 'delete' no es valido.\n",valor_r);
                        return;
                    }
                }else{
                    printf("ERROR: el parametro 'delete' esta declarado mas de una vez.\n");
                    return;
                }
                p_delete=1;
                break;
            case 8:
                if(!p_add){
                    if(p_delete){
                        printf("ERROR: declaracion previa de parametro incopatible 'delete'.\n");
                        return;
                    }
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'delete' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+6]!='\0';i++)
                        valor[i]=token[i+6];
                    valor[i]='\0';
                    valor_r = valor_real(longitud_real(valor),valor);
                    if(!isnumero(valor_r)){
                        printf("ERROR el Valor ingresado para size no es un numero.\n");
                        return;
                    }
                    add = atoi(valor_r);
                    if(add==0){
                        printf("ERROr el valor \"%i\" para el parametro 'add' no es valido.\n",add);
                        return;
                    }
                }else{
                    printf("ERROR: el parametro 'add' esta declarado mas de una vez.\n");
                    return;
                }
                p_add=1;
                break;
            default:
                printf("Parametro \"%s\" NO reconocido.\n",token);
                return;
        }
        token = strtok(NULL, " ");
    }
    if(p_delete){
        if(p_name*p_path){
            if(p_fit)
                printf("ADVERTENCIA: El parametro 'fit' sera ignorado dado que no es nesesario para su eliminacion.\n");
            if(p_type)
                printf("ADVERTENCIA: El parametro 'type' sera ignorado dado que no es nesesario para su eliminacion.\n");
            if(p_unit)
                printf("ADVERTENCIA: El parametro 'unit' sera ignorado dado que no es nesesario para su eliminacion.\n");
            if(p_size)
                printf("ADVERTENCIA: El parametro 'size' sera ignorado dado que no es nesesario para su eliminacion.\n");
            eliminar_particion(name,path,vdelete);
        }else{
            if(!p_name)
                printf("ERROR: para eliminar una particion es nesario el nombre de la misma.\n");
            if(!p_path)
                printf("ERROR: para eliminar una particion es nesario el path en donde se encuentra el disco.\n");
            return;
        }
    }else if(p_add){
        if(p_unit*p_name*p_path){
            if(p_size)
                printf("ADVERTENCIA: El parametro 'size' sera ignorado dado que no es nesesario para su eliminacion.\n");
            if(p_type)
                printf("ADVERTENCIA: El parametro 'type' sera ignorado dado que no es nesesario para su eliminacion.\n");
            if(p_fit)
                printf("ADVERTENCIA: El parametro 'fit' sera ignorado dado que no es nesesario para su eliminacion.\n");
            switch(unit){
                case 1:
                    modificar_tamano_particion(name,path,'m',add);
                    break;
                case 2:
                    modificar_tamano_particion(name,path,'k',add);
                    break;
                case 3:
                    modificar_tamano_particion(name,path,'b',add);
                    break;
            }
        }else{
            if(!p_name)
                printf("ERROR: para modificar el tamano en una particion es nesario el nombre de la misma.\n");
            if(!p_path)
                printf("ERROR: para modificar el tamano en una particion es nesario el path del disco en donde esta la misma.\n");
            if(!p_unit)
                printf("ERROR: para modificar el tamano en una particion es nesario especificar en que medida se aumentara/disminuira la misma.\n");
            return;
        }
    }else{
        if(p_name*p_path*p_size){
            if(((int)strlen(name))>15){
                printf("ERROR: el nombre para la particion exede el tamano permitido de 14 caracteres.\n");
                return;
            }
            switch(unit){
                case 1:
                    if(size<10){
                        printf("ERROR: el tamano especificado es menor al limite permitido (10 MB).\n");
                        return;
                    }
                    break;
                case 2:
                    if(size/1024<10){
                        printf("ERROR: el tamano especificado es menor al limite permitido (10,240 KB).\n");
                        return;
                    }
                    break;
                case 3:
                    if(((size)/1024)/1024<10){
                        printf("ERROR: el tamano especificado es menor al limite permitido (10,485,760 B).\n");
                        return;
                    }
                    break;
            }
            if(type=='p' || type=='P')
                crear_pp(name,path,size,fit,unit);
            if(type=='e' || type=='E')
                crear_pe(name,path,size,fit,unit);
            if(type=='l' || type=='L')
                crear_pl(name,path,size,fit,unit);
        }else{
            if(!p_name)
                printf("ERROR: para crear una particion se nesesita de un nombre.\n");
            if(!p_size)
                printf("ERROR: para crear una particion se nesesita de un tamano.\n");
            if(!p_path)
                printf("ERROR: para crear una particion se nesesita del directorio en donde esta el disco que la alojara.\n");
            return;
        }
    }


}

void mount(char *token)
{
    int p_path=0;
    int p_name=0;

    char path[200];
    char *name;

    char valor[500];
    char *valor_r;
    int i;

    token = strtok(NULL, " ");
    while (token)
    {
        switch(parametros(token)){
            case 3:
                if(!p_path){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'path' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    while(!pasa(valor,0,1)){
                        token = strtok(NULL," ");
                        if(!token){
                            printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                            return;
                        }
                        strcat(valor," ");
                        strcat(valor,token);
                    }
                    valor_r = valor_real(longitud_real(valor),valor);

                    valor_r = ncomillas(valor_r);

                    strcpy(path,valor_r);
                    printf("%s\n",path);
                }else{
                    printf("ERROR: el parametro 'path' esta declarado mas de una vez.\n");
                    return;
                }
                p_path=1;
                break;
            case 4:
                if(!p_name){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'name' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    while(!pasa(valor,0,1)){
                        token = strtok(NULL," ");
                        if(!token){
                            printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                            return;
                        }
                        strcat(valor," ");
                        strcat(valor,token);
                    }
                    valor_r = valor_real(longitud_real(valor),valor);
                    valor_r = ncomillas(valor_r);
                    name = malloc(sizeof(char)*strlen(valor_r));
                    strcpy(name,valor_r);
                }else{
                    printf("ERROR: el parametro 'name' esta declarado mas de una vez.\n");
                    return;
                }
                p_name =1;
                break;
                default:
                printf("Parametro \"%s\" NO reconocido.\n",token);
                return;
        }
        token = strtok(NULL, " ");
    }
    if(p_path*p_name){
        mount_c(path,name);
    }else{
        if(!p_path){
            printf("ERROR: falta el parametro 'path'.\n");
        }
        if(!p_name){
            printf("ERROR: falta el parametro 'name'.\n");
        }
    }
}

void umount(char *token)
{

}


void rep(char *token){
    int p_name=0;
    int p_id=0;
    int p_ruta=0;
    int p_path=0;

    char path[200];
    char ruta[200];
    char id[200];
    int name=0;

    char valor[500];
    char *valor_r;
    int i;

    token = strtok(NULL, " ");
    while (token)
    {
        switch(parametros(token)){
            case 3:
                if(!p_path){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'path' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    while(!pasa(valor,0,1)){
                        token = strtok(NULL," ");
                        if(!token){
                            printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                            return;
                        }
                        strcat(valor," ");
                        strcat(valor,token);
                    }
                    valor_r = valor_real(longitud_real(valor),valor);

                    valor_r = ncomillas(valor_r);

                    strcpy(path,valor_r);
                    printf("%s\n",path);
                }else{
                    printf("ERROR: el parametro 'path' esta declarado mas de una vez.\n");
                    return;
                }
                p_path=1;
                break;
            case 4:
                if(!p_name){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'name' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    while(!pasa(valor,0,1)){
                        token = strtok(NULL," ");
                        if(!token){
                            printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                            return;
                        }
                        strcat(valor," ");
                        strcat(valor,token);
                    }
                    valor_r = valor_real(longitud_real(valor),valor);
                    valor_r = ncomillas(valor_r);
                    name=name_rep(valor_r);
                    if(name==0){
                        printf("ERROR: el valor para el parametro name no es valido.\n");
                        return;
                    }
                }else{
                    printf("ERROR: el parametro 'name' esta declarado mas de una vez.\n");
                    return;
                }
                p_name =1;
                break;
                case 9:
                    if(!p_id){
                        if(!pasa(token,1,0)){
                            printf("ERROR: no se presenta el separador '::' entre el parametro 'name' y el valor.\n");
                            return;
                        }
                        for(i=0;token[i+5]!='\0';i++)
                            valor[i]=token[i+5];
                        valor[i]='\0';
                        while(!pasa(valor,0,1)){
                            token = strtok(NULL," ");
                            if(!token){
                                printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                                return;
                            }
                            strcat(valor," ");
                            strcat(valor,token);
                        }
                        valor_r = valor_real(longitud_real(valor),valor);
                        valor_r = ncomillas(valor_r);
                        strcpy(id,valor_r);
                    }else{
                        printf("ERROR: el parametro 'id' esta declarado mas de una vez.\n");
                        return;
                    }
                    p_id =1;
                    break;
                case 10:
                    if(!p_ruta){
                        if(!pasa(token,1,0)){
                            printf("ERROR: no se presenta el separador '::' entre el parametro 'name' y el valor.\n");
                            return;
                        }
                        for(i=0;token[i+7]!='\0';i++)
                            valor[i]=token[i+7];
                        valor[i]='\0';
                        while(!pasa(valor,0,1)){
                            token = strtok(NULL," ");
                            if(!token){
                                printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                                return;
                            }
                            strcat(valor," ");
                            strcat(valor,token);
                        }
                        valor_r = valor_real(longitud_real(valor),valor);
                        valor_r = ncomillas(valor_r);
                        strcpy(ruta,valor_r);
                    }else{
                        printf("ERROR: el parametro 'ruta' esta declarado mas de una vez.\n");
                        return;
                    }
                    p_ruta =1;
                    break;
                default:
                    printf("Parametro \"%s\" NO reconocido.\n",token);
                return;
        }
        token = strtok(NULL, " ");
    }
    if(p_path*p_name*p_id){
        switch(name){
            case 1:
                mbr_r(id,path);
                break;
            case 2:
                disk_r(id,path);
                break;
            case 3:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
            case 4:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
            case 5:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
            case 6:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
            case 7:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
            case 8:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
            case 9:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
            case 10:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
            case 11:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
            case 12:
                printf("-- Disculpa, reporte aun no valido --\n");
                break;
        }
    }else{
        if(!p_path)
            printf("ERROR: falta el parametro path.\n");
        if(!p_name)
            printf("ERROR: falta el parametro name.\n");
        if(!p_id)
            printf("ERROR: falta el parametro id.\n");
        return;
    }

}

void exec(char *token){
    int p_path=0;

    char path[200];

    char valor[500];
    char *valor_r;
    int i;

    token = strtok(NULL, " ");
    while (token)
    {
        switch(parametros(token)){
            case 3:
                if(!p_path){
                    if(!pasa(token,1,0)){
                        printf("ERROR: no se presenta el separador '::' entre el parametro 'path' y el valor.\n");
                        return;
                    }
                    for(i=0;token[i+7]!='\0';i++)
                        valor[i]=token[i+7];
                    valor[i]='\0';
                    while(!pasa(valor,0,1)){
                        token = strtok(NULL," ");
                        if(!token){
                            printf("ERROR: se ha llegado al final de lectura sin terminar el valor con la doble comillas.\n");
                            return;
                        }
                        strcat(valor," ");
                        strcat(valor,token);
                    }
                    valor_r = valor_real(longitud_real(valor),valor);

                    valor_r = ncomillas(valor_r);

                    strcpy(path,valor_r);
                    printf("%s\n",path);
                }else{
                    printf("ERROR: el parametro 'path' esta declarado mas de una vez.\n");
                    return;
                }
                p_path=1;
                break;
                default:
                printf("Parametro \"%s\" NO reconocido.\n",token);
                return;
        }
        token = strtok(NULL, " ");
    }
    if(p_path){
            struct stat st = {0};
            printf("%s\n",path);
            if(stat(path,&st)==-1){
                printf("ERROR: el archivo especificado no existe.\n");
                printf("-------------CARGA FALLIDA--------------\n\n");
                return;
            }

            FILE *execa;

            execa = fopen(path,"r");

            if(!execa){
                printf("ERROR: el Disco no ha podido Abrirse.\n");
                printf("-------------CARGA FALLIDA--------------\n\n");
                return;
            }

            long size;
            fseek(execa,0,SEEK_END);
            size = ftell(execa);
            fseek(execa,0,SEEK_SET);
            char contenido [size];
            fread(contenido,1,size,execa);
            fclose(execa);

            int ban=0;

            for(int i=0;contenido[i];i++){
                if(contenido[i]=='\\'){
                    ban = 1;
                    contenido[i]=' ';
                }else{
                    if(ban){
                        if(contenido[i]=='\n'){
                            ban=0;
                            contenido[i]=' ';
                        }
                    }else{
                        contenido[i]=contenido[i];
                    }
                }
            }

            printf("%s\n",contenido);
/*
            char *tokenp;
            char *tokenp2;
            char comando[100];
            tokenp = strtok(cont,"\n");
            strcpy(comando,tokenp);
            tokenp2 = strcpy(comando," ");
            while(tokenp){
                while (tokenp2) {
                    switch(opcion(token)){
                        case -1:
                            printf("%s\n",tokenp2);
                            break;
                        case 1:
                            mkdisk(tokenp2);
                            break;
                        case 2:
                            rmdisk(tokenp2);
                            break;
                        case 3:
                            fdisk(tokenp2);
                            break;
                        case 4:
                            mount(tokenp2);
                            break;
                        case 5:
                            printf("Comando UMount invocado\n");
                            break;
                        case 6:
                            rep(tokenp2);
                            break;
                        default:
                            printf("ERROR: el comando \"%s\" no reconocido\n",tokenp2);
                            break;
                    }
                    while (token)
                        tokenp2 = strtok(NULL, " ");
                }
                tokenp = strtok(NULL,"\n");
                strcpy(comando,tokenp);
            }
    }else{
        printf("ERROR: falta el parametro 'path'.\n");
    */}
}


int isnumero(char *token){

    for(int i=0;token[i]!='\0';i++){
        if(isalpha(token[i])){
            return 0;
        }
    }

    return 1;
}

char Fast_Full(char *valor){
    if(strlen(valor)==4){
        char *pos = strstr(valor,"fast");
        if(pos){
            return 'a';
        }
        pos = strstr(valor,"full");
        if(pos){
            return 'u';
        }
    }
    return 'n';

}

char BF_FF_WF(char *valor){
    if(strlen(valor)==2){
        char *pos = strstr(valor,"bf");
        if(pos){
            return 'b';
        }
        pos = strstr(valor,"ff");
        if(pos){
            return 'f';
        }
        pos = strstr(valor,"wf");
        if(pos){
            return 'w';
        }
    }
    return 'n';
}

int k_m(char *valor){
    if(strlen(valor)==1){
        char *pos = strstr(valor,"m");
        if(pos){
            return 1;
        }
        pos = strstr(valor,"k");
        if(pos){
            return 2;
        }
        pos = strstr(valor,"b");
        if(pos){
            return 3;
        }
    }
    return 0;
}

int name_rep(char *token){

    char cadena[100];

    strcpy(cadena,token);

    for(int i = 0; cadena[i]; i++)
        cadena[i] = tolower(cadena[i]); //<--convierte a minusculas

    char *pos = strstr(cadena,"mbr");
    if(pos){
        return 1;
    }
    pos = strstr(cadena,"disk");
    if(pos){
        return 2;
    }
    pos = strstr(cadena,"inode");
    if(pos){
        return 3;
    }
    pos = strstr(cadena,"journaling");
    if(pos){
        return 4;
    }
    pos = strstr(cadena,"block");
    if(pos){
        return 5;
    }
    pos = strstr(cadena,"bm_inode");
    if(pos){
        return 6;
    }
    pos = strstr(cadena,"bm_block");
    if(pos){
        return 7;
    }
    pos = strstr(cadena,"tree");
    if(pos){
        return 8;
    }
    pos = strstr(cadena,"sb");
    if(pos){
        return 9;
    }
    pos = strstr(cadena,"file");
    if(pos){
        return 10;
    }
    pos = strstr(cadena,"ls+i");
    if(pos){
        return 11;
    }
    pos = strstr(cadena,"ls+s");
    if(pos){
        return 12;
    }
    return 0;
}


int parametros(char *token){

    char cadena[100];

    strcpy(cadena,token);

    for(int i = 0; cadena[i]; i++)
        cadena[i] = tolower(cadena[i]); //<--convierte a minusculas

    char *pos = strstr(cadena,"-size");
    if(pos){
        return 1;
    }
    pos = strstr(cadena,"+unit");
    if(pos){
        return 2;
    }
    pos = strstr(cadena,"-path");
    if(pos){
        return 3;
    }
    pos = strstr(cadena,"-name");
    if(pos){
        return 4;
    }
    pos = strstr(cadena,"+type");
    if(pos){
        return 5;
    }
    pos = strstr(cadena,"+fit");
    if(pos){
        return 6;
    }
    pos = strstr(cadena,"+delete");
    if(pos){
        return 7;
    }
    pos = strstr(cadena,"+add");
    if(pos){
        return 8;
    }
    pos = strstr(cadena,"-id");
    if(pos){
        return 9;
    }
    pos = strstr(cadena,"+ruta");
    if(pos){
        return 10;
    }
    return 0;
}

char *ncomillas(char *a){
    char *r = malloc(sizeof(char)*strlen(a));
    int i;
    for(i=0;a[i+1]!='\"';i++){
        if(a[i+1]!='\"'){
            r[i]=a[i+1];
        }
    }
    r[i]='\0';
    return r;
}

char *valor_real(int a, char *b){
    char *r = malloc(sizeof(char)*(a+1));
    int i;
    for(i = 0;i<a;i++){
        r[i]=b[i];
    }
    r[i]='\0';
    return r;
}

int longitud_real(char *a){
    int i=1;
    while(a[i-1]!='\0')
        i++;
    return i-1;
}

int pasa(char *token,int opcion1,int opcion2){
    int dos_puntos = 0;
    int comillas =0;
    int i;
    for(i=0;token[i];i++)
        if(token[i]==':')
            dos_puntos++;
        else if(token[i]=='\"')
            comillas++;

    if(opcion1)
        if(dos_puntos==2)
            return 1;
        else
            return 0;
    else
        if(comillas==2)
            return 1;
        else
            return 0;
}

int opcion(char *token){

    char cadena[100];

    strcpy(cadena,token);

    for(int i = 0; cadena[i]; i++)
        cadena[i] = tolower(cadena[i]); //<--convierte a minusculas

    if(strcmp(cadena, "mkdisk")==0)
        return 1;
    if(strcmp(cadena, "rmdisk")==0)
        return 2;
    if(strcmp(cadena, "fdisk")==0)
        return 3;
    if(strcmp(cadena, "mount")==0)
        return 4;
    if(strcmp(cadena, "umount")==0)
        return 5;
    if(strcmp(cadena, "rep")==0)
        return 6;
    if(strcmp(cadena, "exec")==0)
        return 7;
    if(strcmp(cadena, "exit")==0)
        return 8;
    if(strstr(cadena,"#")==0){
        return -1;
    }
    return 0;
}
