/**
 * @author Batto1
 * @brief RAM Retention API for nRF52 series SoCs (can be implemented to other chips if proper modification is made)
 * @date 	Last version date: 01.02.2024
 * 
 * @note following Kconfig options must be enabled for utilizing ram retention:
	CONFIG_APP_RETENTION=y
	CONFIG_CRC=y
 * @note following Kconfig options must be configured for utilizing logging:
	CONFIG_LOG=y
	CONFIG_LOG_BACKEND_SHOW_COLOR=y  (optional)
	CONFIG_LOG_MODE_MINIMAL=n	   (optional)
 * @note when macros involving logging are used, you also need to include necessary library and log registration in the file where the macro is used. Use:
 	#include <zephyr/logging/log.h>
	LOG_MODULE_REGISTER(foo, CONFIG_LOG_DEFAULT_LEVEL); // make configurations to log registration macro as needed.
 */


#ifndef RAM_RETENTION_UTILS_H
#define RAM_RETENTION_UTILS_H



#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include "sys_init_utils.h"

	



/**
 *  @brief Declare a composite struct type that is used for defining ram retention variables on top of existing types.
 *  @param [in] var_type type of the variable (i.e. int, i.e. struct foo) that will be ram ratained.
 *  @param [in] typedef_name name given to the typedef created for the ram retention variable.
 *  @note  user has to put semicolon (;) themself.
 *  @recommended: Start @param typedef_name with "RamRet" prefix for having naming compability with the other functions in this library. i.e. RamRetFoo
 *  @example  typical use looks like: 
			struct  my_struct{ // use of a struct is not mandatory, any type is valid.
				int my_var;
			};
			RamRetTypeDeclare(struct my_struct, RamRetMyType);
			RR_Init_Var_Ram_Retention(RamRetMyType, g_rr_foo);  // or if you just want to defie a variable use: RamRetVarDefine(RamRetMyType, g_rr_foo);
*/
#define RamRetTypeDeclare(var_type, typedef_name) 	\
	typedef struct _##typedef_name{				\
		var_type	rr_var;						\
		uint32_t 	crc;							\
	}typedef_name



/**
 * @brief INTERNAL. needed for using with FOR_EACH_FIXED_ARG
*/
#define _RamRetVarDefine(var_name, var_type) __noinit var_type var_name // needed for using with FOR_EACH_FIXED_ARG

/**
 *  @brief Define a __noninit variable with given variable type and variable name. 
 *  @note  For primitive data types, you can alternatively use the other macros created for each specific type i.e. RamRetUint32t_
 *  @param [in] var_type type of the variable i.e. int
 *  @param [in] var_name given name for the variable
 *  @example  <RamRetVar(int, foo);> corresponds to <__noinit int foo>
*/
#define RamRetVarDefine(var_type, var_name) __noinit var_type var_name

/**
 *  @brief Similar to the macro RamRetVarDefine(var_type, var_name) but you can input many variable names as arguments.
 *  @param [in] var_type type of the variable i.e. int
 *  @param [in] ... name of the variables, separated using commas.
 *  @example  <RamRetDefineVars(int, var1, var2, var3);> corresponds to <__noinit int var1; __noinit int var2; __noinit int var3;>
*/
#define RamRetDefineVars(var_type, ...)  			\
	 FOR_EACH_FIXED_ARG(_RamRetVarDefine, (;), var_type, __VA_ARGS__) 



/**
 * @brief INTERNAL. needed for using with FOR_EACH_FIXED_ARG
*/
#define _RamRetVarExtern(var_name, var_type) extern var_type var_name 

/**
 *  @brief Declare an extern variable of given type.
 *  @note  You can use RamRetVarDefine(var_type, var_name) macro to define a variable.
 *  @note  After defining a variable, you need to make extern declaration on some other file that you want to access the variable. Or alternatively, make the extern declaration in some header file and include that header file in any other file that you want to access the defined variable.
 *  @param [in] var_type type of the variable i.e. int
 *  @param [in] var_name variable's name
 *  @example  <RamRetVarExtern(int, foo);> corresponds to <extern int foo>
*/
#define RamRetVarExtern(var_type, var_name) extern var_type var_name


