#include <mutex>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

pthread_mutex_t gmtx;
template <typename T>
class Singleton {
    private:
        static Singleton* instance;
        T data;
        mutex mtx;
        Singleton (T val){
            this->data = val;
            mtx.lock();
        }

    public:
        static Singleton<T> * getInstance(T val) {
            if (instance == nullptr) {
                pthread_mutex_lock(&gmtx);
                instance = new Singleton(val);
            }
            pthread_mutex_unlock(&gmtx);
            return instance;
        }
        
        void destroy() {
            instance = nullptr;
            mtx.unlock();
        }

};

template<typename T>
Singleton<T> * Singleton<T>::instance = nullptr;

int main(){
    FILE *fptr; 
    fptr = fopen("test.txt", "r");
    Singleton<FILE*>* singleton1 = Singleton<FILE*>::getInstance(fptr);
    Singleton<FILE*>* singleton2 = Singleton<FILE*>::getInstance(fptr);
    cout << "singleton1: "<< singleton1 << endl;
    cout << "singleton2: "<< singleton2 << endl;
    return 0;
}
