#ifndef FUNC_GENERACION_H_INCLUDED
#define FUNC_GENERACION_H_INCLUDED


void generar_disco(char *nombre,char *path, int tamano, int tipo);
void eliminar_disco(char *path);
void crear_pp(char *nombre, char *path, int tamano, char fit,int unit);
void crear_pl(char *nombre, char *path, int tamano, char fit,int unit);
void crear_pe(char *nombre, char *path, int tamano, char fit,int unit);
void eliminar_particion(char *name,char *path);
void modificar_tamano_particion(char *name, char*path,char unit);
void mount_c(char *path,char *name);
void umount_c();

#endif // FUNC_GENERACION_H_INCLUDE
