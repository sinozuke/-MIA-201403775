#include <stdlib.h>
#include <string.h>
#include "comandos.h"

int parametros_mkdisk(char *token,int *p_size,int *p_unit,int *p_path,int *p_name);

void mkdisk(char *token)
{
    char* parametro,valor;
    int p_size=0,p_unit=0,p_path=0,p_name=0;
    token = strtok(NULL, " ");
    while (token)
    {
        switch(parametros_mkdisk(token,&p_size,&p_unit,&p_path,&p_name)){
            case 1:
                strcpy(parametro,token);
                printf("%s\n",parametro);
                parametro = strcat(parametro,"hola");
                printf("%s\n",parametro);
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                printf("Error: el parametro \"%s\" no es reconocido\n",token);
                return;
        }
        token = strtok(NULL, " ");
    }
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
