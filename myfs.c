#include "myfs.h"

// initialize new file system
void mymkfs(int s)
{

    // Allocate memory for superblock
    sb = (superblock*)malloc(sizeof(superblock));

    sb->num_inodes = (s/10)/sizeof(inode); //allocate 10% from s to inodes
    int size_without_sb_inodes = sb->num_inodes - sizeof(superblock); // one block for super_block
    sb->num_disk_blocks = size_without_sb_inodes;

    // Allocate Memory for inodes
    inodes = (inode*)malloc((sb->num_inodes)*sizeof(inode));
    for (size_t i = 0; i < sb->num_inodes; ++i)
    {
        strcpy(inodes[i].name, "");
        inodes[i].next = -1;
    }

    // Allocate Memory for disk blocks
    dbs = (disk_block*)malloc((sb->num_disk_blocks)*sizeof(disk_block));
    for (size_t i = 0; i < sb->num_disk_blocks; i++)
    {
        strcpy(dbs[i].data, "");
        dbs[i].next_block_num = -1;
    }

    create_file_fs("fs"); //create our file system
    
}

void create_file_fs(const char* path)
{

    FILE *file;
    file = fopen(path, "w+");
    fwrite(sb, sizeof(superblock), 1, file);
    fwrite(inodes, (sb->num_inodes)*sizeof(struct inode), 1, file);
    fwrite(dbs, (sb->num_disk_blocks)*sizeof(struct disk_block), 1, file);
    fclose(file);


}


// load a file system
int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data)
{
    // target is the path to the FILE that represents our fs

    if (source==NULL&&target==NULL) 
        return -1;
    create_file_fs(target);
    return 0;

}

// open a file in our fs
int myopen(const char *pathname, int flags) 
{

    /* ==== Searching for available fd ==== */
    int available_j = 3;
    for(int index = available_j; index < MAX_FILES; ++index)
    {
        if(my_open_files[index].fd == -1)
            available_j = index;
    }

    /* ==== Initialize fd in the available space in open_files table ==== */
    for ( int i = 0; i<sb->num_inodes; i++)
    {
        if ( strcmp(pathname,inodes[i].name) == 0 ) 
        {
            my_open_files[available_j].fd = i;
            my_open_files[available_j].inode_index = i;
            my_open_files[available_j].file_position = 0;
            return i;
        }
    }

    return -1;

}

int allocate_file( int size ,const char* name)
{

    // This function allocate inode and enough blocks for a new file in our fs.
   
   // name size limit is 8
   if(strlen(name)>= 8){
       return -1;
   }

   int empty_node = find_empty_node();
   if( empty_node == -1)
        return -1;

    int empty_block = find_empty_block();
    if(empty_block == -1)
        return -1;

    // Set the name of the file
    strcpy(inodes[empty_node].name, name);

    inodes[empty_node].size = size;
    inodes[empty_block].next = empty_block;
    dbs[empty_block].next_block_num = -2; // Mark as captured disk block

    // Return a file 
    return empty_node;

}

int find_empty_node()
{


    for ( int i = 0 ;i < sb->num_inodes ;i++)
    {

        if(inodes[i].next == -1)
        {
            return i;
        }

    }
    return -1;
}

int find_empty_block()
{


    for ( int i = 0 ;i < sb->num_disk_blocks ;i++)
    {

        if(dbs[i].next_block_num == -1)
        {
            return i;
        }

    }
    return -1;
}

int myclose(int myfd) 
{
    my_open_files[myfd].fd = -1;
    my_open_files[myfd].file_position = -1;
}

char read_byte(int fd, void* start, int size_to_read) 
{
    // fd = file descriptor
    // size_to_read = how many bytes are we going to read (divided to inodes)
    // start = from which position are we going to read from 

    int start_conv = (int)start;
    int block = inodes[fd].next;

    char c = dbs[block].data[start_conv];

    return c;

}

ssize_t myread(int myfd, void *buf, size_t count)
{

    // If file doesn't exist
    if(my_open_files[myfd].fd == -1 ){
        return -1;
    }

    char *output = (char*)(malloc((sizeof(char)*count) + 1 ));
    for ( int i = 0; i< count; i++)
    {
        output[i] = read_byte(myfd,buf,count);
        my_open_files[myfd].file_position++;
        *(buf)++; //increament variable buf - not sure if it's correct.
    }

    return my_open_files[myfd].file_position;

}

ssize_t mywrite(int myfd, const void *buf, size_t count)
{

    // If file doesn't exist in our open file table
    if(my_open_files[myfd].fd == -1 )
        return -1;
    
    char *output = (char*)buf;
    for ( int i = 0; i< count; i++)
    {
        write_byte(myfd,buf,output[i]);
        my_open_files[myfd].file_position++;
        *(buf)++; //increament variable buf - not sure if it's correct.
    }

    return my_open_files[myfd].file_position;
    
}

void write_byte(int fd, void* start, char c) 
{
    // fd = file descriptor
    // c = the byte we are going to write
    // start = to which position are we going to write

    int start_conv = (int)start;
    int block = inodes[fd].next;
    dbs[block].data[start_conv] = c;

}

off_t mylseek(int myfd, off_t offset, int whence)
{

    // offset - for example when a program is accessing an array of bytes, the fifth 
    // byte is offset by four bytes from the array's beginning
    // implemenation of SEEK_CUR, SEEK_END and SEEK_SET are exactly according as it's described in man documentation of lseek.
    
    if (my_open_files[myfd].fd != myfd) 
    {
        return -1;
    }
    if (whence==SEEK_CUR)  //if our "cursor" is located where we need, add the offset and return it later
    {
        my_open_files[myfd].file_position += offset;
    } 
    else if (whence==SEEK_END) 
    {
        my_open_files[myfd].file_position = inodes[myfd].size+offset; //no use of whence, advance cursor with offset
    } 
    else if (whence==SEEK_SET)
     {
        my_open_files[myfd].file_position = offset;
    }
    if (my_open_files[myfd].file_position<0) //in case the offset is a negative number - return the cursor (whence) to be zero.
    {
        my_open_files[myfd].file_position = 0;
    }
    //finally - return the updated file descriptor position
    return my_open_files[myfd].file_position;

}

myDIR *myopendir(const char *name)
{
    // input: full name path of a directory
    // reurn value: myDIR variable

    myDIR* ans = (myDIR*)malloc(sizeof(myDIR));
    char fullPath[100];
    strcpy(fullPath, name);
    char *token;
    char currPath[NAME_SIZE] = "";
    char prevPath[NAME_SIZE] = "";

    token = strtok(fullPath, "/"); 
    while(token != NULL ) 
    {          
        strcpy(prevPath, currPath);
        strcpy(currPath, token);
        token = strtok(NULL, "/");
    }

    for (int i = 0; i < sb->num_inodes; i++)
    {
        if (strcmp(inodes[i].name, currPath)==0) 
        {
            if (inodes[i].isDIR!=1)
            {
                // it's a file, so we return error
                return NULL;
            }
            ans->key = i;
            return ans;
        }
    }

    return NULL;
}

struct mydirent *myreaddir(myDIR *dirp)
{
    /* Uses the argument dirp to find the requested directory and return it's data as struct mydirent. */
    int key = dirp->key;

    if (inodes[key].isDIR != 1)
        return -1;
    
    return (struct mydirent*)dbs[inodes[key].next].data; 

}

int myclosedir(myDIR *dirp)
{
    inodes[dirp->key].next = -1; // dirp isn't saved at my_open_files table,
                                 // so it's enough to change the next inode to be -1 (free it).
}