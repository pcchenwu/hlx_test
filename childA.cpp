#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


#include <iostream>

#define BUFSIZE 2048

using namespace std;

void kill_child(int sig){
    cout<<"[childA] Child process exits\n";
    exit(EXIT_SUCCESS);
}

int main(int argc, const char * argv[]) {
 
    std::cout << "[childA] Child process started\n";
    signal(SIGTERM,(void (*)(int))kill_child);  

   while(1){
       usleep(10000000);
       cout<<"ChildA wake up\n";
    }

    return 0;
}
