/**
 * @brief Common utilities
 * @author Batto1
 * @date of last modification: 31.01.2024
*/







#ifndef USER_COMMON_H
#define USER_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Related to workaround for order of macro expansion in C.
 * If you only use this, special macros like __COUNTER__, __LINE__, or __FILE__ get treated as literal string rather than the value they're
 * representing. To work around this, additional level of indirection by introducing another macro layer can be used. 
 * */
#define     _u_STRINGIFY(x) #x  /* @internal  you don't use this */
#define     u_STRINGIFY(x) _u_STRINGIFY(x) 

/* Macro expansion workaround */
// #define _u_MACRO_EXPANSION_WA(x) x
// #define u_MACRO_EXPANSION_WA(x) _u_MACRO_EXPANSION_WA(x)

/* Macro expansion workaround with concatenate*/
#define _u_CONCATENATE(x, y) x ## y
#define u_CONCATENATE(x, y) _u_CONCATENATE(x, y)


/* For creating unique names, concatenates __LINE__ and __COUNTER__ macros. */
#define LINE_AND_COUNTER u_CONCATENATE(u_CONCATENATE(__LINE__, _), __COUNTER__ )








#ifdef __cplusplus
}
#endif

#endif // END: USER_COMMON_H
