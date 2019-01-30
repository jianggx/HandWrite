#ifndef __LOG_H__
#define __LOG_H__

#include <sstream>
#include <string>
#include <cstdio>

#define DISALLOW_EVIL_CONSTRUCTORS(TypeName) \
	TypeName(const TypeName&);  \
	TypeName& operator=(const TypeName&)


/////////////////////////////////////////////////////////////////////////////
// ThreadMutex
//
class ThreadMutex
{
public:
	ThreadMutex(){
#ifdef WIN32
		InitializeCriticalSection(&m_mutex);
#endif

#ifdef UNIX
		pthread_mutexattr_t mutexattr;
		pthread_mutexattr_init(&mutexattr);
		pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
		int ret_val = pthread_mutex_init(&m_mutex, &mutexattr);
		pthread_mutexattr_destroy(&mutexattr);
		MCS_ASSERT(ret_val == 0);
#endif


	};
	~ThreadMutex()
	{

#ifdef WIN32
		DeleteCriticalSection(&m_mutex);
#endif

#ifdef UNIX
		int ret_val = pthread_mutex_destroy(&m_mutex);
		MCS_ASSERT(ret_val == 0);
#endif

	};

	int   acquire()

	{
#ifdef WIN32
		EnterCriticalSection(&m_mutex);
		return 0;
#endif

#ifdef UNIX
		int ret_val = pthread_mutex_lock(&m_mutex);
		MCS_ASSERT(ret_val == 0);

		if (ret_val == 0) return 0;
		return -1;
#endif

	};
	int   try_acquire(){

#ifdef WIN32
		return TryEnterCriticalSection(&m_mutex);
#endif

#ifdef UNIX
		int err = pthread_mutex_trylock(&m_mutex);
		if (err == 0) return 0;
		return -1;
#endif


	};

	int   release(){
#ifdef WIN32
		LeaveCriticalSection(&m_mutex);
		return 0;
#endif

#ifdef UNIX
		pthread_mutex_unlock(&m_mutex);
		return 0;
#endif
	};

protected:
	DISALLOW_EVIL_CONSTRUCTORS(ThreadMutex);

protected :
#if defined(WIN32)
	CRITICAL_SECTION	m_mutex;
#endif

#if defined(UNIX)
	pthread_mutex_t		m_mutex;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// ThreadMutex




/////////////////////////////////////////////////////////////////////////////
// T120_Lock
class T120_Lock
{
public :
	T120_Lock(ThreadMutex &cs):cs_reference(cs)
	{
		cs_reference.acquire();
	};
	virtual ~T120_Lock(){	cs_reference.release();};

protected :
	ThreadMutex& cs_reference;
};


#pragma warning(disable: 4995 )

inline std::string nowTime();

enum TLogLevel { logERROR = 0, logWARNING, logINFO, logDEBUG };

extern ThreadMutex  g_Log_Mutex[];

template <typename T>
class Log
{
public:
	Log();
	virtual ~Log();
	Log<T> & Get(TLogLevel level = logINFO);

	Log<T> & operator << (double value);
	Log<T> & operator << (float value);
	Log<T> & operator << (int value);
	Log<T> & operator << (DWORD value);
	Log<T> & operator << (long long value);
	Log<T> & operator << (const char * value);
	Log<T> & operator << (const std::string & value);

public:
	static TLogLevel& ReportingLevel();
	static std::string ToString(TLogLevel level);
	static TLogLevel FromString(const std::string& level);

protected:
	std::ostringstream		os_;
	ThreadMutex *           mutex_;

private:
	Log(const Log&);
	Log& operator =(const Log&);
};

template <typename T>
Log<T>::Log()
{
	mutex_ = NULL;
}

template <typename T>
Log<T> & Log<T>::Get(TLogLevel level)
{
	mutex_ = &g_Log_Mutex[level];
	T120_Lock lck(*mutex_);

	os_ << "- " << nowTime();
	os_ << " " << ToString(level) << ": ";
	os_ << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');

	return *this;
}

template <typename T>
Log<T> & Log<T>::operator << (double value)
{
	if (!mutex_) return *this;
	T120_Lock lck(*mutex_);

	os_ << value;
	return *this;
}

template <typename T>
Log<T> & Log<T>::operator << (float value)
{
	if (!mutex_) return *this;
	T120_Lock lck(*mutex_);

	os_ << value;
	return *this;
}

template <typename T>
Log<T> & Log<T>::operator << (int value)
{
	if (!mutex_) return *this;
	T120_Lock lck(*mutex_);

	os_ << value;
	return *this;
}

template <typename T>
Log<T> & Log<T>::operator << (long long value)
{
	if (!mutex_) return *this;
	T120_Lock lck(*mutex_);

	os_ << value;
	return *this;
}

template <typename T>
Log<T> & Log<T>::operator << (const char * value)
{
	if (!mutex_) return *this;
	T120_Lock lck(*mutex_);

	if (value)
		os_ << value;
	else
		os_ << "NULL";
	return *this;
}


template <typename T>
Log<T> & Log<T>::operator << (const std::string & value)
{
	if (!mutex_) return *this;
	T120_Lock lck(*mutex_);

	os_ << value;
	return *this;
}

template <typename T>
Log<T> & Log<T>::operator << (DWORD value)
{
	if (!mutex_) return *this;
	T120_Lock lck(*mutex_);

	os_ << value;
	return *this;
}

template <typename T>
Log<T>::~Log()
{
	if (!mutex_) return;
	T120_Lock lck(*mutex_);

	os_ << "\r\n";
	T::Output(os_.str());
}

template <typename T>
TLogLevel& Log<T>::ReportingLevel()
{
	static TLogLevel reportingLevel = logDEBUG;
	return reportingLevel;
}

template <typename T>
std::string Log<T>::ToString(TLogLevel level)
{
	static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG"};
	return buffer[level];
}

template <typename T>
TLogLevel Log<T>::FromString(const std::string& level)
{
	if (level == "DEBUG")
		return logDEBUG;
	if (level == "INFO")
		return logINFO;
	if (level == "WARNING")
		return logWARNING;
	if (level == "ERROR")
		return logERROR;

	Log<T>().Get(logWARNING) << "Unknown logging level '" << level << "'. Using INFO level as default.";

	return logINFO;
}


class Output2FILE
{
public:
	static FILE*& Stream();