/**
 *  @brief Similar to the macro RamRetVarExtern(var_type, var_name) but you can input many variable name as arguments.
 *  @note  After defining a variable, you need to make extern declaration on some other file that you want to access the variable. Or alternatively, make the extern declaration in some header file and include that header file in any other file that you want to access the defined variable.
 *  @param [in] var_type type of the variable i.e. int
 *  @param [in] ... name of the variables, separated using commas.
 *  @example  <RamRetExternVar(int, var1, var2, var3);> corresponds to <extern int var1; extern int var2; extern int var3;>
*/
#define RamRetExternVars(var_type, ...)  			\
	 FOR_EACH_FIXED_ARG(_RamRetVarExtern, (;), var_type, __VA_ARGS__) 
  	


/* ram retention struct type declarations for primitive or some other data types and some helper macros associated with them */
/* To be more concise, some of the data types are declared with the help of macros and other are explicitly, to show both declarations */



/* int ram retention struct type declaration and helper macros */
/** 
 * @brief int user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(int, RamRetInt);

/**
 * @brief 	Macro for user user convenience when defining __noinit variable of type RamRetInt
 * @example 	RamRetInt_t foo;
 */
#define RamRetInt_t __noinit RamRetInt

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetInt
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineInt(var1, var2, var3);
 */
#define RamRetDefineInt(...) RamRetDefineVars(RamRetInt, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetInt
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternInt(var1, var2, var3);
 */
#define RamRetExternInt(...) RamRetExternVars(RamRetInt, __VA_ARGS__)





/* flaot ram retention struct type declaration and helper macros */
/** 
 * @brief float user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(float, RamRetFloat);

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetFloat
 * @example RamRetFloat_t foo;
 */
#define RamRetFloat_t __noinit RamRetFloat

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetFloat
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineFloat(var1, var2, var3);
 */
#define RamRetDefineFloat(...) RamRetDefineVars(RamRetFloat, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetFloat
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternFloat(var1, var2, var3);
 */
#define RamRetExternFloat(...) RamRetExternVars(RamRetFloat, __VA_ARGS__)





/* double ram retention struct type declaration and helper macros */
/** 
 * @brief double user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(double, RamRetDouble);

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetDouble
 * @example RamRetDouble_t foo;
 */
#define RamRetDouble_t __noinit RamRetDouble

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetDouble
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineDouble(var1, var2, var3);
 */
#define RamRetDefineDouble(...) RamRetDefineVars(RamRetDouble, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetDouble
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternDouble(var1, var2, var3);
 */
#define RamRetExternDouble(...) RamRetExternVars(RamRetDouble, __VA_ARGS__)





/* uint8_t ram retention struct type declaration and helper macros */
/** 
 * @brief uint8_t user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(uint8_t, RamRetUint8t);

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetUint8t
 * @example RamRetUint8_t foo;
 */
#define RamRetUint8_t __noinit RamRetUint8t

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetUint8t
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineUint8t(var1, var2, var3);
 */
#define RamRetDefineUint8t(...) RamRetDefineVars(RamRetUint8t, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetUint8t
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternUint8t(var1, var2, var3);
 */
#define RamRetExternUint8t(...) RamRetExternVars(RamRetUint8t, __VA_ARGS__)





/* uint16_t ram retention struct type declaration and helper macros */
/** 
 * @brief uint16_t user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(uint16_t, RamRetUint16t);

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetUint16t
 * @example RamRetUint16_t foo;
 */
#define RamRetUint16_t __noinit RamRetUint16t

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetUint16t
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineUint16t(var1, var2, var3);
 */
#define RamRetDefineUint16t(...) RamRetDefineVars(RamRetUint16t, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetUint16t
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternUint16t(var1, var2, var3);
 */
#define RamRetExternUint16t(...) RamRetExternVars(RamRetUint16t, __VA_ARGS__)





/* uint32_t ram retention struct type declaration and helper macros */
/** 
 * @brief uint32_t user variable definition to be used with ram retention.
*/
struct ramRetUint32t{
	uint32_t 	rr_var;
     uint32_t 	crc;
};
typedef struct ramRetUint32t RamRetUint32t;

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetUint32t
 * @example RamRetUint32_t foo;
 */
#define RamRetUint32_t __noinit RamRetUint32t

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetUint32t
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineUint32t(var1, var2, var3);
 */
