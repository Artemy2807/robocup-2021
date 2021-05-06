#pragma once
#include <pthread.h>
#include <unistd.h>

template<typename T>
class Atomic {
public:
    Atomic() {
        lock_ = PTHREAD_MUTEX_INITIALIZER;
    }

    void write(const T& src) {
        pthread_mutex_lock(&(lock_));
        obj_ = src;
        pthread_mutex_unlock(&(lock_));
    }

    T read() {
        T tmp;
        pthread_mutex_lock(&(lock_));
        tmp = obj_;
        pthread_mutex_unlock(&(lock_));
        return tmp;
    }

private:
    T obj_;
	pthread_mutex_t lock_;
};

template<typename T>
class Object {
public:
	T *obj;
	pthread_mutex_t lock;
	int count;
	
    Object() {
        lock = PTHREAD_MUTEX_INITIALIZER;
        obj = new T();
        count = 0;
    }

    ~Object() { delete obj; }

    void free() {
        if(count == 1) {
            delete this;
            return;
        }
        pthread_mutex_lock(&(lock));
        count--;
        pthread_mutex_unlock(&(lock));
        return;
    }

    void busy() {
        pthread_mutex_lock(&(lock));
        count++;
        pthread_mutex_unlock(&(lock));
        return;
    }

};

template<typename T>
class Queue {	
public:
    Queue() {
        obj_ = nullptr;
        lock_ = PTHREAD_MUTEX_INITIALIZER;
    }

    void push(Object<T>* obj) {
        pthread_mutex_lock(&(lock_));
        if(obj_ != nullptr) obj_->free();
        obj_ = obj;
        pthread_mutex_unlock(&(lock_));
    }

    Object<T>* wait(Object<T>* cur_obj) {
        Object<T>* old_obj = cur_obj;
        int count = 0;
        while(1) {
            pthread_mutex_lock(&(lock_));
            cur_obj = obj_;
            pthread_mutex_unlock(&(lock_));
            if((cur_obj == NULL || old_obj == cur_obj) || count < 100) {
                usleep(1000);
                count++;
            }else {
                cur_obj->busy();
                break;
            }
        }
        return cur_obj;
    }
    
private:
	Object<T>* obj_;
    pthread_mutex_t lock_;
    
};
