Subject: Unix File System (UFS)

recommended information: https://www.youtube.com/watch?v=n2AAhiujAqs&ab_channel=drdelhart

*******************************
******** PLEASE NOTICE ********
We implemented the functions as we saw right.
Tests are particularly difficult to write from various reasons such as:

- The lecturers Nezer and Mordechai didn't give us any output example of how the ufs should be displayed.
- In practical lessons the explanation to this assignment was simply by drawing the structures.
- The second part of the assignment (mylibc) wasn't elaborated at all.

We knew from the start missing test cases will affect this assignment's grade, so we consulted with several
students that have finished most of the assignment pretty quickly and asked them of the functions's behaviours.
With enough information we managed to implement all of the functions in both parts and create a valid 
makefile that compiles.

*******************************

Explanations for the implemented functions:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Part 1: myfs

1. void mymkfs(int s) - in this function we build our file system (fs).
                        This function takes only one argument which represents the size of our fs.
                        After the procedures of this function, we are calling to "create_file_fs" function
                        with a given path that writes into the fs structures, after they are initialized.

2. int mymount(const char *source, const char *target, ...) - in this function we load our fs.
    Originally we implemented "create_file_fs" inside mymount function, but later we decided 
    to seperate them into a "helper" function - mymkfs, that calls "create_file_fs" at the end of the initialization procedure.

    In short, mymount checks only source and target agruments and creates our fs if necessary (if an error wasn't returned)
    
3. int myopen(const char *pathname, int flags) - first this function finds the available file descriptor (search from index 3)
    and then initialize fd in the available space in open_files table.
    The pathname has to be the same in each inode.

4. int allocate_file( int size ,const char* name) - this function allocate inode and enough blocks for a new file in our fs.
    Simply finding an empty inode, and an empty block, and then capture space in disk block with the relevant name and size.

5. int myclose(int myfd) - closes the fd by changing the given fd and position to -1.

6. char read_byte(int fd, void* start, int size_to_read) - reading the byte from the given argument "start".

7. ssize_t myread(int myfd, void *buf, size_t count) - reads "count" bytes from the "buffer" in "myfd".

8. ssize_t mywrite(int myfd, const void *buf, size_t count) - writes "count" bytes to the "buffer" of "myfd".

9. void write_byte(int fd, void* start, char c) - writes the byte to the given argument "start" in "fd".

10. off_t mylseek(int myfd, off_t offset, int whence) - this function's behaviour is the same as lseek.

11. myDIR *myopendir(const char *name) - by using strtok function, we iterate over the path seperated by "/".
    When reaching the rightest, we initialize the key of the directory we open to be the same
    as the inode we find, and return it.
    inode[i] can be either a file (0) or a directory (1).

12. struct mydirent *myreaddir(myDIR *dirp) - Uses the argument dirp to find the requested directory 
                                              and return it's data as struct mydirent.

13. int myclosedir(myDIR *dirp) - closes directory.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Part 2: mylibc (overriding stdio library functions)

1. myFILE* myfopen(const char *pathname, const char *mode) - this function checks the mode argument.
    According to the mode we are initializing a myFILE* variable: it's data, cursor, fd, size of inodes.

2. int myfclose(myFILE *stream) - free space of stream's info.

3. size_t myfread(void * ptr, size_t size, size_t nmemb, myFILE * stream) - reading (size*nmemb) bytes from stream's data,
    and update the cursor.

4. size_t myfwrite(const void * ptr, size_t size, size_t nmemb, myFILE * stream) - in case of the mode is "r" an error is returned.
    This function's behaviour similary to myfread function.

5. int myfseek(myFILE *stream, long offset, int whence) - this function's behaviour is the same as fseek.

6. int myfscanf(myFILE * stream, const char * format, ...) - this function's behaviour simulates fscanf behaviour.
    Supports only %f, %c and %d formats, scanning from our given stream.

7. int myfprintf(myFILE * stream, const char * format, ...) - this function's behaviour simulates fprintf behaviour.
    Supports only %f, %c and %d formats, writing to our stream using sprintf function.
    When the function finishes its job, we call myfwrite function in order to move the cursor
    to the right place before writing to the stream's data.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Submitted by: 311382360 Eran Levy _ 315524389 Or Mendel