#define RamRetDefineUint32t(...) RamRetDefineVars(RamRetUint32t, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetUint32t
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternUint32t(var1, var2, var3);
 */
#define RamRetExternUint32t(...) RamRetExternVars(RamRetUint32t, __VA_ARGS__)





/* uint64_t ram retention struct type declaration and helper macros */
/** 
 * @brief uint64_t user variable definition to be used with ram retention.
*/
typedef struct ramRetUint64t{
	uint64_t	rr_var;
	uint32_t 	crc;
}RamRetUint64t;

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetUint64t
 * @example RamRetUint64_t foo;
 */
#define RamRetUint64_t __noinit RamRetUint64t

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetUint64t
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineUint64t(var1, var2, var3);
 */
#define RamRetDefineUint64t(...) RamRetDefineVars(RamRetUint64t, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetUint64t
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternUint64t(var1, var2, var3);
 */
#define RamRetExternUint64t(...) RamRetExternVars(RamRetUint64t, __VA_ARGS__)





/* int8_t ram retention struct type declaration and helper macros */
/** 
 * @brief int8_t user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(int8_t, RamRetInt8t);

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetInt8t
 * @example RamRetInt8_t foo;
 */
#define RamRetInt8_t __noinit RamRetInt8t

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetInt8t
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineInt8t(var1, var2, var3);
 */
#define RamRetDefineInt8t(...) RamRetDefineVars(RamRetInt8t, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetInt8t
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternInt8t(var1, var2, var3);
 */
#define RamRetExternInt8t(...) RamRetExternVars(RamRetInt8t, __VA_ARGS__)





/* int16_t ram retention struct type declaration and helper macros */
/** 
 * @brief int16_t user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(int16_t, RamRetInt16t);

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetInt16t
 * @example RamRetInt16_t foo;
 */
#define RamRetInt16_t __noinit RamRetInt16t

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetInt16t
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineInt16t(var1, var2, var3);
 */
#define RamRetDefineInt16t(...) RamRetDefineVars(RamRetInt16t, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetInt16t
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternInt16t(var1, var2, var3);
 */
#define RamRetExternInt16t(...) RamRetExternVars(RamRetInt16t, __VA_ARGS__)





/* int32_t ram retention struct type declaration and helper macros */
/** 
 * @brief int32_t user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(int32_t, RamRetInt32t);

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetInt32t
 * @example RamRetInt32_t foo;
 */
#define RamRetInt32_t __noinit RamRetInt32t

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetInt32t
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineInt32t(var1, var2, var3);
 */
#define RamRetDefineInt32t(...) RamRetDefineVars(RamRetInt32t, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetInt32t
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternInt32t(var1, var2, var3);
 */
#define RamRetExternInt32t(...) RamRetExternVars(RamRetInt32t, __VA_ARGS__)





/* int64_t ram retention struct type declaration and helper macros */
/** 
 * @brief int64_t user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(int64_t, RamRetInt64t);

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetInt64t
 * @example RamRetInt64_t foo;
 */
#define RamRetInt64_t __noinit RamRetInt64t

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetInt64t
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineInt64t(var1, var2, var3);
 */
#define RamRetDefineInt64t(...) RamRetDefineVars(RamRetInt64t, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetInt64t
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternInt64t(var1, var2, var3);
 */
#define RamRetExternInt64t(...) RamRetExternVars(RamRetInt64t, __VA_ARGS__)





/* atomic_t ram retention struct type declaration and helper macros */
/** 
 * @brief atomic_t user variable definition to be used with ram retention.
*/
RamRetTypeDeclare(atomic_t, RamRetAtomic);

/**
 * @brief Macro for user user convenience when defining __noinit variable of type RamRetAtomic
 * @example RamRetAtomic_t foo;
 */
#define RamRetAtomic_t __noinit RamRetAtomic

/**
 * @brief 	Macro for user user convenience when defining multiple __noinit variable of type RamRetAtomic
 * @param [in] ... variable names to be defined.
 * @example 	RamRetDefineAtomic(var1, var2, var3);
 */
#define RamRetDefineAtomic(...) RamRetDefineVars(RamRetAtomic, __VA_ARGS__)