	static void Output(const std::string& msg);
};

inline FILE*& Output2FILE::Stream()
{
	static FILE* pStream = stderr;
	return pStream;
}

inline void Output2FILE::Output(const std::string& msg)
{   
	FILE* pStream = Stream();
	if (!pStream)
		return;

	fprintf(pStream, "%s", msg.c_str());
	fflush(pStream);
}



#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   if defined (BUILDING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllexport)
#   elif defined (USING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllimport)
#   else
#       define FILELOG_DECLSPEC
#   endif // BUILDING_DBSIMPLE_DLL
#else
#   define FILELOG_DECLSPEC
#endif // _WIN32

class FILELOG_DECLSPEC FILELog : public Log<Output2FILE> {};


#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL logDEBUG
#endif

#define FILE_LOG(level) \
	if (level > FILELOG_MAX_LEVEL) ;\
	else if (level > FILELog::ReportingLevel() || !Output2FILE::Stream()) ; \
	else FILELog().Get(level)

class Logs
{
public:
	static void logInfo(const std::string& info);//记录info日志并且打印到屏幕中
	static void logError(const std::string& info);//记录error日志并且打印到屏幕中
};


inline void  Logs::logInfo(const std::string& info)
{
	FILE_LOG(logINFO)<<info;
	fprintf(stdout, info.c_str());
}

inline void  Logs::logError(const std::string& info)
{
	FILE_LOG(logERROR)<<info;
	fprintf(stderr, info.c_str());
}


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <winsock2.h>  
#include <time.h>  
#include <windows.h>
#include <chrono>
#include <time.h>
#include <sys/timeb.h>

#pragma comment(lib,"setupapi.lib") 
inline long long NowTime()
{
	auto tp = std::chrono::system_clock::now().time_since_epoch();
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp);
	return ms.count();
}

inline int gettimeofday(struct timeval *tv, void *tz)
{
	struct _timeb timebuffer;
	_ftime(&timebuffer);

	tv->tv_sec = timebuffer.time;
	//这个存储的是剩余的微秒数 不是全部毫秒 存不下这么大 add by changliu
	tv->tv_usec = (long)timebuffer.millitm * 1000;

	return 0;
}


inline std::string nowTime()
{
	const static ULONGLONG starttime = (ULONGLONG)NowTime();

	const int MAX_LEN = 200;
	char buffer[MAX_LEN];
	if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
		"HH':'mm':'ss", buffer, MAX_LEN) == 0)
		return "Error in nowTime()";


	struct timeval tv;

	gettimeofday(&tv, 0);
	char result[100] = { 0 };
	_snprintf(result, sizeof(result), "%s.%ld t:%I64d tid: %lu", buffer, tv.tv_usec / 1000, NowTime() - starttime, ::GetCurrentThreadId());

	return result;
}

#else

#include <sys/time.h>

inline std::string NowTime()
{
	char buffer[11];
	time_t t;
	time(&t);
	tm r = {0};
	strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
	struct timeval tv;
	gettimeofday(&tv, 0);
	char result[100] = {0};
	std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000); 
	return result;
}

#endif //WIN32

#endif //__LOG_H__
