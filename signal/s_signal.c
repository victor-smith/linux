#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void sigroutine(int dunno)
{
printf("enter %s\n",__FUNCTION__);
        switch (dunno) {
        case 1:
        printf("Get a signal -- SIGHUP \n");
        break;

        case 2:
        printf("Get a signal -- SIGINT \n");
        break;

        case 3:
        printf("Get a signal -- SIGQUIT \n");
        break;
        }
        return;
}



int main(int argc, char** argv) {
        printf("process id is %d \n",getpid());
        signal(SIGHUP, sigroutine);
        signal(SIGINT, sigroutine);
        signal(SIGQUIT,sigroutine);

        for (;;) ;
        return 0;
}
