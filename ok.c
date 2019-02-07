#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define DICTIONARY_PATH "./dictionaries/webster"
#define WORD_LENGTH 16

int readWord(int fd, int lineNumber, int* currentLine, char* buffer);
int process(char* inputString);
int search(int fd, char* word);

int main(int argc, char** argv){
    // Check input for errors, and process it.
    if(argc >= 2){
        if(process(argv[1]) < 0) return -1;
    }else{
        write(2, "'./ok'  Requires one argument\n",31);
        return -1;
    }
    char buff[WORD_LENGTH+1] = {'\0'};
    // Attempt to open the file located at DICTIONARY_PATH
    int fd = open(DICTIONARY_PATH,O_RDONLY);
    if(fd < 0) {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n",2);
        printf("%s\n",strerror(errno));
        return -1;
    }
    int res, end = lseek(fd, 0, SEEK_END), high = end/16, nextLine =(high+(high%2))/2, low = 0, currentLine = 0, range;
    lseek(fd,-end,SEEK_CUR);
    do{
        // If the current line is lower than the input string then low = currentline
        if(readWord(fd,nextLine,&currentLine, buff) < 0){
            printf("Failed to read line %i\n",nextLine);
            return -1;
        }
        res = strcmp(buff, argv[1]);
        if(res == 0){
            printf("yes\n");
            return 0;
        }
        if(res < 0){ // most recent line is less than the argument which was passed in, go further down.
            /*printf("low %s. %s.\n",buff, argv[1]);
            printf("High: %i, Low: %i\n",high, low);*/
            low = currentLine;
            range = high-low;
            nextLine = low+(range+(range%2==0))/2;
        }else if(res > 0){ // Most recent line was greater than the argument, go up in the file.
            /*printf("high %s. %s.\n",buff,argv[1]);
            printf("High: %i, Low: %i\n",high, low);*/
            high = currentLine;
            range = high-low;
            nextLine = high-(range+(range%2==0))/2;
        }
    }while(high - low > 1);
    printf("no\n");
    return 0;
}

int process(char* inputString){
    // Remove white space and truncate at WORD_LENGTH
    int i, spaces, res = 0;
    for(i = 0, spaces = 0; inputString[i+spaces] != '\0' && i+spaces< WORD_LENGTH; i++){
        while(isspace(inputString[i+spaces])) spaces++;
        if((res = isalpha(inputString[i+spaces])) || ispunct(inputString[i+spaces]) || isdigit(inputString[i+spaces])){
            if(res){
                inputString[i] = tolower(inputString[i+spaces]);
            }else{
                inputString[i] = inputString[i+spaces]; 
            }
        }else if(inputString[i+spaces] == '\0'){
            break;
        }else{
            write(2, "skipping Bad character\n",24);
            printf("skipping bad character ASCII: %i \n",inputString[i+spaces]);
            spaces++;
        }
    }
    if(i == 0){
        write(2, "Empty input string\n",20);
        printf("Empty input string\n");
        return -1;
    }
    inputString[i] = '\0';
    return i+1;

}

int readWord(int fd, int lineNumber, int* currentLine, char* buffer){
    int res;
	assert(fd != -1);
	res = lseek(fd,(lineNumber-(*currentLine)-1)*WORD_LENGTH,SEEK_CUR);
    *currentLine = lineNumber;
	if(read(fd, buffer, WORD_LENGTH) == WORD_LENGTH){
        int i = 0;
        while((isalpha(buffer[i]) || ispunct(buffer[i]) ) && buffer[i] != '\0') i++;
        buffer[i] = '\0';
        return res;
    }
    write(2, strerror(errno), strlen(strerror(errno)+1));
    write(2, "\n",2);
    return -1;

}


int search(int fd, char* word){
    return 0;
}
