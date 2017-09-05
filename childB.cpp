#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>

#include <algorithm>
#include <iostream>

#define BUFSIZE 2048

using namespace std;

static bool isUpdated = false;

void kill_child(int sig){
    cout<<"[childB] Child process exits\n";
    exit(EXIT_SUCCESS);
}

void data_updated(int sig){
    isUpdated = true;
}

int main(int argc, const char * argv[]) {
    // signal
    std::cout << "[childB] Child process started\n";
    signal(SIGTERM,(void (*)(int))kill_child);
    signal(SIGUSR1,(void (*)(int))data_updated);
   
    // shared memory
    int shmid;
    key_t key;
    int *shmadd;
    
    key = ftok(".",5566);
    if(key == -1){
        perror("ftok");
    }
    
    shmid = shmget(key, BUFSIZE, SHM_R|SHM_W);
    if(shmid < 0){
        perror("shmget");
        exit(-1);
    }
    
    shmadd = (int*)shmat(shmid, NULL, 0);
    if(shmadd < 0){
        perror("shmat");
        exit(-1);
    }
    
    while(1){
        usleep(5000000);
        
        if(isUpdated){
            int len;
            memcpy(&len, shmadd, sizeof(int));
            int array[len];
            memcpy(array,shmadd+1,len*sizeof(int));

            cout<<"[childB] Random Numbers Received From Shared Memory:";
            for(int i=0; i< len; i++){
                cout<<array[i]<<" ";
            }
            cout<<endl;
    
            sort(array, array+len);
        
            cout<<"[childB] Sorted Sequence:";
            for(int i=0; i< len; i++){
                cout<<array[i]<<" ";
            }
            cout<<endl;
        
            double x=1;
            for(int i=0; i< len; i++){
                x *= array[i];
            }
            x = pow(x,(1.0/len));
            cout<<"[childB] Geometric Mean:"<<x<<endl;
            isUpdated = false;
        }
    }

   return 0;
}
