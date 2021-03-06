
#include "sts_thread.h"
#include <sts_time.h>
#include <assert.h>

bool sts_thread_create(THREAD_START_ROUTINE func, void* var, unsigned long *thread)
{
#ifdef _MSC_VER
	uintptr_t result = 0;
	unsigned int id = 0;
	result = _beginthreadex(NULL, 0, &func, var, 0, &id);
	if (result == 0)
	{
		return false;
	}
	else
	{
		CloseHandle((HANDLE)result);
	}
#else
	pthread_t result = 0;
	pthread_attr_t attr;
	int irc;
	irc = pthread_attr_init(&attr); 
	if(irc) { return false; }
	irc = pthread_attr_setstacksize(&attr, 1024 * 64);//测试2008-07-15
	if(irc) { return false; }
	irc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(irc) { return false; }
	irc = pthread_create(&result, &attr, func, var);
	if(irc) { return false; }
	pthread_attr_destroy(&attr);
#endif
	*thread = (unsigned long)result;
	return true;
}

void sts_thread_join(unsigned thread)
{

#ifdef _MSC_VER
	WaitForSingleObject((void*)thread, INFINITE);
	CloseHandle((HANDLE)thread);
#else
	pthread_join((pthread_t)thread, 0);
#endif
}

void sts_thread_clear(unsigned thread)
{
#ifndef _MSC_VER
	pthread_t t = (pthread_t)thread;
	pthread_detach(t);
#endif

}

unsigned sts_thread_self()
{
#ifdef _MSC_VER
	return (unsigned)GetCurrentThreadId();
#else
	return (unsigned)pthread_self();
#endif
}

/////////////////////////////////////
//
//////////////////////////////////////////
int sts_mutex_create(s_sts_thread_mutex_t *m)
{
#ifdef _MSC_VER
	InitializeCriticalSection(m);
	return 0;
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	// 设置允许递归加锁(默认不支持)
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	return pthread_mutex_init(m, &attr);
#endif
}
void sts_mutex_destroy(s_sts_thread_mutex_t *m)
{
	assert(m);
#ifdef _MSC_VER
	DeleteCriticalSection(m);
#else
	pthread_mutex_destroy(m);
#endif
}
void sts_mutex_lock(s_sts_thread_mutex_t *m)
{
	assert(m);
	pthread_mutex_lock(m);
}
void sts_mutex_unlock(s_sts_thread_mutex_t *m)
{
	assert(m);
	pthread_mutex_unlock(m);
}
////////////////////////
// 多读一写锁定义
////////////////////////
int sts_mutex_rw_create(s_sts_thread_mutex_rw *m)
{
	int rtn = sts_mutex_create(&m->mutex_s);
	m->try_write_b = false;
	m->reads_i = 0;
	m->writes_i = 0;
	return rtn;
}
void sts_mutex_rw_destroy(s_sts_thread_mutex_rw *m)
{
	assert(m);
	sts_mutex_destroy(&m->mutex_s);
}
void sts_mutex_rw_lock_r(s_sts_thread_mutex_rw *m)
{
	assert(m);
	for (;;)
	{
		sts_mutex_lock(&m->mutex_s);
		if (m->try_write_b || m->writes_i > 0)
		{
			sts_mutex_unlock(&m->mutex_s);
#ifdef _MSC_VER
			Sleep(50);
#else
			usleep(50 * 1000);
#endif
			continue;
		}

		assert(m->reads_i >= 0);
		++m->reads_i;
		sts_mutex_unlock(&m->mutex_s);
		break;
	}
}
void sts_mutex_rw_unlock_r(s_sts_thread_mutex_rw *m)
{
	assert(m);
	sts_mutex_lock(&m->mutex_s);
	--m->reads_i;
	assert(m->reads_i >= 0);
	sts_mutex_unlock(&m->mutex_s);
}
void sts_mutex_rw_lock_w(s_sts_thread_mutex_rw *m)
{
	for (;;)
	{
		sts_mutex_lock(&m->mutex_s);
		m->try_write_b = true;
		if (m->reads_i > 0 || m->writes_i > 0)
		{
			sts_mutex_unlock(&m->mutex_s);
#ifdef _MSC_VER
			Sleep(50);
#else
			usleep(50 * 1000);
#endif
			continue;
		}
		m->try_write_b = false;
		assert(m->writes_i >= 0);
		++m->writes_i;
		sts_mutex_unlock(&m->mutex_s);
		break;
	}
}
void sts_mutex_rw_unlock_w(s_sts_thread_mutex_rw *m)
{
	assert(m);
	sts_mutex_lock(&m->mutex_s);
	--m->writes_i;
	assert(m->writes_i >= 0);
	sts_mutex_unlock(&m->mutex_s);
}
