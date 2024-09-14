#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int nr1, nr2;
    char op;

    printf("nr1 nr2 op:\n");
    scanf("%d %d %c", &nr1, &nr2, &op);

    pid_t pid = fork();
    if (pid == 0) {
        //copil
        char num1_str[20], num2_str[20];
        sprintf(num1_str, "%d", nr1);
        sprintf(num2_str, "%d", nr2);

        execl("./server", "server", num1_str, num2_str, &op, NULL);
        perror("exec");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // parinte
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            int result = WEXITSTATUS(status);
            printf("result: %d ", result);
        }
    } else {
        perror("\n error fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}
