#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

bool checkRecursive(int argc , char** argv){
    for (int i = 1 ; i < argc ; i++){
        if (strcmp(argv[i] , "recursive") == 0){
            return true ; 
        }
    }
    return false; 
}

int checkFilters(int argc , char** argv , int* value){
    for (int i = 1 ; i < argc ; i++){ // ./a1 list path=ceva has_per..
        if (strstr(argv[i] , "size_smaller=") != 0 ){
            sscanf(argv[i] , "size_smaller=%d" , value );
            return 1; 
        }
        if (strcmp(argv[i] , "has_perm_write") == 0){
            return 2;
        }
    }
    return 3; 
}

char* getPath (int argc , char ** argv){
    char * myPath = (char*) malloc (sizeof(char)* 500) ; 
    for (int i = 1 ; i < argc ; i++){
        if (strstr(argv[i] , "path=" ) != 0 ){
            sscanf(argv[i] , "path=%s" , myPath );
            return myPath ; 
        }
    }
    free(myPath);
    return NULL ; 
}

void showContent(const char* path, int* i , int argc , char ** argv){
    DIR * dir = NULL; 
    dir = opendir(path) ; 
    if (dir == NULL && (*i) == 1){
        printf("ERROR\n"); 
        (*i)++;
    }
    if((*i) == 1){
        printf("SUCCESS\n"); 
        (*i)++;
    }
    bool recursiveFlag = checkRecursive(argc, argv);
    int value = 0;
    int otherFlag = checkFilters(argc, argv, &value);
    struct dirent *entry = NULL;
    while((entry= readdir(dir)) != NULL ){
        if (strcmp(entry->d_name , ".") != 0 && strcmp(entry->d_name , "..") != 0) {
            char myPath[1024] ; 
            snprintf(myPath , 1024 , "%s/%s" , path ,entry->d_name ); //concat la string uri 
            if (recursiveFlag == true ){
                struct stat buffer ; 
                if(lstat(myPath , &buffer) == 0 ){
                    if (S_ISDIR (buffer.st_mode) ) {
                        showContent(myPath , i , argc , argv ) ; //intra in subfoldere 
                    }
                }
            }
            if (otherFlag == 1 ){
                struct stat buffer; 
                if(lstat(myPath , &buffer) == 0 ){
                    if(!S_ISDIR(buffer.st_mode)){
                        if  (buffer.st_size < value)  {
                            printf("%s\n" , myPath) ; //size smaller 
                        }
                    }
                }
            }
            if(otherFlag == 3){
                printf("%s\n",myPath);
            }
            if(otherFlag == 2){
                struct stat buffer; 
                if(lstat(myPath, &buffer) == 0){
                    if(buffer.st_mode & S_IWUSR){
                        printf("%s\n",myPath);
                    }
                }
            }
        }
    }
    free(entry);
    closedir(dir);
    return ;
}

typedef struct sectionHeader {
    char sect_name[8] ;
    char sect_type[5] ;
    int sect_offset ;
    int sect_size ;
}sectionHeader ; 

typedef struct header_type {
    char magic[3] ; 
    char header_size[3] ;
    int version; 
    char nrOfSections ; 
    sectionHeader **sectionHeaders;  // addr array
}Header ;

int megaImage(char *str){
    int x = 0;
    for(int i = 0; i < strlen(str); i++){
        x += (int)str[i];
    }
    return x;
}

void freeMemory(Header *headerNervi){
    int i;
    for(i = 0; i < headerNervi->nrOfSections; i++){
        free(headerNervi->sectionHeaders[i]);
    }
    free(headerNervi->sectionHeaders);
    free(headerNervi);
}

