#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX 512

void display_prompt() {
    printf("sshell$ ");
}

bool error_management(char ** command) {        // function to handle error cases
    char * spec_list;
    int char_num = 0;
    spec_list = (char * ) malloc(20 * sizeof(char * ));
    for (int i = 0; i < 20; i++) {
        spec_list[i] = 'x';
    }

    for (int i = 0; command[i] != NULL; i++) {
        if (strcmp(command[i], "|") == 0) {
            spec_list[char_num] = '|';
            char_num++;
            if (i == 0 || command[i + 1] == NULL) {
                fprintf(stderr, "Error: missing command\n");
                return true;
            }
        }
        if (strcmp(command[i], "&") == 0) {
            spec_list[char_num] = '&';
            char_num++;
            if (i == 0) {
                fprintf(stderr, "Error: missing command\n");
                return true;
            }
        }
        if (strcmp(command[i], "<") == 0) {     // error case for no input file
            spec_list[char_num] = '<';
            char_num++;
            if (command[i + 1] == NULL) {
                fprintf(stderr, "Error: no input file\n");
                return true;
            }
        }
        if (strcmp(command[i], ">") == 0) {     // error case for no output file
            spec_list[char_num] = '>';
            char_num++;
            if (command[i + 1] == NULL) {
                fprintf(stderr, "Error: no output file\n");
                return true;
            }
        }
        for (int i = 0; spec_list[i] != 'x'; i++) {     // error case for wrong input/output redirection
            if (i != 0 && spec_list[i] == '<') {
                fprintf(stderr, "Error: mislocated input redirection\n");
                return true;
            } else if (spec_list[i] == '>') {
                if (spec_list[i + 1] != 'x') {
                    fprintf(stderr, "Error: mislocated output redirection\n");
                    return true;
                }
            }
        }
    }
    if (command[16] != NULL) {      // error case for too many args
        fprintf(stderr, "Error: too many process arguments\n");
        return true;
    } else {
        return false;
    }
}

char ** read_line() {       // function to read and parse the command line
    char ** command;
    char * buffer;
    size_t bufsize = 512;
    int curr_arg = 0;

    buffer = (char * ) malloc(bufsize * sizeof(char));      // allocating space for command line
    if (buffer == NULL) {
        perror("Buffer fail");
        exit(1);
    }

    getline( & buffer, & bufsize, stdin);       // using getline to read in the whole line

    if (!isatty(STDIN_FILENO)) {
        printf("%s", buffer);
        fflush(stdout);
    }

    buffer[strcspn(buffer, "\n")] = 0;      // remove new line character at end of buffer

    char * argument = strtok(buffer, " ");     // using strtok to get the first argument

    command = (char ** ) malloc(20 * sizeof(char * ));      // allocating space for the command
    for (int i = 0; i < 20; i++)
        command[i] = (char * ) malloc(512 * sizeof(char));

    while (argument != NULL) {     // while loop to read in all of the other arguments to command
        command[curr_arg] = argument;

        argument = strtok(NULL, " ");
        curr_arg++;
    }

    for (int i = curr_arg; i < 20; i++) {
        command[i] = NULL;
    }

    return command;
}

