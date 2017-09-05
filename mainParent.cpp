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
  
    cout<<"fork?\n";
    pid_t cpid_a, cpid_b, rpid_a, rpid_b;
    
    cpid_a = fork();
    cout<<"cpid_a:"<<cpid_a<<endl;
    
    if(cpid_a < 0) {
        cout << "Fork failed." << '\n';
        return 1;
    }
    else if(cpid_a == 0) { // code for child process A
        cout<<"in a\n";
        if(execl("/Users/pcchenwu/Desktop/develop/hlx/childA", NULL, NULL)<0)
            perror("Err on execlp");
    }
    else { // code for parent process
        cout<<"ready for b\n";
        cpid_b = fork();
        if(cpid_b == 0)
            if(execl("/Users/pcchenwu/Desktop/develop/hlx/childB", NULL, NULL)<0)
                perror("Err on execlp");
//        while(wait(NULL) != -1);
    }
    
    string input = "";
    int myNumber=0;
    while (true) {
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
            cout << "Invalid number, please try again" << endl;
    }

    int status, ret;
    kill(cpid_a, SIGTERM);
    kill(cpid_b, SIGTERM);
    
    
    do{
        
//        rpid_a = waitpid(cpid_a,&status, WNOHANG);
        rpid_a = waitpid(cpid_a,&status, WEXITED);
        /*使用了WNOHANG参數，waitpid不會在這裏等待 */
        
        if(rpid_a==0){
            
            /*如果沒有收集到子進程 */
            
            printf("Nochild exited\n");
            
            sleep(1);
            
        }
        
    }        while(rpid_a==0);
    
    /*沒有收集到子進程，就回去繼續嘗試 */
    
    if(cpid_a==rpid_a)
        printf("successfullyget child %d\n", cpid_a);
    else
        printf("someerror occured\n");
    
    if(WIFEXITED(status))
    {
        ret = WEXITSTATUS(status);
    }
    else if(WIFSIGNALED(status))
    {
        ret = WTERMSIG(status);
    }
//    wait(&cpid_b);
    cout<<"[mainParent] Process Exits\n";
    return 0;
}