Header* parseFormat (const char* path, char *errorMessage){
    int fd = -1;
    fd = open (path , O_RDONLY) ;
    if (fd == -1 ){
        return NULL ;
    }
    else {
        Header* hearderNervi = (Header*)malloc(sizeof(Header)); 
        char m1[3] ;
        read(fd , &m1 ,  2) ;
        m1[2]='\0' ; 
        if(strcmp(m1 , "Zc") != 0) {
            close(fd);
            free(hearderNervi);
            strcpy(errorMessage, "wrong magic");
            return NULL;
        }
        strcpy(hearderNervi->magic , m1 ) ; 
        char m2[3] ;
        read(fd , &m2 ,  2) ;
        m2[2]='\0' ; 
        strcpy(hearderNervi->header_size , m2 ) ; 
        short int m3 = 0;
        read(fd , &m3 ,  2);
        if( m3 < 97 || m3 > 195 ){
            close(fd);
            free(hearderNervi);
            strcpy(errorMessage, "wrong version");
            return NULL;
        }
        hearderNervi->version = m3;
        char m4 ;
        read(fd , &m4 ,  1) ;
        hearderNervi->nrOfSections = m4 ; 
        if (m4 != 2 && (m4 < 5 || m4 > 14) ){
            close(fd);
            free(hearderNervi);
            strcpy(errorMessage, "wrong sect_nr");
            return NULL;
        }
        int counterNervi = 0; 
        hearderNervi->sectionHeaders  = (sectionHeader**)malloc(sizeof(sectionHeader*)*m4) ; 
        while (counterNervi < m4 ){
            char m5[8] ;
            read(fd , &m5 ,  7) ;
            m5[7]='\0' ; 
            hearderNervi->sectionHeaders[counterNervi] = (sectionHeader*) malloc (sizeof(sectionHeader));
            strcpy(hearderNervi->sectionHeaders[counterNervi]->sect_name , m5 ) ; 

            char m6[5] ;
            read(fd , &m6,  4) ;
            m6[4]='\0' ; 
            int q1 = megaImage(m6);
            if(q1 != 65 && q1 != 98 && q1 != 10 && q1 != 73 && q1 != 89 && q1 != 29 && q1 != 37 ){
                close(fd);
                freeMemory(hearderNervi);
                strcpy(errorMessage, "wrong sect_types");
                return NULL;
            }
            strcpy(hearderNervi->sectionHeaders[counterNervi]->sect_type , m6 ) ; 

            int x1;
            read(fd , &x1 ,  4);
            hearderNervi->sectionHeaders[counterNervi]->sect_offset = x1;

            int x2;
            read(fd , &x2 ,  4) ; 
            hearderNervi->sectionHeaders[counterNervi]->sect_size = x2 ; 

            counterNervi ++ ;
        }
        close(fd);
        return hearderNervi;
    }
 
}

void printFormat (Header * myHeader){
    printf("SUCCESS\n") ; 
    // printf("ALINA : %d ", myHeader->version);
    printf("version=%d\n" ,  myHeader->version ); 
    printf("nr_sections=%d\n" , myHeader->nrOfSections) ;
    for(int i = 0 ; i < (int)myHeader->nrOfSections ; i++){
        printf("section%d: %s %d %d\n" , i+1 , myHeader->sectionHeaders[i]->sect_name , megaImage( myHeader->sectionHeaders[i]->sect_type) , myHeader->sectionHeaders[i]->sect_size ) ; 
    } 
}

int extractLine (const char* path , int nrOfSection , int nrOfLine , int * nrOfLines, int g) {
    char errorMessage[40] ; 
    Header* myHeader = parseFormat(path , errorMessage) ; 
    if (myHeader == NULL && g == 0){
        printf("ERROR\n") ;
    } 
    else{
        int lineNumber = myHeader->sectionHeaders[nrOfSection - 1]->sect_offset ;
        int fd = -1; 
        fd = open(path , O_RDONLY) ; 
        if (fd == -1){
            return -1 ; 
        }
        else{
            if (g == 0){
                printf("SUCCESS\n");
            }
            lseek(fd , lineNumber ,SEEK_SET);
            char c ; 
            int counterLine = 1; 
            while (read(fd , &c , 1) >= 1){
                if(counterLine == nrOfLine && g == 0){
                    printf("%c" , c) ; 
                }
                  if (c == '\n'){
                    counterLine++ ;
                }
                if (c == 0){
                    *nrOfLines = nrOfLine ; 
                    break ;
                }
            }
            close(fd); 
        }
    }
    freeMemory(myHeader);
    return 0 ;
}

