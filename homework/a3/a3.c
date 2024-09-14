#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdbool.h>

#define resp "RESP_PIPE_39865" 
#define req "REQ_PIPE_39865"

int memorySize = 4296223  ;

int variant = 39865 ;  
char success[250] = "SUCCESS";
char error[250] ="ERROR" ;
int memoryFD = -1;
unsigned int offset=0; 
unsigned int value =0; 




void readMyString (int fd , char* myString){
    char ch ; 
    int size = 0;
    while(read(fd , &ch , 1)==1){
        if (ch == '!'){
            break;
        }
        myString[size++] = ch ; 
        
        //size++ ; 
    }
    myString[size] = '\0'; 
   
}

void writeMyString(int fd, char* myString) {
    for(int i=0;i<strlen(myString);i++){
        write(fd,&myString[i],sizeof(myString[i]));
    }
    write(fd, "!" , sizeof(char));
   
}


int main(int argc , char **argv){
    int finalSize = sizeof(char)*memorySize ;
    volatile char* sharedMemory = NULL;
    volatile char* fileData = NULL;
    unsigned int offset1 = 0 ; 
    unsigned int numbOfBytes = 0 ; 
    int fileSize = -1;
    int mapFD = -1 ;

    unsigned int sectionNo = -1;
    unsigned int offset2 = -1;
    unsigned noOfBytes1 = -1;
    unsigned int logicalOffset, noOfBytes;

    if (mkfifo(resp,0664)!= 0){
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe");
        return -1;
    }

    int fd2 = open(req , O_RDONLY);
    if(fd2 == -1){
        printf("ERROR\ncannot create the response pipe | cannot open the request pipe");
        return -1;

    } 
    int fd1 = open(resp, O_WRONLY);
    if(fd1 == -1){
        printf("ERROR\ncannot create the response pipe | cannot open the request pipe");
        return -1;

    } 

    char message[] = "START" ;
    writeMyString(fd1,message);
   
    printf("SUCCESS");
    
    
    //requests loop
    char buffer[250]="" ;  
    for(;;){
        memset(buffer, 0, sizeof(buffer));
        readMyString(fd2,buffer);
        printf("%s", buffer);
       
            if( strcmp(buffer,"EXIT")==0){
                close(fd1); 
                close(fd2); 
                unlink(resp);
            }
            //request
            if(strcmp(buffer,"ECHO")==0){
                writeMyString(fd1,"ECHO");
                unsigned int var = 39865;
                write(fd1,&var, sizeof(unsigned int));
                writeMyString(fd1, "VARIANT");
                 
            }
            else if (strcmp(buffer,"CREATE_SHM")==0){
                    unsigned int var;
                    read(fd2, &var, sizeof(unsigned int)); // read number 
                    writeMyString(fd1,"CREATE_SHM");
                    
                    memoryFD = shm_open("/SAfQpX",O_RDWR|O_CREAT,0664);
                    if (memoryFD== -1){
                        perror("Could not aquire shm");
                        return -1;
                    }
                     
                    ftruncate(memoryFD,finalSize) ;
                    sharedMemory = (volatile char*)mmap(0,finalSize,PROT_READ|PROT_WRITE,MAP_SHARED,memoryFD,0) ;
                    if (sharedMemory == (void*)-1){
                        writeMyString(fd1,error);
                    }
                    else {
                        writeMyString(fd1 ,success);
                    }
                } else if( strcmp(buffer,"WRITE_TO_SHM")==0){
                        writeMyString(fd1,"WRITE_TO_SHM");
                        
                        read(fd2,&offset ,sizeof(unsigned int));
                        read(fd2,&value,sizeof(unsigned int));
                 
                        if(offset+sizeof(unsigned int)<=memorySize){
                                unsigned int *location = (unsigned int *)(sharedMemory+offset);
                                *location = value;
                            
                                writeMyString(fd1,success);
                            }
                            else{
                                writeMyString(fd1,error);
                            }
                        
                        }
                        else{

                            if(strcmp(buffer,"MAP_FILE")==0){
                                writeMyString(fd1,"MAP_FILE");
                                char fileName[250] ="";
                                readMyString(fd2 , fileName);
                                mapFD = open(fileName,O_RDONLY);
                                if (mapFD== -1){
                                    writeMyString(fd1,error);
                                    break;
                                }
                                
                                fileSize = lseek(mapFD , 0 , SEEK_END);
                                lseek(mapFD,0,SEEK_SET);
                                fileData = (volatile char*)mmap(NULL,fileSize,PROT_READ,MAP_SHARED,mapFD,0) ; // memoria
                                if ( fileData == (void*)-1){
                                    writeMyString(fd1,error);
                                }
                                else {
                                    writeMyString(fd1 ,success);
                                }

                            }
                            else{
                                if(strcmp(buffer,"READ_FROM_FILE_OFFSET")==0){
                                    writeMyString(fd1, "READ_FROM_FILE_OFFSET");
                                    read(fd2,&offset1 ,sizeof(unsigned int));
                                    read(fd2,&numbOfBytes,sizeof(unsigned int));
                                    char dataMem[numbOfBytes] ; 
                                    int j = 0 ; 
                                    int finalSize2 = offset1+numbOfBytes;
                                    for(int i = offset1 ; i < finalSize2 ; i++){
                                        dataMem[j]= fileData[i];
                                        j++ ; 
                                    }
                                    if ( dataMem[0]> 0){
                                    }
                                    for(int i = 0 ; i < numbOfBytes;i++){
                                        sharedMemory[i] = dataMem[i];
                                        printf("%c",dataMem[i]);
                                    }
                                    if(finalSize2 > fileSize || fileData == (void*)-1 || sharedMemory == (void*)-1){
                                        writeMyString(fd1,error);
                                    }
                                    else{
                                        writeMyString(fd1,success);
                                    }
                                }else{
                                    if (strcmp(buffer ,"READ_FROM_FILE_SECTION")==0){
                                        writeMyString(fd1 , "READ_FROM_FILE_SECTION");

                                        read(fd2, &sectionNo, sizeof(unsigned int));
                                        read(fd2, &offset2, sizeof(unsigned int));
                                        read(fd2, &noOfBytes1, sizeof(unsigned int));

                                        if (fileData != NULL) {
                                            unsigned int nrSections = fileData[6];
                                            
                                            if (sectionNo == 0 || sectionNo > nrSections) {
                                                writeMyString(fd1, error);
                                                continue ;
                                            }
                                            
                                            unsigned int sectionHeaderOffset = 7 + (sectionNo-1) * 19;
                                            unsigned int sectionOffset = *(unsigned int *)(fileData + sectionHeaderOffset + 11);
                                            unsigned int sectionSize = *(unsigned int *)(fileData + sectionHeaderOffset + 15);
                                          
                                            if (offset2 + noOfBytes1 > sectionSize) {
                                                writeMyString(fd1, error);
                                            }
                                            
                                            unsigned int readOffset = sectionOffset + offset2;
                                            memcpy((void*)sharedMemory, (void*)(fileData + readOffset), noOfBytes1); // good approach why not 
                                            writeMyString(fd1, success);

                                        } 
                                        else {
                                            writeMyString(fd1, error);
                                        }
                                    } else 
                                        if (strcmp(buffer, "READ_FROM_LOGICAL_SPACE_OFFSET") == 0) {
                                            writeMyString(fd1, "READ_FROM_LOGICAL_SPACE_OFFSET");
                                            read(fd2, &logicalOffset, sizeof(unsigned int));
                                            read(fd2, &noOfBytes, sizeof(unsigned int));

                                            if (fileData != NULL) {
                                                unsigned int nrSections = fileData[6];
                                                unsigned int logicalAddress = 0;
                                                char found = 0;
                                                for (int i = 0; i < nrSections; i++) {
                                                    unsigned int sectionHeaderOffset = 7 + i * 19;
                                                    unsigned int sectionOffset = *(unsigned int *)(fileData + sectionHeaderOffset + 11);
                                                    unsigned int sectionSize = *(unsigned int *)(fileData + sectionHeaderOffset + 15);

                                                    if (logicalOffset >= logicalAddress && logicalOffset < logicalAddress + sectionSize) {
                                                        unsigned int offsetInSection = logicalOffset - logicalAddress;
                                                        if (offsetInSection + noOfBytes > sectionSize) {
                                                            writeMyString(fd1, error);
                                                            found = 1;
                                                            break;
                                                        }

                                                        unsigned int readOffset = sectionOffset + offsetInSection;
                                                        memcpy((void*)sharedMemory, (void*)(fileData + readOffset), noOfBytes);
                                                        writeMyString(fd1, success);
                                                        found = 1;
                                                        break;
                                                    }
                                                    logicalAddress += (sectionSize + 1023) & ~1023;
                                                }
                                                if (!found) {
                                                    writeMyString(fd1, error);
                                                }
                                            } else {
                                                writeMyString(fd1, error);
                                                break ;
                                            }
                                    }
                                    else {
                                        break ;
                                    }
                                }
                                                                    
                                                                    
                            }
                        }
                                                                
                                                            
                                                            
    }                 
                                                    
    
    return 0 ; 
    
}
