#include "contiki.h"
#include "dev/button-sensor.h"
#include "arch/dev/sensor/sht11/sht11-sensor.h"
#include "arch/platform/sky/dev/light-sensor.h"
#include "leds.h"
#include "math.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include <stdio.h>
#include <string.h>
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
//#include "Mini-project data.h"
/*---------------------------------------------------------------------------*/
int length = 6;
uint8_t count[6];
static linkaddr_t dest_addr = {{ 0x02, 0x02, 0x02, 0x00, 0x02, 0x74, 0x12, 0x00 }};
/*---------------------------------------------------------------------------*/
float temp1[30] =	{24.69, 24.72, 24.72, 24.73, 24.73, 24.76,
	24.76, 24.80, 24.80, 24.80, 24.83, 24.83,
	24.84, 24.86, 24.87, 24.90, 24.90, 24.91,
	24.93, 24.95, 24.95, 24.97, 25.00, 25.00,
25.01, 25.02, 25.05, 25.05, 25.06, 25.08};

float light1[30] =	{28.90, 28.49, 28.49, 29.31, 28.90, 28.49,
	28.90, 28.49, 28.49, 28.90, 28.90, 28.49,
	29.31, 28.49, 28.90, 28.90, 28.90, 28.90,
	29.31, 29.31, 29.31, 28.08, 29.31, 28.90,
28.49, 28.90, 28.90, 28.90, 28.49, 28.90};

float getTemp1(int i)
{
	if((i >= 1) && (i <= 30))
	{
		return temp1[i-1];
	}
	else
	{
		return 0;
	}
}
float getLight1(int i)
{
	if((i >= 1) && (i <= 30))
	{
		return light1[i-1];
	}
	else
	{
		return 0;
	}
}

/*uint8_t get_origin(const union linkaddr_t source)
{
	if(source == {{0x02, 0x02, 0x02, 0x00, 0x02, 0x74, 0x12, 0x00}})
	{
		return 2;
	}
	else if(source == {{0x03, 0x03, 0x03, 0x00, 0x03, 0x74, 0x12, 0x00}})
	{
		return 3;
	}
	else if(source == {{0x04, 0x04, 0x04, 0x00, 0x04, 0x74, 0x12, 0x00}})
	{
		return 4;
	}
	else if(source == {{0x05, 0x05, 0x05, 0x00, 0x05, 0x74, 0x12, 0x00}})
	{
		return 5;
	}
	else if(source == {{0x06, 0x06, 0x06, 0x00, 0x06, 0x74, 0x12, 0x00}})
	{
		return 6;
	}
	else if(source == {{0x07, 0x07, 0x07, 0x00, 0x07, 0x74, 0x12, 0x00}})
	{
		return 7;
	}
	else if(source == {{0x08, 0x08, 0x08, 0x00, 0x08, 0x74, 0x12, 0x00}})
	{
		return 8;
	}
}*/

/*---------------------------------------------------------------------------*/
PROCESS(snd_proc, "Sender process");
PROCESS(rcv_proc, "Receiver process");

AUTOSTART_PROCESSES(&snd_proc, &rcv_proc);

/*---------------------------------------------------------------------------*/
void Xinput_callbackX(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest)
{
	memcpy(&count, data, sizeof(count));
	if(count[0] == 0)
	{
		count[0] = src->u8[0];
	}
	nullnet_buf = count;
	NETSTACK_NETWORK.output(&dest_addr);
	
	//LOG_INFO_LLADDR(&dest_addr);
	//LOG_INFO_("\n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(snd_proc, ev, data)
{
	static struct etimer timer;
	
	static int i = 10;
	static int k = 1;
	uint8_t sensorData[length];
	sensorData[0] = 0;
	float measure[2] = {0,0};
	PROCESS_BEGIN();
	
	etimer_set(&timer, CLOCK_SECOND*2);
	
	while(1)
	{
		//getData1(1,&measure[0],&measure[1]);
		measure[0] += getTemp1(i);
		measure[1] += getLight1(i);
		
		if(k == 3)
		{
			measure[0] /= 3;
			measure[1] /= 3;
			sensorData[1] = measure[0];
			sensorData[2] = (measure[0]*100)-(((int)measure[0])*100);
			sensorData[3] = measure[1];
			sensorData[4] = (measure[1]*100)-(((int)measure[1])*100);
			sensorData[0] = 0;
			nullnet_buf = sensorData;
			nullnet_len = length;
			NETSTACK_NETWORK.output(&dest_addr);
			measure[0] = 0;
			measure[1] = 0;
		}
		
		//printf("Sensor: sender %d, origin %d temp %d.%d, light %d.%d, i %d\n",sensorData[0],sensorData[1],sensorData[2],sensorData[3],sensorData[4],sensorData[5],i);
		
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
		etimer_reset(&timer);
		
		k++;
		
		if(i == 30)
		{
			i = 1;
			//printf("Test 30\n");
		}
		else
		{
			i++;
			//printf("Test !30\n");
		}
	}
	
	
	
	PROCESS_END();
}

PROCESS_THREAD(rcv_proc, ev, data)
{
	static struct etimer timer;
	
	PROCESS_BEGIN();
	
	etimer_set(&timer, CLOCK_SECOND);
	
	nullnet_set_input_callback(Xinput_callbackX);
	
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
		etimer_reset(&timer);
	}
	
	PROCESS_END();
}