#include <stdlib.h>
#include <string.h>
#include "comandos.h"

int parametros_mkdisk(char *token,int *p_size,int *p_unit,int *p_path,int *p_name);
int longitud_real(char *a);
char *valor_real(int a, char *b);
char *ncomillas(char *a);
int k_m(char *valor);


void mkdisk(char *token)
{
    char valor[500];
    char *valor_r;
    char *path;
    int valor_size;
    int p_size=0,p_unit=0,p_path=0,p_name=0;
    int kilo_mega=2;
    token = strtok(NULL, " ");
    int i;
    while (token)
    {
        switch(parametros_mkdisk(token,&p_size,&p_unit,&p_path,&p_name)){
            case 1:
                for(i=0;token[i+7]!='\0';i++)
                    valor[i]=token[i+7];
                valor[i]='\0';
                valor_r = valor_real(longitud_real(&valor),valor);
                sscanf(valor_r,"%d",&valor_size);
                if(valor_size<=0){
                    printf("ERROR: el valor \"%s\" para el aprametro size no es aceptado.\n",valor_r);
                    return;
                }
                //aqui ya se tiene la cantidad de bytes que se desea del disco a crearse
                break;
            case 2:
                for(i=0;token[i+7]!='\0';i++)
                    valor[i]=token[i+7];
                valor[i]='\0';
                valor_r = valor_real(longitud_real(&valor),valor);
                kilo_mega = k_m(valor_r);
                printf("%i\n",kilo_mega);
                if(kilo_mega==0){
                    printf("el valor \"%s\" para el parametro unit no es aceptado.\n",valor_r);
                    return;
                }
                break;
            case 3:
                for(i=0;token[i+7]!='\0';i++)
                    valor[i]=token[i+7];
                valor[i]='\0';
                valor_r = valor_real(longitud_real(&valor),valor);
                valor_r = ncomillas(valor_r);
                path = malloc(sizeof(char)*strlen(valor_r));
                strcpy(path,valor_r);
                break;
            case 4:
                printf("parametro name identificado");
                break;
            default:
                printf("Error: el parametro \"%s\" no es reconocido\n",token);
                return;
        }
        token = strtok(NULL, " ");
    }
    if(p_size*p_path*p_name){

    }else{
        if(!p_size)
            printf("",);
        return;
    }
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

void rmdisk(char *token)
{

}

void fdisk(char *token)
{

}

void mount(char *token)
{

}

void umount(char *token)
{

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
    return 0;
}

int parametros_mkdisk(char *token,int *p_size,int *p_unit,int *p_path,int *p_name){
    char *pos = strstr(token,"-size");
    if(pos){
        *p_size = 1;
        return 1;
    }
    pos = strstr(token,"+unit");
    if(pos){
        *p_unit = 1;
        return 2;
    }
    pos = strstr(token,"-path");
    if(pos){
        *p_path = 1;
        return 3;
    }
    pos = strstr(token,"-name");
    if(pos){
        *p_name = 1;
        return 4;
    }
    return 0;
}
