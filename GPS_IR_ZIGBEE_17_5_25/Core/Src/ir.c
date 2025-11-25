#include<stdio.h>
#include<string.h>
#include "main.h"
#include "ir.h"
#include "stm32l4xx_hal_tim.h"


void ir_data()
{

	        uint32_t duration;

	        // Handle timer overflow
	        if (now >= last_time)
	            duration = now - last_time;
	        else
	            duration = (0xFFFF - last_time + now + 1);

	        last_time = now;

	        // NEC protocol:
	        // Start pulse = 9ms LOW + 4.5ms HIGH = ~13.5ms gap
	        // '0' = 560us pulse + 560us space (~1.1ms total)
	        // '1' = 560us pulse + 1.69ms space (~2.2ms total)

	        if (!receiving)
	        {
	            // Looking for start pulse (~9ms LOW + 4.5ms HIGH = total ~13.5ms)
	            if (duration > 12000 && duration < 14000)
	            {
	                receiving = 1;
	                bit_index = 0;
	                ir_code = 0;
	            }
	        }
	        else
	        {
	            // Interpret pulse as 0 or 1 based on duration
	            if (duration > 1000 && duration < 1400)
	            {
	                // Logic 0s
	                ir_code |= (0 << bit_index);
	                bit_index++;
	            }
	            else if (duration > 2000 && duration < 2500)
	            {
	                // Logic 1
	                ir_code |= (1UL << bit_index);
	                bit_index++;
	            }

	            if (bit_index >= 32)
	            {
	                receiving = 0;
	                done = 1;
	            }
	        }
	    }

uint32_t actual_data(uint32_t ir_val)
{
	uint32_t res;
	 uint8_t inverted_command = ir_val & 0xFF;               // Last byte (0x12)
	 uint8_t inverted_address = (ir_val >> 16) & 0xFF;       // Third byte from left (0x25)
	 res = (inverted_command<<8)|inverted_address;
	 attacker_id = inverted_command&0xFF;
	 fire_mode =(inverted_address>>4) & 0x0F;
	 weapon_id = inverted_address & 0x0F;
	 return(res);

}
