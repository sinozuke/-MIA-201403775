#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comandos.h"
#include "funciones.h"


int parametros(char *token);
int longitud_real(char *a);
char *valor_real(int a, char *b);
char *ncomillas(char *a);
int k_m(char *valor);
int pasa(char *token,int opcino1,int opcion2);
char BF_FF_WF(char *valor);

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
                    valor_r = valor_real(longitud_real(valor),valor);
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
        printf("-------------CREANDO PARTICION-------------\n");
        printf("-------------------DATOS-------------------\n");
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

    int add_delete=0;
    int size;
    int unit = 2;
    char *path;
    char *name;
    char type = 'p';
    char fit = 'w';
    int vdelete;
    int add;

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
                    sscanf(valor_r,"%d",&size);
                    if(size<=0){
                        printf("ERROR: el valor \"%s\" para el aprametro size no es aceptado.\n",valor_r);
                        return;
                    }
                    p_size = 1;
                    printf("%i\n",size);
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
                    valor_r = valor_real(longitud_real(valor),valor);
                    name = malloc(sizeof(char)*strlen(valor_r));
                    strcpy(name,valor_r);
                }else{
                    printf("ERROR: el parametro 'name' esta declarado mas de una vez.\n");
                    return;
                }
                printf("%s\n",name);
                p_name =1;
                break;
            case 5:
                if(!p_type){
                    //correccion para el tipo type :P u.u
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
                        printf("ERROR: valor \"%s\" no permitido para el parametro 'type'.\n",valor_r);
                        return;
                    }
                    if((fit=BF_FF_WF(valor_r))=='n'){
                        printf("ERROR: valor \"%s\" no permitido para el parametro 'type'.\n",valor_r);
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
                    printf("Parametro 'delete' reconocido.\n");
                }else{
                    printf("ERROR: el parametro 'delete' esta declarado mas de una vez.\n");
                    return;
                }
                break;
            case 8:
                if(!p_add){
                    if(p_delete){
                        printf("ERROR: declaracion previa de parametro incopatible 'delete'.\n");
                        return;
                    }
                    printf("Parametro 'add' reconocido.\n");
                }else{
                    printf("ERROR: el parametro 'add' esta declarado mas de una vez.\n");
                    return;
                }
                break;
            default:
                printf("Parametro \"%s\" NO reconocido.\n",token);
                return;
        }
        token = strtok(NULL, " ");
    }

}

void mount(char *token)
{

}

void umount(char *token)
{

}


void rep(char *token){


}

void exec(char *token){

}

char BF_FF_WF(char *valor){
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
    return 'n';
}

int k_m(char *valor){
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
    return 0;
}

int parametros(char *token){
    char *pos = strstr(token,"-size");
    if(pos){
        return 1;
    }
    pos = strstr(token,"+unit");
    if(pos){
        return 2;
    }
    pos = strstr(token,"-path");
    if(pos){
        return 3;
    }
    pos = strstr(token,"-name");
    if(pos){
        return 4;
    }
    pos = strstr(token,"+type");
    if(pos){
        return 5;
    }
    pos = strstr(token,"+fit");
    if(pos){
        return 6;
    }
    pos = strstr(token,"+delete");
    if(pos){
        return 7;
    }
    pos = strstr(token,"+add");
    if(pos){
        return 8;
    }
    return 0;
}

char *ncomillas(char *a){
    char *r = malloc(sizeof(char)*(strlen(a)-2));
    int i;
    for(i=0;a[i+1];i++){
        if(a[i+1]!='\"'){
            r[i]=a[i+1];
        }
    }
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
    if(strcmp(token, "mkdisk")==0)
        return 1;
    if(strcmp(token, "rmdisk")==0)
        return 2;
    if(strcmp(token, "fdisk")==0)
        return 3;
    if(strcmp(token, "mount")==0)
        return 4;
    if(strcmp(token, "umount")==0)
        return 5;
    if(strcmp(token, "rep")==0)
        return 6;
    if(strcmp(token, "exec")==0)
        return 7;
    if(strcmp(token, "exit")==0)
        return 8;
    return 0;
}
