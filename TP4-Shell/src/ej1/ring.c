#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

// Funciones auxiliares para manejar la salida del programa
static void custom_exit(int code)
{
    if (code == 0)
        _exit(1);
    _exit(code);
}
#define exit(code) custom_exit(code)

static void xwrite(int fd, const void *buf, size_t n)
{
    const char *p = buf;
    while (n)
    {
        ssize_t k = write(fd, p, n);
        if (k < 0)
        {
            perror("write");
            _exit(1);
        }
        p += k;
        n -= (size_t)k;
    }
}
static void xread(int fd, void *buf, size_t n)
{
    char *p = buf;
    while (n)
    {
        ssize_t k = read(fd, p, n);
        if (k <= 0)
        {
            perror("read");
            _exit(1);
        }
        p += k;
        n -= (size_t)k;
    }
}

int main(int argc, char **argv)
{
    int start, status, pid, n;
    int buffer[1];

    if (argc != 4)
    {
        printf("Uso: anillo <n> <c> <s> \n");
        exit(0);
    }

    /* Parsing of arguments */
    char *endptr;

    // n: cantidad de procesos (>0)
    long ln = strtol(argv[1], &endptr, 10);
    if (*endptr || ln <= 0 || ln > INT_MAX)
    {
        fprintf(stderr, "n inválido\n");
        return 1;
    }
    n = (int)ln;

    // c: valor inicial (INT range)
    long long lc = strtoll(argv[2], &endptr, 10);
    if (*endptr || lc < INT_MIN || lc > INT_MAX)
    {
        fprintf(stderr, "c fuera de rango int\n");
        return 1;
    }
    buffer[0] = (int)lc;

    // s: índice del proceso que arranca (0 ≤ s < n)
    long ls = strtol(argv[3], &endptr, 10);
    if (*endptr || ls < 0 || ls >= n)
    {
        fprintf(stderr, "s inválido\n");
        return 1;
    }
    start = (int)ls;

    // Evitar overflow cuando se sumen los n incrementos
    if ((buffer[0] > 0 && n > INT_MAX - buffer[0]) ||
        (buffer[0] < 0 && n < INT_MIN - buffer[0]))
    {
        fprintf(stderr, "overflow\n");
        return 1;
    }

    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n",
           n, buffer[0], start);

    /* You should start programming from here... */

    // Pipes
    int (*p)[2] = malloc(sizeof(int[2]) * n);
    if (!p)
    {
        perror("malloc");
        return 1;
    }
    for (int i = 0; i < n; ++i)
        if (pipe(p[i]) == -1)
        {
            perror("pipe");
            return 1;
        }

    // Pipe para devolver resultado al parent
    int parent_pipe[2];
    if (pipe(parent_pipe) == -1)
    {
        perror("pipe");
        return 1;
    }

    int last = (start + n - 1) % n;

    for (int i = 0; i < n; ++i)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return 1;
        }

        if (pid == 0)
        {
            // Hijos
            if (dup2(p[i][0], STDIN_FILENO) == -1)
            {
                perror("dup2");
                _exit(1);
            }
            int out_fd = (i == last) ? parent_pipe[1] : p[(i + 1) % n][1];
            if (dup2(out_fd, STDOUT_FILENO) == -1)
            {
                perror("dup2");
                _exit(1);
            }

            for (int j = 0; j < n; ++j)
            {
                close(p[j][0]);
                close(p[j][1]);
            }
            close(parent_pipe[0]);
            close(parent_pipe[1]);

            int val;
            xread(STDIN_FILENO, &val, sizeof(val));
            ++val;
            xwrite(STDOUT_FILENO, &val, sizeof(val));
            _exit(0);
        }
    }

    for (int i = 0; i < n; ++i)
    {
        close(p[i][0]);
        if (i != start)
            close(p[i][1]);
    }
    close(parent_pipe[1]);

    xwrite(p[start][1], buffer, sizeof(int));
    close(p[start][1]);

    int final;
    xread(parent_pipe[0], &final, sizeof(int));
    close(parent_pipe[0]);

    while (wait(&status) > 0)
    {
    }

    free(p);

    printf("Valor final: %d\n", final);
    return 0;
}
