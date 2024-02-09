#ifndef __H_COMMON__
#define __H_COMMON__

int doDebug=0;

#ifdef LOG_SYS
#undef LOG_SYS
#endif

#ifdef WINDOWS
#define LOG_SYS(...) printf(__VA_ARGS__)
#else
#define LOG_SYS(...)    if (doDebug) \
			printf(__VA_ARGS__);\
			else \
			syslog(LOG_LOCAL7 | LOG_INFO,__VA_ARGS__)
#endif

#endif /*__H_COMMON__*/