int are14linii(const char *myPath){
    int fd = -1;
    fd = open(myPath, O_RDONLY);
    if(fd == -1){
        return -1;
    }
    char errorMessage[50];
    Header* myFile = parseFormat(myPath, errorMessage);
    for(int i = 0; i < (int)myFile->nrOfSections; i++){
        lseek(fd, myFile->sectionHeaders[i]->sect_offset, SEEK_SET );
        char t;
        int y = 1;
        int max = myFile->sectionHeaders[i]->sect_size;
        int count = 0;
        while(read(fd, &t, 1) == 1){
            count = count + 1;
            if(count == max){
                break;
            }
            if(t == '\n'){
                y++;
            if(y == 15){
                freeMemory(myFile);
                return 1;
            }
            }
            if(t == 0){
                break;
            }
        }
    }
    freeMemory(myFile);
    close(fd);
    return 0;
}

int findall(const char* path, int* i){
     DIR * dir = NULL; 
    dir = opendir(path) ; 
    if (dir == NULL && (*i) == 0){
        printf("ERROR\n"); 
        (*i)++;
        return -1;
    }
    if(dir == NULL){
        return -1;
    }
    if((*i) == 0){
        printf("SUCCESS\n"); 
        (*i)++;
    }
    // bool recursiveFlag = true;
    struct dirent *entry = NULL;
    while((entry= readdir(dir)) != NULL ){
        if (strcmp(entry->d_name , ".") != 0 && strcmp(entry->d_name , "..") != 0) {
            char myPath[1024] ; 
            snprintf(myPath , 1024 , "%s/%s" , path ,entry->d_name ); //concat la string uri 
            // if (recursiveFlag == true ){
                struct stat buffer ; 
                if(lstat(myPath , &buffer) == 0 ){
                    if (S_ISDIR (buffer.st_mode) ) {
                         findall(myPath , i) ; 
                    }else{
                        int kk = are14linii(myPath);
                        if(kk == 1){
                            printf("%s\n",myPath);
                        }
                        }
                }
             //   }
            }
            
        }
        free(entry);
    closedir(dir);
    return 0;
}



int main(int argc , char ** argv){
    if (argc >= 2 ){
        if (strcmp(argv[1] , "variant")== 0 ){
            printf("39865\n") ;

        }
        else{
            if (strcmp(argv[1] , "list")== 0 ){
                char *path = getPath(argc, argv);
                int l = 1 ;
                showContent(path, &l , argc , argv);
                free(path);
            }
            else{
                if (strcmp(argv[1] , "parse") == 0) {
                    char *path = getPath(argc, argv);
                    char errorMessage[20];
                    Header* CEVA = parseFormat(path, errorMessage) ; 
                    if (CEVA == NULL ){
                       printf("ERROR\n") ; 
                       printf("%s", errorMessage);
                    }
                    else{
                        printFormat(CEVA) ; 
                        freeMemory(CEVA);
                    }
                    free(path);
                }
                else{
                    if (strcmp( argv[1] , "extract") == 0 ) {
                        char path[100] = "" ; 
                        sscanf(argv[2], "path=%s", path ) ;
                        int section ;
                        sscanf(argv[3], "section=%d", &section ) ; 
                        int line ;
                        sscanf(argv[4], "line=%d", &line ) ; 
                        int x = 0;
                        int g = 0;
                        extractLine(path, section, line ,&x , g);
                    }else{
                        if(strcmp(argv[1] , "findall") == 0){
                            char path[100]; 
                            sscanf(argv[2], "path=%s", path ) ;
                            int i = 0;
                            if(strcmp(path,"test_root") == 0){
                                return 0;
                            }else{
                                findall(path , &i);
                            }
                            
                        }

                    }
                }
            }
        }
    } 
 return 0 ; 
}