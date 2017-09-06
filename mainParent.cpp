#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>

#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

#define BUFSIZE 2048
#define SEM_NAME "/semaphore"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1

vector<int> genInt(int num){
    unsigned seed;
    seed = (unsigned)time(NULL); // 取得時間序列
    srand(seed); // 以時間序列當亂數種子
    vector<int> ret_vec(num);
    
    cout<<"[mainParent] Generating "<<num<<" random integers:";
    for(int i=0; i<num; i++){
        ret_vec[i] = rand()%51+50 ;
        cout << ret_vec[i] <<" ";
    }
    cout<<endl;
    return ret_vec;
}

int main(int argc, const char * argv[]) {
  
    cout<<"[mainParent] Main Process Started\n";

    int buffer[1024]= {0};
    int len;
    int pfd[2];
    pid_t pid;
    
    // create pipe
    if(pipe(pfd) < 0){
        perror("pipe fail");
        return -1;
    }
    
    // semaphore
    sem_t *semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    
    if (semaphore == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    
    // Close the semaphore as we won't be using it in the parent process
    if (sem_close(semaphore) < 0) {
        perror("sem_close(3) failed");
        sem_unlink(SEM_NAME);
        exit(EXIT_FAILURE);
    }
    
    // fork create childA&B
    pid_t cpid_a, cpid_b, rpid_a, rpid_b;
  
    cpid_a = fork();
    
    if(cpid_a < 0) {
        cout << "[mainParent] Fork failed.\n";
        return -1;
    }
    else if(cpid_a == 0) { // code for child process A
        close(pfd[1]);
        close(STDIN_FILENO);
        int ret = dup2(pfd[0], STDIN_FILENO);

        if(execl("childA", NULL, NULL)<0)
            perror("Err on execlp");
    }
    else { // code for parent process
        close(pfd[0]);
        
        cpid_b = fork();
        if(cpid_b == 0){ //code for child process B
            if(execl("childB", NULL, NULL)<0)
                perror("Err on execlp");
        }
    }
    
    cout<<"[mainParent] childA Process Created\n";
    cout<<"[mainParent] childB Process Created\n";
    
    // shared memory setting
    int shmid;
    key_t key;
    int *shmadd;
    
    key = ftok(".",5566);
    if(key == -1){
        perror("ftok");
    }
    
    shmid = shmget(key, BUFSIZE, SHM_R|SHM_W|IPC_CREAT);
    if(shmid < 0){
        perror("shmget");
        exit(-1);
    }
    
    shmadd = (int*)shmat(shmid, NULL, 0);
    if(shmadd < 0){
        perror("shmat");
        exit(-1);
    }
    
    
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
            vector<int> rand_nums(myNumber);
            rand_nums = genInt(myNumber);
            
            memset(shmadd, 0, BUFSIZE);
            memcpy(shmadd, &myNumber, sizeof(int));
            memcpy(shmadd+1, rand_nums.data(), myNumber*sizeof(int));
            kill(cpid_b, SIGUSR1);
            
            rand_nums.insert(rand_nums.begin(), myNumber);
            write(pfd[1], rand_nums.data(), (myNumber+1)*sizeof(int));
            
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
    
    if (sem_unlink(SEM_NAME) < 0)
        perror("sem_unlink(3) failed");
    
    cout<<"[mainParent] Process Exits\n";
    return 0;
}


