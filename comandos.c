#include <stdlib.h>
#include <string.h>
#include "comandos.h"

int parametros_mkdisk(char *token,int *p_size,int *p_unit,int *p_path,int *p_name);
int longitud_real(char *a);
char *valor_real(int a, char *b);
void mkdisk(char *token)
{
    char valor[500];
    char *valor_r;
    int a;
    int p_size=0,p_unit=0,p_path=0,p_name=0;
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
                sscanf(valor_r,"%d",&a);
                printf("%i\n",a);
                printf("%i\n",a+1);
                break;
            case 2:
                printf("parametro unit identificado");
                break;
            case 3:
                printf("parametro path identificado");
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
