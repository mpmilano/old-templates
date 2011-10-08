#ifndef OXTHREAD_H
#define OXTHREAD_H
#include <pthread.h>
#include <functional>
#include <future.h>
#include <memory>

template< typename T>
class OxThread;

template< typename T>
class OxThread
{
public:

	static std::unique_ptr<Future<T>> create_and_start(std::function<T ()>& f){
		OxThread<T>* t = new OxThread<T>(f);
		return std::move(t->start());
	}
private:

OxThread(std::function<T ()>& f):run(f)
	{
		pthread_t p;
		pthread = &p;
	}

	std::unique_ptr<Future<T>> start(){
		pthread_create(pthread,NULL,trampoline,(void*) this);
		std::unique_ptr<Future<T>> retur(new Future<T>());
		future = retur.get();
		return std::move(retur);
	}
    virtual ~OxThread(){
		pthread_exit(pthread);
	}

private:
	
    static void* trampoline(void *v){
		((OxThread*)v)->ret = std::move(((OxThread*)v) ->run());
		if (((OxThread*)v)->future)
			((OxThread*)v)->future->result = std::move(((OxThread*)v)->ret);
		delete ((OxThread*)v);
		return v;
	}



    pthread_t *pthread;
    std::function<T () > &run;
	T ret;
	Future<T>* future;

};


template<>
class OxThread<void>
{

public:	

	static void create_and_start(std::function<void ()>& f){
		auto t = new OxThread<void>(f);
		t->start();
	}

private:


OxThread(std::function<void ()>& f):run(f)
	{
		pthread_t p;
		pthread = &p;
	}

   void start(){
		pthread_create(pthread,NULL,trampoline,(void*) this);
		
	}
    virtual ~OxThread(){
		pthread_exit(pthread);
	}
	
private:

    static void* trampoline(void *v){
		(((OxThread*)v) ->run());
		return v;
	}



    pthread_t *pthread;
    std::function<void () > &run;

};


/////////////////




#endif // OXTHREAD_H
