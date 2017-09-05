#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>

using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "[childB] Child process started\n";
   
    while(1){
       usleep(10000000);
       cout<<"ChildB wake up\n";
    }
    std::cout << "[childB] Child process exits\n";
    return 0;
}
