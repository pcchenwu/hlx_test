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

using namespace std;

void kill_child(int sig){
    cout<<"[childB] Child process exits\n";
    exit(EXIT_SUCCESS);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "[childB] Child process started\n";
    signal(SIGTERM,(void (*)(int))kill_child);  
   
    while(1){
       usleep(10000000);
       cout<<"ChildB wake up\n";
    }

   return 0;
}
