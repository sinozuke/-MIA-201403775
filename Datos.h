#ifndef DATOS_H_INCLUDED
#define DATOS_H_INCLUDED

#include <time.h>

typedef struct Partition{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
} particion;

typedef struct Master_Boot_Record{
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    particion mbr_partition_1;
    particion mbr_partition_2;
    particion mbr_partition_3;
    particion mbr_partition_4;
} MBR;

typedef struct Extended_Boot_Record{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
} EBR;

typedef struct Super_Bloque{
    int s_file_system_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    time_t s_mtime;
    time_t s_umtime;
    int s_mnt_count;
    int s_magic;
    int s_inode_size;
    int s_block_size;
    int s_first_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
} SB;

typedef struct Inodo{
    int i_nlink;
    char i_pathlink;
    int i_uid;
    int I_gid;
    int i_size;
    time_t i_atime;
    time_t i_ctime;
    time_t i_mtime;
    int i_block[15];
    char i_type;
    int i_perm;
} inodo;

typedef struct Content{
    char bname[12];
    int b_inodo;
} content;

#endif // DATOS_H_INCLUDED
