
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX 512

void display_prompt(){
        printf("sshell$ ");
}

char ** read_line(){
        char **command;
        char *buffer;
        size_t bufsize = 512;
        size_t characters;
        int curr_arg = 0;
        int num_spaces = 0;

        buffer = (char *)malloc(bufsize * sizeof(char));
        if(buffer == NULL)
        {
                perror("Buffer fail");
                exit(1);
        }


        getline(&buffer, &bufsize, stdin);

        printf("INPUT: %s",buffer);

        for(int i=0; i < strlen(buffer)-1; i++){
                if(buffer[i] == ' '){
                        num_spaces++;
                }
        }

        char* token = strtok(buffer, " ");

        command = (char**)malloc(16 * sizeof(char*));

        for(int i=0; i < 16; i++)
            command[i] = (char *)malloc (512 * sizeof (char));
        while (token != NULL) {
                if(curr_arg == num_spaces){
                        for(int i = 0; i < strlen(token)-1; i++){
                                command[curr_arg][i] = token[i];
                        }
                }else{
                        command[curr_arg] = token;
                }

                token = strtok(NULL, " ");

                curr_arg++;
        }

        for(int i=curr_arg; i < 16; i++){
                command[i] = NULL;
        }

        return command;


}



int main(int argc, char *argv[]){

        int status;

        while(1){

                display_prompt();
                char **command;

                command = read_line();

                /*for(int i = 0; command[i]!= NULL; i++){
                        printf("%s", command[i]);
                        printf(" %ld", strlen(command[i]));
                }*/

                if(fork() != 0) {
                        waitpid(-1, &status, 0);
                }else {
                        execvp(command[0], command);
                        perror("execvp");
                        exit(1);
                }

        }

        return 0;
}







