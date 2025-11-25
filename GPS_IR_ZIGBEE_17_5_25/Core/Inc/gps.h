#ifndef _gps_H
#define _gps_H

//variable declaration
uint8_t recv_data;
char recv_buff[48];
int count=0;
int person_id = 7;
float roll;
float pitch;
float yaw;
int is_hit=00;
uint8_t Master_id = 0;
uint16_t attacker_id=0;
uint8_t fire_mode=0;
uint8_t weapon_id=0;
int trigger_event=0;
int bullet_count;

char *latitude;
char *longitude;

//function prototype
void gps_data();
void lat_long(char []);
void TX_data(char *latitude,char *longitude);

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

#endif
