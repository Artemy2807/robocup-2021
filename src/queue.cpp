template<typename T>
Object<T>::Object() {
	lock_ = PTHREAD_MUTEX_INITIALIZER;
}

template<typename T>
void Object<T>::write(const T& src) {
	pthread_mutex_lock(&(lock_));
	obj = src;
	pthread_mutex_unlock(&(lock_));
}

template<typename T>
T Object<T>::read() {
	T tmp;
	pthread_mutex_lock(&(lock_));
	tmp = obj;
	pthread_mutex_unlock(&(lock_));
	return tmp;
}

template<typename T>
Queue<T>::Queue() {
	obj_ = nullptr;
	lock_ = PTHREAD_MUTEX_INITIALIZER;
}

template<typename T>
void Queue<T>::push(Object<T>* obj) {
	pthread_mutex_lock(&(lock_));
	if(obj_ != nullptr) obj_->free();
	obj_ = obj;
	pthread_mutex_unlock(&(lock_));
}

template<typename T>
Object<T>* Queue<T>::waitForNewObject(Object<T>* cur_obj) {
	Object<T>* old_obj = cur_obj;
    int count = 0;
	while(1) {
		pthread_mutex_lock(&(lock_));
		cur_obj = _obj;
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
