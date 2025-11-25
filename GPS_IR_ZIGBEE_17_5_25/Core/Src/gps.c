#include<stdio.h>
#include<string.h>
#include"main.h"
#include"gps.h"


void gps_data()
{

    if (recv_data == '\n') // End of NMEA sentence
    {
        recv_buff[count] = '\0'; // Null terminate
       // printf("%s\n", recv_buff);
        count = 0; // Reset for next sentence

        lat_long(recv_buff);

    }
    else
    {
        if (count < sizeof(recv_buff) - 1)
            recv_buff[count++] = recv_data;
    }

    // Restart reception
    HAL_UART_Receive_IT(&huart3, &recv_data, 1);
}
void lat_long(char buffer[]){
	char *latitude;
	char *longitude;
	char *token;
	int field_count = 0;
//GNRMC FORMAT
	if (strncmp(buffer, "$GNRMC", 6) == 0) {
		token = strtok(buffer, ",");
		while (token != NULL) {
			field_count++;
				if (field_count == 3 && token[0] != 'A')  //valid data
					return;
				if (field_count == 4) { // Latitude field
					latitude = token;
					printf("lat=%s\n", latitude);
			} else if (field_count == 6) {
					longitude = token;
					printf("long=%s\n", longitude);
			}

			token = strtok(NULL, ",");
		}
		TX_data(latitude, longitude);
	}
//GNGLL FORMAT
	else if(strncmp(buffer,"$GNGLL",6)==0){
		 token = strtok(buffer, ",");
		        while (token != NULL)
		        {
		            field_count++;
		            if (field_count == 7 && token[0] != 'A') //valid data
		            	return;
		            if (field_count == 2){
		                latitude = token;
		                printf("lat=%s\n", latitude);
		            }
		            if (field_count == 4){
		                longitude = token;
		                printf("long=%s\n", longitude);
		            }

		            token = strtok(NULL, ",");
		        }
		        TX_data(latitude, longitude);
		    }
//GNGGA FORMAT
	else if(strncmp(buffer,"$GNGGA",6)==0){
			 token = strtok(buffer, ",");
			        while (token != NULL)
			        {
			            field_count++;
			            if(field_count==7 && token==0) //valid data check
			            	return;
			            if (field_count == 3){
			                latitude = token;
			                printf("lat=%s\n", latitude);
			            }
			            if (field_count == 5){
			                longitude = token;
			                printf("long=%s\n", longitude);
			            }

			            token = strtok(NULL, ",");
			        }
			        TX_data(latitude, longitude);
			    }


	/*if (latitude && longitude) {
		// printf("person_id:%d\nLatitude: %s \nLongitude: %s \r\n",person_id,latitude,longitude);
		TX_data(latitude, longitude);
	}*/

}


void TX_data(char *latitude,char *longitude){
	 char json_buffer[70];

	    // Create JSON string

	    snprintf(json_buffer, sizeof(json_buffer),
	             "{%d,%s,%s,%.1f,%.1f,%.1f,%d,%x,%x,%x,%d,%d,%d}\r\n",person_id,latitude,longitude,roll,
				 pitch,yaw,is_hit,attacker_id,fire_mode,weapon_id,Master_id,trigger_event,bullet_count);
	    //printf("person_id:%d\nLatitude: %s \nLongitude: %s \r\n",person_id,latitude,longitude);

	    //xbee
	    HAL_UART_Transmit(&huart1, (uint8_t*)json_buffer, strlen(json_buffer), HAL_MAX_DELAY);

	    //teraterm
	 //  HAL_UART_Transmit(&huart2,(uint8_t*)json_buffer, strlen(json_buffer), HAL_MAX_DELAY);
	    memset(json_buffer,0,sizeof(json_buffer));

}
