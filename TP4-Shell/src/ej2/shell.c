#define _POSIX_C_SOURCE 200809L // strdup, getline, strndup

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 64

// Utils

static char *ltrim(char *s)
{
    while (*s && isspace((unsigned char)*s))
        ++s;
    return s;
}
static void rtrim(char *s)
{
    size_t n = strlen(s);
    while (n && isspace((unsigned char)s[n - 1]))
        s[--n] = '\0';
}

#ifndef _GNU_SOURCE
static char *my_strndup(const char *src, size_t n)
{
    char *out = malloc(n + 1);
    if (!out)
        return NULL;
    memcpy(out, src, n);
    out[n] = '\0';
    return out;
}
#define strndup my_strndup
#endif

static int split_pipeline(const char *line, char ***pieces)
{
    size_t cap = 8, cnt = 0;
    char **out = malloc(cap * sizeof(char *));
    int in_s = 0, in_d = 0;
    const char *seg = line;

    for (const char *p = line;; ++p)
    {
        char c = *p;
        if (c == '\0' || (c == '|' && !in_s && !in_d))
        {
            size_t len = p - seg;
            char *tmp = strndup(seg, len);
            rtrim(tmp);
            char *trim = ltrim(tmp);
            if (*trim == '\0')
            {
                for (size_t i = 0; i < cnt; ++i)
                    free(out[i]);
                free(out);
                free(tmp);
                *pieces = NULL;
                return -1;
            }
            if (cnt == cap)
            {
                cap *= 2;
                out = realloc(out, cap * sizeof(char *));
            }
            out[cnt++] = strdup(trim);
            free(tmp);
            if (c == '\0')
                break;
            seg = p + 1;
        }
        else if (c == '\'' && !in_d)
            in_s ^= 1;
        else if (c == '"' && !in_s)
            in_d ^= 1;
    }
    if (in_s || in_d)
    {
        for (size_t i = 0; i < cnt; ++i)
            free(out[i]);
        free(out);
        *pieces = NULL;
        return -2;
    }
    out = realloc(out, (cnt + 1) * sizeof(char *));
    out[cnt] = NULL;
    *pieces = out;
    return (int)cnt;
}

static int split_args(char *cmd, char ***argv)
{
    int cap = 8, argc = 0;
    *argv = malloc(cap * sizeof(char *));
    int in_s = 0, in_d = 0, esc = 0;
    char *buf = calloc(strlen(cmd) + 1, 1), *o = buf;

    for (char *p = cmd;; ++p)
    {
        char c = *p;
        if (c == '\0' || (!in_s && !in_d && !esc && isspace((unsigned char)c)))
        {
            if (o > buf)
            {
                *o = '\0';
                if (argc == cap)
                {
                    cap *= 2;
                    *argv = realloc(*argv, cap * sizeof(char *));
                }
                (*argv)[argc++] = strdup(buf);
                o = buf;
            }
            if (c == '\0')
                break;
        }
        else
        {
            if (!esc && c == '\\')
            {
                esc = 1;
                continue;
            }
            if (!esc && c == '\'' && !in_d)
            {
                in_s ^= 1;
                continue;
            }
            if (!esc && c == '"' && !in_s)
            {
                in_d ^= 1;
                continue;
            }
            *o++ = c;
            esc = 0;
        }
    }
    free(buf);
    if (in_s || in_d)
    {
        for (int i = 0; i < argc; ++i)
            free((*argv)[i]);
        free(*argv);
        *argv = NULL;
        return -1;
    }
    (*argv)[argc] = NULL;
    return argc;
}

static void free_strv(char **v)
{
    if (!v)
        return;
    for (char **p = v; *p; ++p)
        free(*p);
    free(v);
}

// main
int main()
{

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    int interactive = isatty(STDIN_FILENO);

    while (1)
    {
        if (!interactive)
        {
        } // no ptompt
        else
            printf("Shell> ");

        /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
        fgets(command, sizeof(command), stdin);

        // sigue leyendo si la linea es mayor a 256 bytes
        size_t full_len = strlen(command);
        char *full = strdup(command);
        while (full_len && full[full_len - 1] != '\n' && !feof(stdin))
        {
            char extra[256];
            if (!fgets(extra, sizeof(extra), stdin))
                break;
            size_t add = strlen(extra);
            full = realloc(full, full_len + add + 1);
            memcpy(full + full_len, extra, add + 1);
            full_len += add;
        }
        if (full_len && full[full_len - 1] == '\n')
            full[--full_len] = '\0';

        /* Removes the newline character (\n) from the end of the string stored in command, if present.
           This is done by replacing the newline character with the null character ('\0').
           The strcspn() function returns the length of the initial segment of command that consists of
           characters not in the string specified in the second argument ("\n" in this case). */
        command[strcspn(command, "\n")] = '\0';

        /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function.
           Each resulting token is stored in the commands[] array.
           The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |.
           In each iteration of the while loop, strtok() returns the next token found in command.
           The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
        char *token = strtok(command, "|");
        while (token != NULL)
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        rtrim(full);
        char *line_begin = ltrim(full);
        if (*line_begin == '\0')
        { // linea vacia
            command_count = 0;
            free(full);
            continue;
        }

        char **stages = NULL;
        int n_stages = split_pipeline(line_begin, &stages);
        if (n_stages < 0)
        {
            fprintf(stderr, "Syntax error\n");
            command_count = 0;
            free(full);
            continue;
        }

        if (n_stages == 1 && strcmp(ltrim(stages[0]), "exit") == 0)
        {
            free_strv(stages);
            free(full);
            exit(0);
        }

        pid_t *children = malloc(n_stages * sizeof(pid_t));
        int child_cnt = 0;
        int prev_in = -1;

        for (int i = 0; i < n_stages; ++i)
        {
            int pipefd[2] = {-1, -1};
            if (i < n_stages - 1 && pipe(pipefd) < 0)
            {
                perror("pipe");
                break;
            }

            pid_t pid = fork();
            if (pid < 0)
            {
                perror("fork");
                break;
            }
            else if (pid == 0)
            { // padre
                if (prev_in != -1)
                    dup2(prev_in, STDIN_FILENO);
                if (pipefd[1] != -1)
                    dup2(pipefd[1], STDOUT_FILENO);

                if (prev_in != -1)
                    close(prev_in);
                if (pipefd[0] != -1)
                    close(pipefd[0]);
                if (pipefd[1] != -1)
                    close(pipefd[1]);

                char **argv = NULL;
                int argc = split_args(stages[i], &argv);
                if (argc < 0)
                {
                    fprintf(stderr, "Unmatched quotes\n");
                    _exit(1);
                }
                if (argc > MAX_ARGS)
                {
                    fprintf(stderr, "Too many arguments\n");
                    _exit(1);
                }

                execvp(argv[0], argv);
                perror("execvp");
                _exit(127);
            }
            else
            { // padre
                children[child_cnt++] = pid;
                if (prev_in != -1)
                    close(prev_in);
                if (pipefd[1] != -1)
                    close(pipefd[1]);
                prev_in = pipefd[0];
            }
        }

        if (prev_in != -1)
            close(prev_in);
        for (int i = 0; i < child_cnt; ++i)
            waitpid(children[i], NULL, 0);

        free(children);
        free_strv(stages);
        free(full);
        command_count = 0;

        for (int i = 0; i < command_count; i++)
        {
            printf("Command %d: %s\n", i, commands[i]);
        }
    }
    return 0;
}
