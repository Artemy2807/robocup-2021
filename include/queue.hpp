#pragma once
#include <pthread.h>
#include <unistd.h>

template<typename T>
class Object {
public:
	Object();

    T read();
    write(const T& src);
    
private:
    T obj;
	pthread_mutex_t lock_;
};

template<typename T>
class Queue {	
public:
	Queue();
    
	void push(Object<T>* obj);
	Object<T>* wait(Object<T>* cur_obj);
    
private:
	Object<T>* obj_;
    pthread_mutex_t lock_;
    
};

#include "queue.cpp"
