# nRF52-Ram-Retention-Convenient-Library
## Introduction
Library for easy and convenient utilization of nRF52 series MCU RAM retention function for Zephyr RTOS.

RAM retention is used for retaining desired variables in some block of RAM memory. These variables keep their value even if the system is reset as long as device is properly powered.

## Usage
Useful for tracking data that don't want to be lost due to system resets. If data don't want to be lost at all, even in a power loss, RAM retention isn't enough and data should be written into a non-volatile memory (i.e. SoC user allocated FLASH sections or EEPROM).

## Files 
- ram_retention/ram_retention_utils.h/.c
- ram_retention/Kconfig
- sys_init_utils.h (helper for ram_retention_utils.h/.c)
- user_common.h (helper for sys_init_utils.h)

## Library functionalities
- Conveniently declaring and defining custom ram retention type data.
- Conveniently defining convenient primitive (int, float, int16_t etc) ram retention type data.
- Conveniently initializing any type of data for ram retention.
- Conveniently externing declared ram retention type data.
- Conveniently retaining values after after data  modification.

## Sample Application
Includes sample application for demonstrating some routines, see main.c
