#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

// Colores
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define NC      "\033[0m"

int test_count = 0;
int fail_count = 0;

void print_result(const char* desc, int passed) {
    test_count++;
    printf("   %s%s %s%s\n", passed ? GREEN : RED, passed ? "✔" : "✘", desc, NC);
    if (!passed) fail_count++;
}

void report_error(const char* label, ssize_t r, const char* desc) {
    fprintf(stderr, "      %s✘ %s: %s() → %zd, errno=%d (%s)%s\n",
            RED, desc, label, r, errno, strerror(errno), NC);
}

void expect_write(int fd, void *buf, size_t size, const char* desc, int should_fail) {
    errno = 0;
    ssize_t r = write(fd, buf, size);
    int failed = (r == -1);
    int passed = (should_fail && failed) || (!should_fail && !failed && r == (ssize_t)size);
    print_result(desc, passed);
    if (failed && !should_fail) report_error("write", r, desc);
    if (!failed && should_fail)
        fprintf(stderr, "      %s✘ '%s' no falló como se esperaba%s\n", RED, desc, NC);
}

void expect_read(int fd, void *buf, size_t size, const char* desc, int should_fail) {
    errno = 0;
    ssize_t r = read(fd, buf, size);
    int failed = (r == -1);
    int passed = (should_fail && failed) || (!should_fail && !failed);
    print_result(desc, passed);
    if (failed && !should_fail) report_error("read", r, desc);
    if (!failed && should_fail)
        fprintf(stderr, "      %s✘ '%s' no falló como se esperaba%s\n", RED, desc, NC);
}

// Macros para simplificar tests
#define EXPECT_FAIL_WRITE(fd, buf, size, desc)    expect_write(fd, buf, size, desc, 1)
#define EXPECT_SUCCESS_WRITE(fd, buf, size, desc) expect_write(fd, buf, size, desc, 0)
#define EXPECT_FAIL_READ(fd, buf, size, desc)     expect_read(fd, buf, size, desc, 1)
#define EXPECT_SUCCESS_READ(fd, buf, size, desc)  expect_read(fd, buf, size, desc, 0)

int main() {
    signal(SIGPIPE, SIG_IGN);
    int val = 123, out = 0;

    printf("%s● WRITE TESTS%s\n", YELLOW, NC);

    {
        int fd[2]; pipe(fd);
        close(fd[1]);
        EXPECT_FAIL_WRITE(fd[1], &val, sizeof(val), "write en pipe cerrado (escritura)");
        close(fd[0]);
    }

    {
        int fd[2]; pipe(fd);
        close(fd[0]);
        EXPECT_FAIL_WRITE(fd[1], &val, sizeof(val), "write sin lector");
        close(fd[1]);
    }

    EXPECT_FAIL_WRITE(-1, &val, sizeof(val), "write en descriptor inválido (-1)");

    {
        int fd = open("tester.c", O_RDONLY);
        if (fd >= 0) {
            EXPECT_FAIL_WRITE(fd, &val, sizeof(val), "write en archivo solo lectura");
            close(fd);
        } else {
            print_result("No se pudo abrir tester.c para prueba de solo lectura", 0);
        }
    }

    {
        int fd[2]; pipe(fd);
        EXPECT_SUCCESS_WRITE(fd[1], &val, sizeof(val), "escritura válida en pipe");
        close(fd[0]); close(fd[1]);
    }

    printf("\n%s● READ TESTS%s\n", YELLOW, NC);

    EXPECT_FAIL_READ(-1, &out, sizeof(out), "read en descriptor inválido (-1)");

    {
        int fd[2]; pipe(fd);
        close(fd[1]);
        EXPECT_SUCCESS_READ(fd[0], &out, sizeof(out), "read en pipe con escritura cerrada (EOF)");
        close(fd[0]);
    }

    {
        int fd = open("tester.c", O_WRONLY);
        if (fd >= 0) {
            EXPECT_FAIL_READ(fd, &out, sizeof(out), "read en archivo solo escritura");
            close(fd);
        } else {
            print_result("No se pudo abrir tester.c para prueba de solo escritura", 0);
        }
    }

    {
        int fd[2]; pipe(fd);
        write(fd[1], &val, sizeof(val));
        EXPECT_SUCCESS_READ(fd[0], &out, sizeof(out), "lectura válida en pipe");
        close(fd[0]); close(fd[1]);
    }

    printf("\n%s--------------------------------------------%s\n", BLUE, NC);
    printf("         %sRESUMEN FINAL DE TESTS%s\n", YELLOW, NC);
    printf("   Total de tests:     %d\n", test_count);
    printf("   %s✔ Correctos:       %d%s\n", GREEN, test_count - fail_count, NC);
    printf("   %s✘ Fallidos:        %d%s\n", RED, fail_count, NC);
    printf("%s--------------------------------------------%s\n", BLUE, NC);

    return fail_count == 0 ? 0 : 1;
}
