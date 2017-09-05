#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include <iostream>
#include <sstream>
using namespace std;

void genInt(int num){
    unsigned seed;
    seed = (unsigned)time(NULL); // 取得時間序列
    srand(seed); // 以時間序列當亂數種子
    for(int i=0; i<num; i++)
        cout << rand()%51+50 <<" ";
    
    cout<<endl;
}

int main(int argc, const char * argv[]) {
  
    cout<<"[mainParent] Main Process Started\n";

    // fork create childA&B
    pid_t cpid_a, cpid_b, rpid_a, rpid_b;
  
    cpid_a = fork();
    
    if(cpid_a < 0) {
        cout << "[mainParent] Fork failed.\n";
        return 1;
    }
    else if(cpid_a == 0) { // code for child process A
        if(execl("childA", NULL, NULL)<0)
            perror("Err on execlp");
    }
    else { // code for parent process
        cpid_b = fork();
        if(cpid_b == 0) //code for child process B
            if(execl("childB", NULL, NULL)<0)
                perror("Err on execlp");
    }
    
    cout<<"[mainParent] childA Process Created\n";
    cout<<"[mainParent] childB Process Created\n";
    
    // User Input
    string input = "";
    int myNumber=0;
    while (true) {
        usleep(500000);
        cout << "[mainParent] Enter a positive interger or 0 to exit: ";
        getline(cin, input);
        
        // This code converts from string to number safely.
        stringstream myStream(input);
        if (myStream >> myNumber){
            if(myNumber == 0){
                break;
            }
            genInt(myNumber);
        }
        else
            cout << "[mainParent] Invalid number, please try again" << endl;
    }

    // kill and wait
    int status_a,status_b, ret;
    kill(cpid_a, SIGTERM);
    kill(cpid_b, SIGTERM);
    
    cout<<"[mainParent] Process Waits\n";

    waitpid(cpid_a, &status_a, 0);
    waitpid(cpid_b, &status_b, 0);
    if (status_a > 0 || status_b > 0){
        string s;
        if(status_a){
            s.append(" childA ");
        }
        if(status_b){
            s.append(" childB ");
        }
        cout<<"[mainParent] The child process "<<s<<" terminated with an error!\n";
    }
    
    cout<<"[mainParent] Process Exits\n";
    return 0;
}