int main(int argc, char * argv[]) {
    int status;
    while (1) {
        display_prompt();

        char ** command;
        bool error;
        command = read_line();  // call read_line to get the parsed command

        error = error_management(command);      // determine if there are errors in the command line

        if (strcmp(command[0], "exit") == 0) {      // handling exit command
            fprintf(stderr, "Bye...\n");
            exit(1);
        }

        if (strcmp(command[0], "cd") == 0) {        // handling cd command
            char get_directory[MAX];
            getcwd(get_directory, sizeof(get_directory));
            char * directory = strcat(get_directory, "/");
            char * destination = strcat(directory, command[1]);
            chdir(destination);

            int status = chdir(destination);

            if (status != 0) {
                fprintf(stderr, "Error: no such directory\n");
            }

            continue;
        }

        if (error == false) {
            if (fork() != 0) {      // parent process
                waitpid(-1, & status, 0);
            } else {        // child process
                int fd0 = 0;
                int fd1 = 0;
                int in = 0;
                int out = 0;

                char input[MAX], output[MAX];
                for (int i = 0; command[i] != NULL; i++) {
                    if (strcmp(command[i], "<") == 0) {     // input redirection
                        command[i] = NULL;      // set the < character to null
                        strcpy(input, command[i + 1]);
                        in = 1;
                    } else if (strcmp(command[i], ">") == 0) {      // output redirection
                        command[i] = NULL;      // set the > character to null
                        strcpy(output, command[i + 1]);
                        out = 1;
                    }
                }

                if (in) {
                    fd0 = open(input, O_RDONLY);
                    dup2(fd0, STDIN_FILENO);        // replaces stdin with input file
                    close(fd0);
                }
                if (out) {
                    fd1 = creat(output, 0644);
                    dup2(fd1, STDOUT_FILENO);       // replaces stdout with output file
                    close(fd1);
                }

                // implementing piping
                char ** cmd1;       // 1st command for piping
                char ** cmd2;       // 2nd command for piping
                char ** cmd3;       // 3rd command for piping
                int num_pipes = 0;
                int cmd1_index = 0;
                int cmd2_index = 0;
                int cmd3_index = 0;

                cmd1 = (char ** ) malloc(20 * sizeof(char * ));     // allocating space for piping commands
                for (int i = 0; i < 20; i++)
                    cmd1[i] = (char * ) malloc(512 * sizeof(char));
                cmd2 = (char ** ) malloc(20 * sizeof(char * ));
                for (int i = 0; i < 20; i++)
                    cmd2[i] = (char * ) malloc(512 * sizeof(char));
                cmd3 = (char ** ) malloc(20 * sizeof(char * ));
                for (int i = 0; i < 20; i++)
                    cmd3[i] = (char * ) malloc(512 * sizeof(char));

                for (int i = 0; command[i] != NULL; i++) {      // separate the commands between the pipes
                    if (strcmp(command[i], "|") == 0) {
                        num_pipes++;
                    } else if (num_pipes == 0) {
                        strcpy(cmd1[cmd1_index], command[i]);
                        cmd1_index++;
                    } else if (num_pipes == 1) {
                        strcpy(cmd2[cmd2_index], command[i]);
                        cmd2_index++;
                    } else if (num_pipes == 2) {
                        strcpy(cmd3[cmd3_index], command[i]);
                        cmd3_index++;
                    }
                }

                for (int i = cmd1_index; i < 20; i++) {
                    cmd1[i] = NULL;
                }
                for (int i = cmd2_index; i < 20; i++) {
                    cmd2[i] = NULL;
                }
                for (int i = cmd3_index; i < 20; i++) {
                    cmd3[i] = NULL;
                }

                int pipe_fd1[2];        // declaring pipes
                int pipe_fd2[2];
                int pipe_fd3[2];

                pipe(pipe_fd1);
                pipe(pipe_fd2);
                pipe(pipe_fd3);

                if (num_pipes == 1) {       // handle case if only one pipe
                    int pid1 = fork();
                    if (pid1 == 0) {
                        close(pipe_fd1[0]);
                        dup2(pipe_fd1[1], STDOUT_FILENO);       // pipe_out1 replaces stdout
                        close(pipe_fd1[1]);
                        execvp(cmd1[0], cmd1);
                        perror("execvp");
                        exit(1);
                    } else {
                        if (num_pipes == 2) {       // handle case if 2 pipes
                            int pid2 = fork();
                            if (pid2 == 0) {
                                dup2(pipe_fd1[0], STDIN_FILENO);       // pipe_in1 replaces stdin
                                close(pipe_fd1[0]);
                                dup2(pipe_fd2[1], STDOUT_FILENO);       // pipe_out2 replaces stdout
                                close(pipe_fd2[1]);
                                execvp(cmd2[0], cmd2);
                                perror("execvp");
                                exit(1);
                            } else {
                                waitpid(pid2, & status, 0);
                                close(pipe_fd2[1]);
                                dup2(pipe_fd2[0], STDIN_FILENO);       // pipe_in2 replaces stdin
                                close(pipe_fd2[0]);
                                execvp(cmd3[0], cmd3);
                                perror("execvp");
                                exit(1);
                            }
                        } else {
                            waitpid(pid1, & status, 0);
                            close(pipe_fd1[1]);
                            dup2(pipe_fd1[0], STDIN_FILENO);       //pipe_in1 replaces stdin
                            close(pipe_fd1[0]);
                            execvp(cmd2[0], cmd2);
                            perror("execvp");
                            exit(1);
                        }
                    }
                } else {
                    execvp(command[0], command);        // executing command
                    fprintf(stderr, "Error: command not found\n");
                    printf("+ completed '%s' [1]\n", command[0]);
                    exit(1);
                }
            }
        }
    }
    return 0;
}

