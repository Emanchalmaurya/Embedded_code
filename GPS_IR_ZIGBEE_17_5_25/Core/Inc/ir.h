#ifndef _ir_H
#define _ir_H

#include<stdio.h>
#include "main.h"
//pv
volatile uint8_t bit_index = 0;
volatile uint32_t ir_code = 0;
volatile uint32_t last_time = 0;
volatile uint8_t receiving = 0;
volatile uint8_t done = 0;
volatile uint32_t res;

extern int is_hit;

extern uint8_t Master_id;
extern uint16_t attacker_id;
extern uint8_t fire_mode;
extern uint8_t weapon_id;

extern uint32_t now;
extern TIM_HandleTypeDef htim2;

//pfp
uint32_t actual_data(uint32_t ir_val);
void ir_data();

#endif
