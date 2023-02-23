/**
 * This is an example minimal parser in just flex for the grammar of Shell part 1.
 * 
 * This does not contain any logic for executing the commands, but it should serve
 * as a basis on top of which you can easily continue. Throughout the file, we have
 * included some comments and instructions on how to expand.
 * 
 * Floris Westerman, 2022
 */

/* Here we enter a section that is copied verbatim to the output */
%{
// Headers for use in this file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


//////////// Here you can put some helper functions and code, but make sure to properly
//////////// separate your code in logical "entities" in different files! This helps
//////////// us grade your code as well.
#define MAX_ARGS 100 // Maximum number of arguments for execve

char* args[MAX_ARGS+1]; // Arguments for execve, plus one for NULL termination

int idx = 0; // Index of the current argument being parsed
int evalFlag = 0;
int doNotExecute = 0;
int orEnv = 0;
int andEnv = 0;
int status2 = 0;
int temp;
int statusFlag = 0;
void executeCommand();

%}

/**
 * Parsing in flex is done based on a series of regexes. Below, we list these regexes
 * in order, flex will try to match the input with these in order. As soon as any regex
 * matches, it will run the code associated with it. Because of this order, we should
 * put our more specific regexes at the top: put a match for "exit" before a general
 * string regex.
 *
 * Optionally, you can specify a "start condition" (you can see it as some sort of
 * context) for a rule. This causes the regex to only be tried when the parsing is
 * in this context. You can control the current context using BEGIN(). The default
 * start condition is "INITIAL". So below, as soon as we hit a " char, we enter the
 * "STRING" context that will enable us to match any character until the next ",
 * after which we return to the "INITIAL" context.
 *
 * You could decide to hook up Bison to this part as well, as can be seen in many
 * tutorials online. Then you would store/return tokens from the actions in these rules.
 * However, for the scope of this simple grammar for the shell, that is really not
 * necessary and a few state variables should suffice to get the functionality you need!
 */

/* Here we inform flex that we have two additional "start conditions", besides INITIAL */
%x string error

/* Here we inform flex to not "look ahead" in stdin beyond what is necessary, to prevent
 * issues with passing stdin to another executable. */
%option always-interactive

%%
    /* From here on, comments must be indented! */

    /* Here we start with the rules. The highest priority rules are those to accept any
     * symbol when we are reading a string (so in the STRING context). */
    
    /* The first rule is to match the closing " char */
<string>"\""        BEGIN(INITIAL); /* Return to normal parsing */

    /* The second rule takes the longest string of characters not being " */
<string>[^\"]+      {
                        /* Here we match any entire string. We should either make this
                         * the command to execute, or store this as an option, or it is
                         * a filename, depending on the current state! */

                        // Entire string value is available in yytext with length yyleng
                        // Make sure to copy it!

                        // printf("STRING: %s\n", yytext);
                        args[idx++] = strdup(yytext);

                        //////////// Put your code here!
                    }

    /* Here we have cases for error handling - when we have encountered an invalid syntax,
     * we want to discard the entire line so we continue until the next newline symbol. */
<error>\n           BEGIN(INITIAL); /* Return to normal parsing */
<error>.

    /* From here on, we have only "normal" rules for our parsing */

    /* Built-in commands */
"exit"              {
                        /* For built-in command names, make sure that we are not currently
                         * parsing options or some other place where we cannot reasonably
                         * come across an actual "exit" command. So only perform some action
                         * here if you are sure that it is safe and valid to do so, otherwise
                         * use REJECT to let flex continue to the next match (as a string,
                         * later on). */
                        //printf("andEnv = %d, orEnv = %d\n", andEnv, orEnv);
                        //printf("evalFlag = %d", evalFlag);
                        if (orEnv){
                            if (evalFlag == -1){
                                exit(EXIT_SUCCESS);
                            }
                            orEnv = 0;
                        } else if (andEnv) {
                            //printf("test");
                            //printf("evalFlag = %d\n", evalFlag);
                            if (evalFlag == 0){
                                exit(EXIT_SUCCESS);
                            }
                            andEnv = 0;
                        } else {
                            // printf("andEnv = %d, orEnv = %d\n", andEnv, orEnv);
                            // printf("evalFlag = %d", evalFlag);
                            exit(EXIT_SUCCESS);
                        }

                    }
