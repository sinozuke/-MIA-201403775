#ifndef FUNC_GENERACION_H_INCLUDED
#define FUNC_GENERACION_H_INCLUDED


void generar_disco(char *nombre,char *path, int tamano, int tipo);
void eliminar_disco(char *path);
void crear_pp();
void crear_pl();
void crear_pe();
void mount_c(char *path,char *name);
void umount_c();

#endif // FUNC_GENERACION_H_INCLUDE
