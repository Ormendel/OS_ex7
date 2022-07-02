#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define DISK_BLOCK_SIZE 512
#define NAME_SIZE 8
#define MAX_DIR_SIZE 10
#define MAX_FILES 10000


/* ======== General functions for files ======== */
void mymkfs(int s); // creates a new file system with size 's'

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);
int myopen(const char *pathname, int flags);
int myclose(int myfd);
ssize_t myread(int myfd, void *buf, size_t count);
ssize_t mywrite(int myfd, const void *buf, size_t count);
off_t mylseek(int myfd, off_t offset, int whence);


typedef struct inode
{
    int size;
    char name[NAME_SIZE];
    int next; //the next "cell" which we allocate to inode
    int isDIR; //0 for file, 1 for directory 

}inode;

/* ======== Functions of struct myDIR ======== */

typedef struct myDIR
{
    /* === In this struct we can't access directly to the (so called) key of the directory,
           we will just get it and manipulate with it in myreaddir function === */
    int key;
}myDIR;

myDIR *myopendir(const char *name);
int myclosedir(myDIR *dirp);

/* ======== Functions of struct mydirent ======== */

typedef struct  mydirent
{
    char d_name[MAX_DIR_SIZE]; // name of directory
    inode* files_in_directory; // data
}mydirent;

struct mydirent *myreaddir(myDIR *dirp);

// meta information about the file system
// number of inodes
// number of disk blocks
// size of the disk block


/* ======== Additional structs ======== */
/*
Superblock - The superblock is the very first block of the disk,
contains the essential meta-data about the file system such as the signature of the disk,
number of blocks it contains, the index of the root directory on the disk, 
the index of the data blocks on the disk, and the amount of FAT and Data blocks.*/
typedef struct superblock
{
    int num_inodes;
    int num_disk_blocks;
    int size_disk_blocks;
}superblock;

typedef struct disk_block
{
    char data[DISK_BLOCK_SIZE];
    int next_block_num;
}disk_block;

typedef struct myopenfile 
{

    int fd;
    int inode_index; 
    int file_position;

}open_file;

extern open_file my_open_files[MAX_FILES];
extern superblock* sb;
extern inode* inodes;
extern disk_block* dbs;