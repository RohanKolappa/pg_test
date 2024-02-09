#pragma once

#include "mem_model.h"

#define ADDR_WIDTH 17
#define DATA_WIDTH 8

#define SIGNAL_INTEGRITY_INNNER (1<<8)	//Operations per dot
#define SIGNAL_INTEGRITY_MIDDLE (16)	//Dots per pass
#define SIGNAL_INTEGRITY_OUTER  (16)	//Passes

#define RANDOM_TEST_SIZE    (1<<24)	

int data_bus_test(mem_model * model_dut);
int addr_bus_test(mem_model * model_dut);
int device_test(mem_model * model_dut);
int	device_test_fast(mem_model * model_dut);
int signal_integrity_test(mem_model * model_dut);
int random_test(mem_model * model_dut);

