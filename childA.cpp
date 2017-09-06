#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <algorithm>
#include <iostream>

#define BUFSIZE 1024

using namespace std;

void kill_child(int sig){
    cout<<"[childA] Child process exits\n";
    exit(EXIT_SUCCESS);
}

int main(int argc, const char * argv[]) {
 
    std::cout << "[childA] Child process started\n";
    signal(SIGTERM,(void (*)(int))kill_child);  

    usleep(500000);
   while(1){
       int buffer[BUFSIZE];
       int nbytes;
       int len;
       // get pipe data
       nbytes =  read(0,buffer , sizeof(int)*BUFSIZE);
       len = buffer[0];
       cout<<"[childA] Random Numbers Received From Pipe:";
       for(int i=1; i<=len; i++){
           cout<<buffer[i]<<" ";
       }
       cout<<endl;

       // sort
       sort(buffer+1, buffer+len+1);
       
       // get median
       double mid;
       
       if(len%2){
           mid = buffer[(len/2 + 1)];
       }else{
           mid = (buffer[(len/2)] + buffer[(len/2 + 1)]) / 2.0;
       }
       cout<<"[childA] Median: "<<mid<<endl;
    }

    return 0;
}
