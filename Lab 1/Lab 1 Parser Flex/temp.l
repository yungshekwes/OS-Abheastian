%{
// Headers for use in this file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
%}

#define MAX_ARGS 100 // Maximum number of arguments for execve

char* args[MAX_ARGS+1]; // Arguments for execve, plus one for NULL termination

int arg_idx = 0; // Index of the current argument being parsed

%x string error

/* Here we inform flex to not "look ahead" in stdin beyond what is necessary, to prevent
 * issues with passing stdin to another executable. */
%option always-interactive

%%

<string>"\""        BEGIN(INITIAL); /* Return to normal parsing */

    /* The second rule takes the longest string of characters not being " */
<string>[^\"]+      {
                        printf("STRING: %s\n", yytext);
                        args[arg_idx++] = strdup(yytext);
                    }


<error>\n           BEGIN(INITIAL); /* Return to normal parsing */
<error>.

    /* From here on, we have only "normal" rules for our parsing */

    /* Built-in commands */
"exit"              {
                        REJECT;
                    }

    /* Other grammar parts */
"\""                BEGIN(string); /* We start reading a string until the next " char */
"&&"                {
                        printf("Performing &&\n");
                        exec_command();
                    }
"||"                {
                        printf("Performing ||\n");
                        exec_command();
                    }
;                   {
                        printf("Performing ;\n");
                        exec_command();
                    }
\n                  {
                        printf("\n");
                        exec_command();
                    }
[ \t]+              /* Ignore whitespace */
[^ ;\t\n\"\^|]+     {
                        printf("WORD: %s\n", yytext);
                        args[arg_idx++] = strdup(yytext);
                    }
<<EOF>>             {
                        /* At EOF we should unconditionally terminate! */
                        yyterminate();
                    }
.                   {
                        fprintf(stdout, "Unrecognized character: %s\n", yytext );
                        BEGIN(error);
                    }

%%

void exec_command() {
    if (arg_idx == 0) {
        return; // No command to execute
    }
    args[arg_idx] = NULL; // Null-terminate the argument list

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        // Child process
        if (execve(args[0], args, NULL) == -1) {
            perror("execve");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return;
        }
        if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS) {
            fprintf(stderr, "Command failed with exit status %d\n", WEXITSTATUS(status));
        }
    }

    // Free argument strings
    for (int i = 0; i < arg_idx; i++) {
        free(args[i]);
    }
    arg_idx = 0; // Reset argument index
}

int main(int argc, char *argv[]) {

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    yylex();

    fclose(yyin);
    yylex_destroy();

    return EXIT_SUCCESS;
}