#include "mystdio.h"
#include <math.h>


myFILE* myfopen(const char *pathname, const char *mode)
{
    if(mode == NULL) // error
        return NULL;

    if(strcmp(mode,"r")==0 || strcmp(mode,"r+")==0 || strcmp(mode,"w")==0 || strcmp(mode,"a")==0)
    {
        // TODO
        int fd = myopen(pathname, 0); //0 is by default, cause we are not referring to flags variable in myopen function
        if (fd==-1) //failure
            return NULL;
        
        myFILE* f_ans = (myFILE*)malloc(sizeof(myFILE));
        
        // Attach the mode and the fd to our return file
        strcpy(f_ans->mode,mode);
        f_ans->file_descriptor=fd;

        if (strcmp(mode, "a")==0)  // set the cursor pointer to the end of the file
            f_ans->cursor = inodes[fd].size;
        else       
            f_ans->cursor = 0; //set the cursor pointer to the end of the file

        f_ans->size = inodes[fd].size;
        f_ans->data = (char*)malloc(f_ans->size);

        if (strcmp(mode, "w")==0) 
        { // wipe the content of the file 
            for (int i = 0; i < f_ans->size; i++)
            {
                f_ans->data[i] = ' ';
            }
            f_ans->data[f_ans->size] = '\0';
        } 
        else if (strcmp(mode, "r+")==0) //read and write (r+)
        {
            for (int i = 0; i < f_ans->size; i++)
            {
                f_ans->data[i] = readbyte(fd, i);
            }
        }
        else //read only (r)
        {
            for (int i = 0; i < f_ans->size; i++)
            {
                int byte = readbyte(fd, i); //we are only reading from fd, nothing to do with it
            }
        }
        myclose(fd);
        return f_ans;
    }
}

int myfclose(myFILE *stream)
{
    free(stream->data);
    free(stream);
    return 0; //success
}

size_t myfread(void * ptr, size_t size, size_t nmemb, myFILE * stream)
{
    // @param ptr - our cursor (from which position in the file are we reading)
    // @param size and @param nmemb - when multiplying their values we are getting how many bytes are we reading 
    // @param stream - our myFILE

    int count = size*nmemb;
    char* buffer = malloc(count+1);
    int stream_cursor = stream->cursor;
    for (int t = 0; t < count; t++)
    {
        if ((stream->cursor+t) > stream->size) 
            break;
        buffer[t] = stream->data[stream->cursor+t];
        stream_cursor++;
    }
    stream->cursor = stream_cursor;
    buffer[count] = '\0';
    strncpy(ptr, buffer, count);
    free(buffer);

    return stream->cursor;
}

size_t myfwrite(const void * ptr, size_t size, size_t nmemb, myFILE * stream)
{
    if(strcmp(stream->mode, "r")==0) //in case we got read only, return error
        return -1;

    int count = size*nmemb;
    char* buffer = (char*)ptr;
    if ((stream->cursor+count) > stream->size) 
    {
        char* tempbuffer = malloc(stream->size+1);
        for (size_t i = 0; i < stream->size; i++)
        {
            tempbuffer[i] = stream->data[i];
        }
        free(stream->data);
        stream->data = malloc(stream->cursor+count);
        for (size_t i = 0; i < stream->size; i++)
        {
            stream->data[i] = tempbuffer[i];
        }
        free(tempbuffer);
    }
    int stream_cursor = stream->cursor;
    for (size_t i = 0; i < count; i++)
    {
        stream->data[stream->cursor+i] = buffer[i];
        stream_cursor++;
    }
    stream->cursor = stream_cursor;
    return stream->cursor;
}

int myfseek(myFILE *stream, long offset, int whence)
{
    // This function is used to move the file pointer, @return the new location of the pointer.
    // implemenation of SEEK_CUR, SEEK_END and SEEK_SET are exactly according as it's described in man documentation of fseek.
    
    if (whence==SEEK_CUR) //if our "cursor" is located where we need, add the offset and return it later
        stream->cursor += offset;

    else if (whence==SEEK_END) //no use of whence, advance cursor with offset
        stream->cursor = stream->size+offset;

    else if (whence==SEEK_SET) 
        stream->cursor = offset;

    if (stream->cursor<0) //in case the new cursor is a negative number - intialize the cursor to be zero.
        stream->cursor = 0;

    return stream->cursor;
}

int myfscanf(myFILE * stream, const char * format, ...)
{
    /**
     * @brief This function uses a va list format to read data from the file stream
     * @return 1 for success, 0 for fail
     * 
     */

    if(stream == NULL)
        return 0;

    va_list arguments;
    va_start(arguments, format); //all values after format will be stored in arguments

    int i=0, j=0; /**
     * @brief i,j are variables for format index and current length of tempbuffer string respectively.
     * @param format - can be %d, %c, %f and other any other character not including any '%'.
     */

    char* tempbuffer; // used to store numbers

    while (format && format[i])
    {
        if (format[i] == '%') 
        {
            switch(format[i+1])
            {
                case 'd':
                        *(int *)va_arg( arguments, int* ) = strtol(&stream->data[j], &tempbuffer, 10); //the base is 10
                        j+=strlen(tempbuffer) - stream->size;
                        break;
                case 'c':
                        *(char *)va_arg( arguments, char* ) = stream->data[j];
                        break;
                case 'f':
                        *(double *)va_arg( arguments, double* ) = strtol(&stream->data[j], &tempbuffer, 10); //the base is 10
                        j+=strlen(tempbuffer) - stream->size;
                        break;
                
                default:

                        break;
            }
            i++; //very important because we want to advance i by 2 (one time here and another time after this switch case)
        }
        i++;
        j++;
    }

    va_end(arguments);

    return 1; 

}

int myfprintf(myFILE * stream, const char * format, ...)
{
    /**
     * @brief This function uses a va list format to write data into the file stream
     * @return the number of bytes written to stream
     * 
     */

    if(stream == NULL)
        return -1;

    va_list arguments;
    va_start ( arguments, format );
    char *buffer = (char*)malloc(strlen(format)+1500);
    char *tempbuffer = (char*)malloc(250);
    int j = 0; // buffer's index
    for (size_t i = 0; i < strlen(format); i++)
    {
        int currvar1; char currvar2; double currvar3;
        memset(tempbuffer, 0, 250);
        if (format[i]=='%')
        {
            switch(format[i+1])
            {
                case 'd':
                            currvar1 = va_arg ( arguments, int );
                            sprintf(tempbuffer,"%d",currvar1);
                            break;
                case 'c':
                            currvar2 = va_arg ( arguments, int );
                            tempbuffer[0] = currvar2;
                            sprintf(tempbuffer,"%c",currvar2);

                        break;
                case 'f':
                            currvar3 = va_arg ( arguments, double );
                            sprintf(tempbuffer,"%f",currvar3);
                            break;
                default:
                        break;
            }
            i++;
        } 
        else
            tempbuffer[0] = format[i];
        for (size_t k = 0; k < strlen(tempbuffer); k++)
        {
            buffer[j] = tempbuffer[k];
            j++;
        }
    }
    buffer[j] = '\0';
    myfwrite(buffer, strlen(buffer), 1, stream); //the value of cursor is 1
    va_end(arguments);
    return strlen(buffer);
}