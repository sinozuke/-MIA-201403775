#include <stdlib.h>
#include "comandos.h"

void mkdisk(char *token)
{
    char* parametro1,parametro2;
    int p_size=0,p_unit=0,p_path=0,p_name=0;
    token = strtok(NULL, " ");
        while (token)
        {
            parametro1 = strtok(token,"::");
            parametro2 = strtok(parametro1,"::");
            printf("parametro:%s\n",parametro1);
            printf("valor:%s",parametro2);
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
