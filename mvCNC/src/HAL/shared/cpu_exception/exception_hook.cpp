/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "exception_hook.h"

void * __attribute__((weak)) hook_get_hardfault_vector_address(unsigned) { return 0; }
void * __attribute__((weak)) hook_get_memfault_vector_address(unsigned) { return 0; }
void * __attribute__((weak)) hook_get_busfault_vector_address(unsigned) { return 0; }
void * __attribute__((weak)) hook_get_usagefault_vector_address(unsigned) { return 0; }
void __attribute__((weak)) hook_last_resort_func() {}