/**
 * @brief 	Macro for user user convenience when externing multiple variables of type RamRetAtomic
 * @param [in] ... variable names to be externed.
 * @example 	RamRetExternAtomic(var1, var2, var3);
 */
#define RamRetExternAtomic(...) RamRetExternVars(RamRetAtomic, __VA_ARGS__)





/**
 * @brief Define and initialize a ram retention variable. This macro is created so that definition and initialization can be done with 1 line of code. 
 * @note This macro utilizes SYS_INIT infrastructure internally and creates a unique initialization function for every call for the macro.
 * @note If need arises that another type of variable is used and that type isn't readily available, it first must be declared using RamRetTypeDeclare(var_type, typedef_name) macro.
 * @note If a error occurs at initialization, logging must be enabled and registered at the caller file to see the error message.
 * @param [in] rr_var_type type of the variable i.e. RamRetInt
 * @param [in] rr_var_name given name for the variable
*/
#define RR_Init_Var_Ram_Retention(rr_var_type, rr_var_name) 	\
			RamRetVarDefine(rr_var_type, rr_var_name);		\
			SYS_INIT_SIMPLE(APPLICATION, 30)				\
			{										\
				bool is_valid = ram_retained_validate(&rr_var_name, (size_t)sizeof(rr_var_name), (size_t)offsetof(rr_var_type, crc), (size_t)sizeof(rr_var_name.crc));			 	\
				if(false == is_valid){					\
					LOG_ERR("Ram ret initialization error at SYS_INIT of variable <%s %s>", #rr_var_type, #rr_var_name); 	\
				}									\
				return 0;								\
			}			





/**
 * @brief Similar to RR_Init_Var_Ram_Retention(rr_var_type, rr_var_name). Define and initialize a ram retention variable and also configure SYS_INIT parameters. 
 * @note  check RR_Init_Var_Ram_Retention(rr_var_type, rr_var_name) macro for additional notes.
 * @note  If a error occurs at initialization, logging must be enabled and registered at the caller file to see the error message.
 * @param [in] rr_var_type type of the variable i.e. RamRetInt
 * @param [in] rr_var_name given name for the variable
 * @param [in] level      Initialization level of SYS_INIT()       @defgroup   initialization_levels 	at sys_init_utils.h	
 * @param [in] prio       Initialization priority of SYS_INIT()	  @defgroup   initialization_priority 	at sys_init_utils.h	
*/
#define RR_Init_Var_Ram_Retention_Conf(rr_var_type, rr_var_name, level, prio) 	\
			RamRetVarDefine(rr_var_type, rr_var_name);		\
			SYS_INIT_SIMPLE(level, prio)					\
			{										\
				bool is_valid = ram_retained_validate(&rr_var_name, (size_t)sizeof(rr_var_name), (size_t)offsetof(rr_var_type, crc), (size_t)sizeof(rr_var_name.crc));			 \
				if(false == is_valid){					\
					LOG_ERR("Ram ret initialization error at SYS_INIT of variable <%s %s>", #rr_var_type, #rr_var_name); 	\
				}									\
				return 0;								\
			}	



/**
 * @brief After operating on some ram retention variable in any way that would cause it to change, you need to call void RR_Var_Ram_Retain(void * a_p_retained_var, size_t a_retained_crc_offset) function. But this macro can be used instead in place of the function if more convenient, since the parameters it takes is more straightforward.
 * @param [in] rr_var_type type of the variable i.e. RamRetInt
 * @param [in] rr_var_addr address of the ram retention type variable
*/
#define RR_Var_Ram_Ret(rr_var_type, rr_var_addr) 							\
		RR_Var_Ram_Retain(rr_var_addr, (size_t)offsetof(rr_var_type, crc)) 



/**
 * @brief For initialization of a RAM retained variable you need to call RR_Init_Variable_Ram_Retention(void * a_retained_var_ptr, size_t a_retained_var_size, size_t a_retained_crc_offset, size_t a_sizeof_retained_crc) function. But this macro can be used instead in place of the function if more convenient, since the parameters it takes is more straightforward.
 * @note if you'd like your variables to be start the program initialized, you can use RR_Init_Var_Ram_Retention() or RR_Init_Var_Ram_Retention_Conf() macros since they also initialize the variables with the use of SYS_INIT(). 
 * @param [in] rr_var_type type of the variable i.e. RamRetInt
 * @param [in] rr_var_name ram retention type variable's name.
 * @retval true if ram retain successful, false if not.
*/
#define RR_Var_Ram_Ret_Init(rr_var_type, rr_var_name) 							\
		ram_retained_validate(&rr_var_name, (size_t)sizeof(rr_var_name), (size_t)offsetof(rr_var_type, crc), (size_t)sizeof(rr_var_name.crc))





//*   FUNCTION PROTOTYPES   */

// only for testing purposes, user normally shouldn't try to use these
bool ram_retained_validate(void * a_var_ptr, size_t a_var_size, size_t a_retained_crc_offset, size_t a_sizeof_retained_crc );
void ram_retained_update(void * a_p_retained_var, size_t a_retained_crc_offset);
int ram_range_retain(const void *ptr, size_t len, bool enable); // doesn't need to be used by the user, only used for testing purposes


/**
 * @brief it is observed that if you don't use ram_retained_validate() before starting to retain and change the variable, variable gets garbage value.
 * 		This might be unwanted if this variable is used/printed before the time its modificated.
 * 		So this function simply inits the retaining process of that variable and resets it by calling this function.
 * @return true if valid, false if not.
*/
bool RR_Init_Variable_Ram_Retention(void * a_retained_var_ptr, size_t a_retained_var_size, size_t a_retained_crc_offset, size_t a_sizeof_retained_crc);


/**
 * @brief After operating on some ram retention variable in any way that would cause it to change, you need to call this function afterwards.
 * @param [in] a_retained_var_ptr pointer to the ram retained variable
 * @param [in] a_retained_crc_offset ram retention type variable's crc offset.
*/
inline void RR_Var_Ram_Retain(void * a_p_retained_var, size_t a_retained_crc_offset)
{
	ram_retained_update(a_p_retained_var, a_retained_crc_offset);
}

/**
 * @deprecated Don't need to be used since workaround exist and explained.
 * @brief Boolean conditions to be used when ram retention on boolean variables are used and configured. 
 * @problem: A ram ret variable is always 0 at first start (which corresponds to false if variable holds boolean variable). Then when variable need to be initialized to some other value by default (i.e. 8) you need to set this value at every initialization. But you might also want to set the variable to some other value other than its initialization value. So the solution is that at the initialization, you check if the value is equal or smaller than the initialization value, if it is, you initialize the value to default, if not, you know that it's set a different value than default and leave it as it is. So for the boolean values case, you can implement this workaround. Check the note.
 * @note You can completely avoid using this enum. This can be avoided by the init value 0 (logical false) respresenting the default value you want to set on variable so that initialization of variable won't be necessary. For example, if ram ret variable <is_enabled>'s default value should be 1 (true) instead of using this workaround, you can rename the variable to <is_disabled> (and also inverse the logic in the program) so that the first initialization and default value is 0 (false).Variable can be configured to true later while retaining this value without any problem. 
*/
enum RamRetBoolConditions{
     RR_ATOMIC_BOOL_DEFAULT_FALSE = 0, 				// not initialized, not configured (i.e. by user) value that corresponds to logical false when atomic variables will be used for logical operations involve persistent configuration.
	RR_ATOMIC_BOOL_DEFAULT_TRUE = 1, 				// initialized, not configured (i.e. by user) value that corresponds to logical true when atomic variables will be used for logical operations involve persistent configuration.
	IS_RR_ATOMIC_BOOL_AT_DEFAULT_VALUE_CHECK = 2, 	// if the variable's value is smaller than this value, it means that this ram retained atomic variable is at default value and was never configured since power up time.
	RR_ATOMIC_BOOL_CONFIGURED_FALSE = 3,			//  configured (i.e. by user) value that corresponds to logical false when atomic variables will be used for logical operations involve persistent configuration. Variable this value will be set shouldn't be reinitialized to default later if initialized properly.
	RR_ATOMIC_BOOL_CONFIGURED_TRUE = 4,			//  configured (i.e. by user) value that corresponds to logical true when atomic variables will be used for logical operations that involve persistent configuration. Variable this value will be set shouldn't be reinitialized to default later if initialized properly.
};	



#ifdef __cplusplus
}
#endif

#endif /* RETAINED_H_ */
