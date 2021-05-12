#pragma once

//#define __CV_TESTING__
//#define __LINE_TESTING__
#define __FULL_PROGRAM__

#if (defined(__CV_TESTING__) || defined(__FULL_PROGRAM__))
#   define __CV_INCLUDE__
#endif

#if defined(__FULL_PROGRAM__)
#	define __LINE_INCLUDE__     2
#	define __FULL_REACTION__
#elif defined(__LINE_TESTING__)
#	define __LINE_INCLUDE__     1
#endif
