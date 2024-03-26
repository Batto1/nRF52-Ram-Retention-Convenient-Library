/**
 * @author Batto1
 * @brief  Example application for showcasing some functionality of nRF52 RAM retention utilities library. Note that some specific KConfig configurations are have to be made to enable RAM retention.
 * @date 	 26.03.2024
 *
 */


#include <inttypes.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ram_retention_utils.h"


LOG_MODULE_REGISTER(main_c, LOG_LEVEL_DBG);

RR_Init_Var_Ram_Retention(RamRetAtomic, g_cnt); 		// atomic RAM retention variable type declared, defined and initialized for RAM retention.

RamRetInt64_t g_poweredup_time = {0}; 				// int64_t data holding RAM retention type declared and defined. Note that it isn't automatically RAM retention initiallized like other variables, it'll be custom initialized for demonstration purposes.

// custom RAM retention variable type declared.
typedef struct myCustomData{
	int 		foo;
	float 	bar;
	char 	baz[20];
	// ...
}MyCustomData;

RamRetTypeDeclare(MyCustomData, RamRetMyCustomData); 		// type declaration for custom data.
RR_Init_Var_Ram_Retention(RamRetMyCustomData, g_custom); 	// custom RAM retention variable initialized for RAM retention.

int g_no_ram_ret_var = 0;

void main(void)
{
	for(int i = 0; i < 5; i++){
		(void)atomic_inc(&g_cnt.rr_var); // increment RAM retained atomic variable.
		RR_Var_Ram_Ret(RamRetAtomic, &g_cnt); // retain the variable after modifying it, otherwise changes won't be retained.

		k_busy_wait(100000); // wait 100msec.		
	}


	bool is_valid = RR_Var_Ram_Ret_Init(RamRetInt64t, g_poweredup_time); // initialized RAM retention variable.
	if(false == is_valid){					
		printk("Ram ret initialization error\n"); 	
	}	
	g_poweredup_time.rr_var += k_uptime_get();
	RR_Var_Ram_Ret(RamRetInt64t, &g_poweredup_time); // retain the variable after modifying it, otherwise changes won't be retained.


	g_custom.rr_var.foo += 5;
	g_custom.rr_var.bar += 5;
	RR_Var_Ram_Ret(RamRetMyCustomData, &g_custom); // retain the variable after modifying it, otherwise changes won't be retained.
	k_busy_wait(100000); // wait 100msec.


	g_no_ram_ret_var ++; 
	printk("RAM ret variables current values:\n");
	printk("g_cnt: %d\n", (int)atomic_get(&g_cnt.rr_var));
	printk("g_poweredup_time: %"PRId64"ms\n", g_poweredup_time.rr_var);
	printk("g_custom.rr_var.foo:%d  g_custom.rr_var.bar: %f\n", g_custom.rr_var.foo, g_custom.rr_var.bar);

	printk("\nint variable that is not RAM retained: %d\n", g_no_ram_ret_var);

	printk("Main end, restart board for observing RAM retention variables again.\n");
	printk("*******************************************");

	while(1);
}
