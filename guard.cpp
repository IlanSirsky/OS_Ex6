#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// https://www.geeksforgeeks.org/reentrant-function/
// need to answer question in pdf (strtok() and strtok_r())


using namespace std;

void * ptr;
pthread_mutex_t lock  = PTHREAD_MUTEX_INITIALIZER;
bool sleepFlag = true;

class Guard {
public:
    Guard() {
        pthread_mutex_lock(&lock);
        cout << "Guard: Lock locked" << endl;
    }

    ~Guard() {
        pthread_mutex_unlock(&lock);
        cout << "Guard: Lock unlocked" << endl;
    }
};

void* updatePtr(void * newPtr){
    Guard guard;
    if (sleepFlag)
    {
        sleepFlag = false;
        sleep(2);
    }
    ptr = newPtr;
    int * temp = (int*)ptr;
    cout << *temp << endl;
    return NULL;
}

int main(){
    int a, b;
    cout << "Enter two numbers: ";
    cin >> a >> b;
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, &updatePtr, &a);
    pthread_create(&thread2, NULL, &updatePtr, &b);
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    return 0;
}