"status"            {
                        // when command for status is called, display the final exit code
                        statusFlag = 1;
                        if (orEnv){
                            if (evalFlag == -1){
                                printf("The most recent exit code is: %d\n", status2);
                            }
                            orEnv = 0;
                        } else if (andEnv) {
                            if (evalFlag == 0){
                                printf("The most recent exit code is: %d\n", status2);
                            }
                            andEnv = 0;
                        } else {
                            printf("The most recent exit code is: %d\n", status2);
                        }
                    }
    /* Other grammar parts */
"\""                BEGIN(string); /* We start reading a string until the next " char */
"&&"                {
                        // printf("Performing &&\n");
                        printf("evalFlag before executing echo b = %d", evalFlag);
                        executeCommand();
                        andEnv = 1;
                        
                        //////////// Put your code here!
                    }
"||"                {
                        // printf("Performing ||\n");
                        executeCommand();
                        orEnv = 1;

                        //////////// Put your code here!
                    }
;                   {   

                        // printf("Performing ;\n");
                        executeCommand();
                        evalFlag = 0;
                        doNotExecute = 0;
                        orEnv = 0;
                        andEnv = 0;

                        //////////// Put your code here!
                    }
\n                  {
                        // printf("\n");
                        executeCommand();
                        evalFlag = 0;
                        doNotExecute = 0;
                        orEnv = 0;
                        andEnv = 0;
                        statusFlag = 0;

                        //////////// Put your code here!
                    }
[ \t]+                /* Ignore whitespace */
[^ ;\t\n\"\^|]+     {
                        /* Here we match any sequence of characters without whitespace as a
                         * "word" or so. We should either make this the command to execute,
                         * or store this as an option, or it is a filename, depending on the
                         * current state! */

                        // Entire string value is available in yytext with length yyleng
                        // Make sure to copy it!
                        // printf("WORD: %s\n", yytext);

                        
                        args[idx++] = strdup(yytext);

                        //////////// Put your code here!
                    }
<<EOF>>             {
                        /* At EOF we should unconditionally terminate! */
                        yyterminate();
                    }
.                   {
                        /* Error: unknown character! (probably doesn't happen) */
                        // fprintf(stdout, "Unrecognized character: %s\n", yytext );
                        BEGIN(error);
                    }

%%


void executeCommand() {
    if (statusFlag) {
        for (int i = 0; i < idx; i++) {
            free(args[i]);
        }
        idx = 0;
    }
    // case where there is no command to execute
    if (idx == 0) {
        return;
    }
    printf("orEnv = %d, andEnv = %d, evalFlag = %d\n", orEnv, andEnv, evalFlag);
    if (orEnv){
        if (evalFlag == -1){
            doNotExecute = 0;
        } else {
            doNotExecute = 1;
        }
    } else if (andEnv) {
        if (evalFlag == -1){
            doNotExecute = 1;
        } else {
            doNotExecute = 0;
        }        
    }
    if (doNotExecute){
        for (int i = 0; i < idx; i++) {
            free(args[i]);
        }
        idx = 0;
        if (orEnv) {
            orEnv = 0;
        } else if (andEnv) {
            andEnv = 0;
        }
        return;
    }

    // NULL terminating the argument list
    args[idx] = NULL;

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        // case where fork failed
        fprintf(stdout, "fork() could not create a child process!");
        exit(0);
    } else if (pid == 0) {
        //printf("entered child fork\n");
        // case of child fork
        if (execvp(args[0], args) == -1) {
            printf("Error: command not found!\n");
            // WHY THE FUCK DOES EVALFLAG NOT CHANGE TO -1???
            evalFlag = -1;
            printf("evalFlag in child fork = %d\n", evalFlag);
            status2 = 127;
            // exit(EXIT_FAILURE);
        } else {
            printf("test2\n");
            evalFlag = 0;
            status2 = 0;
        }
        // exit from the child process
        exit(status2);
    } else {
        // case of parent fork
        int status;
        //printf("entered parent fork\n");
        if (wait(&status) == -1) {
            printf("Child process not ended\n");
            return;
        }
        //status2 = WEXITSTATUS(status);
        //printf("exit status: %d\n", WEXITSTATUS(status));
        // if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS) {
        //     fprintf(stdout, "Command failed with exit status %d\n", WEXITSTATUS(status));
        // }
    }

    // freeing argument strings
    for (int i = 0; i < idx; i++) {
        free(args[i]);
    }
    idx = 0;
    orEnv = 0;
    andEnv = 0;
}

/* All code after the second pair of %% is just plain C where you typically
 * write your main function and such. */

int main(int argc, char *argv[]) {
    // Initialize program
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // Start parsing process
    yylex();

    // Cleanup
    fclose(yyin);
    yylex_destroy();

    return EXIT_SUCCESS;
}
