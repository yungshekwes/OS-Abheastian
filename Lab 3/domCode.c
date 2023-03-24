void executePipe() {
    flag.sameName = false;

    if(flag.hasInFile && flag.hasOutFile && strcmp(inputFile,outputFile)==0){
        flag.sameName = true;
        printf("Error: input and output files cannot be equal!\n");
        free(inputFile);
        free(outputFile);
        // freeTokenList(tokenList);
        exitStatus = 1;
        return true;
    }


    int prev_pipe, pfds[2];
    prev_pipe = STDIN_FILENO;
    pid_t pids[numCommands];
    int i;
    // Execute commands in pipeline
    if (flag.cd){
        // int nothing = 0;
        cdCheck();
    } else {
        for (i = 0; i < numCommands; i++) {
            pipe(pfds);
            int pid = fork();
            if (pid == -1) {
                printf("fork!\n");
                exit(0);
            } else if (pid == 0) {
                dup2(prev_pipe, STDIN_FILENO);
                if (i != numCommands -1 ) {
                    // Redirect previous pipe to stdin
                    if (prev_pipe != STDIN_FILENO) {
                        dup2(prev_pipe, STDIN_FILENO);
                        close(prev_pipe);
                    }
                    // Redirect stdout to current pipe
                    dup2(pfds[1], STDOUT_FILENO);
                    // Start command
                } else {
                    // Get stdin from last pipe
                    if (prev_pipe != STDIN_FILENO) {
                        dup2(prev_pipe, STDIN_FILENO);
                        close(prev_pipe);
                    }
                    // Redirect standard output if necessary
                    if (flag.hasOutFile && outputFile != NULL) {
                        // FILE *fp = open(outputFile, "w", stdout);
                        int fp = open(outputFile, O_CREAT | O_WRONLY | O_TRUNC);
                        // if (fp == NULL) {
                        //     free(outputFile);
                        //     perror("Error: cannot open input file");
                        //     exitStatus = 1;
                        //     _exit(EXIT_FAILURE);
                        // }
                        dup2(fp, STDIN_FILENO);
                        close(fp);
                    }

                    // Redirect standard output if necessary
                    if (flag.hasInFile && inputFile != NULL) {
                        // FILE *fp = open(inputFile, "w", stdout);
                        int fp = open(inputFile, O_RDONLY);
                        // if (fp == NULL) {
                        //     free(inputFile);
                        //     perror("Error: cannot open output file");
                        //     exitStatus = 1;
                        //     _exit(EXIT_FAILURE);
                        // }
                        dup2(fp, STDIN_FILENO);
                        close(fp);
                    }
                }
                close(pfds[1]);
                // close(pfds[0]);
                // close(prev_pipe);
                int execution;
                if(commandArray[i][0]!=NULL && commandArray[i]!=NULL){
                    execution = execvp(commandArray[i][0], commandArray[i]);
                }else{
                    printf("Error: command not found!\n");
                    _exit(127);
                }
                if(execution == -1){
                    printf("Error: command not found!\n");
                    freeArguments();
                    free(command);
                    freeNewArray(commandArray);
                    free(inputLine);
                    freeTokenList(tokenList);
                    _exit(127);
                }
            } else {
                pids[i] = pid;

                // Close read end of previous pipe (not needed in the parent)
                if(prev_pipe != STDIN_FILENO) close(prev_pipe);

                // Close write end of current pipe (not needed in the parent)
                close(pfds[1]);

                // Save read end of current pipe to use in next iteration
                prev_pipe = pfds[0];
            }

        }

        close(prev_pipe);


        for (int i = 0; i < numCommands; i++) {
            int childStatus;
            waitpid(pids[i], &childStatus, 0);
            int weExited = WIFEXITED(childStatus);
            int childExitStatus = 0;
            if (weExited) {
                childExitStatus = WEXITSTATUS(childStatus);
            }
            exitStatus = childExitStatus;

        }
        // free(inputFile);
        // free(outputFile);
        freeNewArray(commandArray);
    }
}