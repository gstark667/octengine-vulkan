#ifndef H_EXPORT
#define H_EXPORT

#ifdef _WIN32
#define EXPORT __stdcall __declspec(dllexport)
#else
#define EXPORT
#endif

#endif
