            // execute pipeline
            int i, fd[2], in = 0;
            for (i = 0; i <= num_commands; i++) {
                pipe(fd);
                pid_t pid = fork();
                if (pid == -1) {
                    printf("Error: failed to fork process\n");
                    exit(1);
                } else if (pid == 0) {
                    // handle input redirection
                    // case 1: given input file
                    if (i == 0 && strcmp(commands[i][0], "<") == 0) {
                        int fd_in = open(commands[i][1], O_RDONLY);
                        if (fd_in == -1) {
                            printf("Error: failed to open input file '%s'\n", commands[i][1]);
                            exit(1);
                        }
                        dup2(fd_in, STDIN_FILENO);
                        close(fd_in);
                    } else if (i > 0) {
                        // case 2: for all other than first command
                        dup2(in, STDIN_FILENO);
                        close(in);
                    }
                    // handle output redirection
                    if (i < num_commands && strcmp(commands[i+1][0], ">") == 0) {
                        int fd_out = open(commands[i+1][1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                        if (fd_out == -1) {
                            printf("Error: failed to open output file '%s'\n", commands[i+1][1]);
                            exit(1);
                        }
                        dup2(fd_out, STDOUT_FILENO);
                        close(fd_out);
                    } else if (i < num_commands) {
                        dup2(fd[1], STDOUT_FILENO);
                    }
                    // execute command
                    if (execvp(commands[i][0], commands[i]) == -1) {
                        printf("Error: failed to execute command '%s'\n", commands[i][0]);
                        exit(1);
                    }
                } else {
                    wait(NULL);
                    close(fd[1]);
                    in = fd[0];
                }
            }
            num_commands = 0;
        }
    }
    return 0;
}
