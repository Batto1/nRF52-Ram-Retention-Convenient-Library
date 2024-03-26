/**
 * @brief       Built upon Zephyr Initialization infrastructure to simplify using system initialization and offer some related tools.
 * @author      Batto1
 * @date        of last modification: 26.03.2024
 * @warning     SYS_INIT is just some code working at the beginning of the main thread. Main thread exist whether user writes code block into it or not. Due to this, the priority and stacksize of main thread matters. For example, if a thread that is higher priority than main is started or signaled in one of the SYS_INIT functions, it will preempt main thread, thus the SYS_INIT tasks (if main isn't configured to be cooperative thread), so this situation should be considered.
 */


/**
 * @note Zephyr offers an infrastructure to call initialization code before main.
Such initialization calls can be registered using SYS_INIT() or SYS_INIT_NAMED() macros. By using a combination of initialization levels and priorities init sequence can be adjusted as needed. The available initialization levels are described, in order, below:

 * @defgroup  initialization_levels
        EARLY: Used very early in the boot process, right after entering the C domain (z_cstart()). This can be used in architectures and SoCs that extend or implement architecture code and use drivers or system services that have to be initialized before the Kernel calls any architecture specific initialization code.
        PRE_KERNEL_1: Executed in Kernel's initialization context, which uses the interrupt stack. At this point Kernel services are not yet available.
        PRE_KERNEL_2: Same as PRE_KERNEL_1.
        POST_KERNEL: Executed after Kernel is alive. From this point on, Kernel primitives can be used.
        APPLICATION: Executed just before application code (main).
        SMP: Only available if CONFIG_SMP is enabled, specific for SMP.

 * @defgroup initialization_priority
        can take a value in the range of 0 to 99. Note that it must be a decimal integer literal without leading zeroes or sign (e.g. 32), or must be an equivalent symbolic name (e.g. #define MY_INIT_PRIO 32). Symbolic expressions are not permitted (e.g. CONFIG_KERNEL_INIT_PRIORITY_DEFAULT + 5).
        Lower values correspond to higher priority.
*/

#ifndef SYS_INIT_UTILS_H
#define SYS_INIT_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include "user_common.h"        // under file named "common"


#define SYS_INIT_HIGHEST_PRIO 0   // sys init code with priority approaching to this values works earliest
#define SYS_INIT_LOWEST_PRIO  99  // sys init code with priority approaching to this values works latest

/**
 * @brief The kind of function pointer type that the SYS_INIT takes. You can define it as static.
*/
typedef int (*SYS_Init_Func_t)(const struct device *);


/**
 * @brief Simply establish a SYS_INIT code block. You don't need to provide a function name and must write the block of code composing with this macro (see @example).. This macro automatically creates a uniquely named initialization function.
 * @param   [in] level Initialization level      @defgroup   initialization_levels
 * @param   [in] prio  Initialization priority   @defgroup   initialization_priority
 * @warning The convenience of using this macro for easily defining SYS_INIT code blocks anywhere, rather than writing them to a common SYS_INIT function creates 16 byte overhead on the FLASH memory.
 * @note return function block with 0 to not get any warnings in the compilation phase.
 * @example 
            SYS_INIT_SIMPLE(APPLICATION, 50){
                printk("This will print before main!\n");
                return 0;
            }
*/
#define SYS_INIT_SIMPLE(level, prio)                 \
    static int u_CONCATENATE(__sys_init_func_, __LINE__ )(const struct device *dev);    \
    SYS_INIT(u_CONCATENATE(__sys_init_func_, __LINE__ ), level, prio);                  \
    static int u_CONCATENATE(__sys_init_func_, __LINE__ )(const struct device *dev)


/**
 * @brief Same macro as with the SYS_INIT_SIMPLE(level, prio) while only the naming is different for more convenient naming considerations and backwards compability. 
*/
#define SYS_INIT_EASY(level, prio) SYS_INIT_SIMPLE(level, prio)



/**
 * @brief Similar to SYS_INIT_SIMPLE: Simply establish a SYS_INIT code block where you also provide the function name. You must write the block of code composing with this macro (see @example). This macro automatically creates a initialization function with the user provided function name.
 * @param   [in] fn_name    Given name for the SYS_INIT() initialization function.
 * @param   [in] level      Initialization level      @defgroup   initialization_levels
 * @param   [in] prio       Initialization priority   @defgroup   initialization_priority
 * @warning The convenience of using this macro for easily defining SYS_INIT code blocks anywhere, rather than writing them to a common SYS_INIT function creates 16 byte overhead on the FLASH memory.
 * @note return function block with 0 to not get any warnings in the compilation phase.
 * @example 
            SYS_INIT_USER_NAMED(my_func, APPLICATION, 50)
            {
                printk("hi there\n");
                return 0;
            }
*/
#define SYS_INIT_USER_NAMED(fn_name, level, prio)        \
    static int u_CONCATENATE( , fn_name ) (const struct device *dev);     \
    SYS_INIT(u_CONCATENATE( , fn_name ), level, prio);                    \
    static int u_CONCATENATE( , fn_name ) (const struct device *dev)










#ifdef __cplusplus
}
#endif

#endif /* SYS_INIT_UTILS_H */
