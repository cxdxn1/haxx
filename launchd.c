// PLEASE DO NOT USE THIS YET - STILL IN TESTING AND COULD DESTROY YOUR WHOLE DEVICE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <spawn.h>
#include <string.h>
#include <IOKit/IOKitLib.h>

#define LAUNCHD "/sbin/launchd.real"
#define HAXX "/sbin/haxx"
#define FILE_TO_WRITE "/path/to/file" // This needs fixing lol

static const char __unused fakelaunchd[] = "fakelaunchd";
char *real_argv[] = {"LAUNCHD_HAXX", NULL};

void log_error(const char *message) {
    fprintf(stderr, "%s\n", message);
}

void close_fds() {
    close(0);
    close(1);
    close(2);
}

void execute_launchd(char *envp[]) {
    execve(LAUNCHD, real_argv, envp);
    log_error("Cannot execute launchd! Exiting...");
    exit(1);
}

void execute_haxx(char *envp[]) {
    execve(HAXX, real_argv, envp);
    log_error("Cannot execute haxx! Exiting...");
    exit(1);
}

int main(int argc, char *argv[], char *envp[]) {
    if (getpid() != 1) {
        log_error("fakelaunchd cannot be run directly. Exiting...");
        exit(1);
    }

    int fd_console = open("/dev/console", O_RDWR, 0);
    dup2(fd_console, 0);
    dup2(fd_console, 1);
    dup2(fd_console, 2);
    close(fd_console);

    for (uint8_t i = 0; i < 10; i++) {
        printf("============ WE ARE PID 1 ============\n");
    }

    io_registry_entry_t entry = IORegistryEntryFromPath(MACH_PORT_NULL, "IODeviceTree:/options");
    if (entry == 0) {
        execute_launchd(envp);
    }

    CFStringRef cfNvramVar = IORegistryEntryCreateCFProperty(entry, CFSTR("boot-args"), kCFAllocatorDefault, 0);
    if (cfNvramVar == NULL) {
        execute_launchd(envp);
    }

    const char *nvramVar = CFStringGetCStringPtr(cfNvramVar, kCFStringEncodingUTF8);

if (nvramVar != NULL)
    {
        if (strstr(nvramVar, "no_untether") != NULL)
        {
            // Write a file only if "no_untether" is set
            int fd = open(FILE_TO_WRITE, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd != -1)
            {
                const char *content = "File content specific to no_untether\n";
                write(fd, content, strlen(content));
                close(fd);
            }

            execve(LAUNCHD, real_argv, envp);
            fprintf(stderr, "cannot execute %s!g: %s... bailing\n", LAUNCHD, strerror(errno));
            exit(1);
        }
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        execve(HAXX, real_argv, envp);
        fprintf(stderr, "cannot execute %s!g: %s... bailing\n", HAXX, strerror(errno));
        exit(1);
    }
    else
    {
        execve(LAUNCHD, argv, envp);
        fprintf(stderr, "cannot execute %s!g: %s... spinning\n", LAUNCHD, strerror(errno));
        while (1)
        {
            sleep(__INT_MAX__);
        }
    }
    exit(42);
}
