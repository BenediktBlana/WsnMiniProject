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
static linkaddr_t dest_addr = {{ 0x01, 0x01, 0x01, 0x00, 0x01, 0x74, 0x12, 0x00 }};
/*---------------------------------------------------------------------------*/
float temp2[30] =	{19.93, 19.84, 19.75, 19.67, 19.56, 19.43,
	19.29, 19.18, 19.06, 18.97, 18.88, 18.74,
	18.65, 18.56, 18.47, 18.34, 18.18, 17.99,
	17.90, 17.81, 17.74, 17.70, 17.58, 17.51,
17.43, 17.34, 17.29, 17.22, 17.08, 16.90};

float light2[30] =	{274.79, 274.79, 274.38, 274.79, 274.38,
	274.38, 275.60, 276.42, 275.60, 275.19,
	275.19, 275.19, 275.60, 276.01, 275.60,
	275.60, 276.01, 276.01, 276.42, 276.42,
	276.82, 276.82, 277.23, 277.64, 277.23,
278.03, 277.64, 277.23, 278.45, 279.27};

float getTemp2(int i)
{
	if((i >= 1) && (i <= 30))
	{
		return temp2[i-1];
	}
	else
	{
		return 0;
	}
}
float getLight2(int i)
{
	if((i >= 1) && (i <= 30))
	{
		return light2[i-1];
	}
	else
	{
		return 0;
	}
}


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
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(snd_proc, ev, data)
{
	static struct etimer timer;
	
	static int i = 1;
	static int k = 1;
	uint8_t sensorData[length];
	sensorData[0] = 0;
	float measure[2] = {0,0};
	PROCESS_BEGIN();
	
	etimer_set(&timer, CLOCK_SECOND*2);
	
	while(1)
	{
		//getData1(1,&measure[0],&measure[1]);
		measure[0] += getTemp2(i);
		measure[1] += getLight2(i);
		
		
		if(k == 3)
		{
			measure[0] /= 3;
			measure[1] /= 3;
			sensorData[1] = measure[0];
			sensorData[2] = (measure[0]*100)-(((int)measure[0])*100);
			sensorData[3] = 255;
			sensorData[4] = measure[1]-255;
			sensorData[5] = (measure[1]*100)-(((int)measure[1])*100);
			nullnet_buf = sensorData;
			nullnet_len = length;
			NETSTACK_NETWORK.output(&dest_addr);
			measure[0] = 0;
			measure[1] = 0;
		}
		
		//printf("Sensor: sender %d, origin %d temp %d.%d, light %d.%d, i %d\n",sensorData[0],sensorData[1],sensorData[2],sensorData[3],sensorData[4],sensorData[5],i);
		
		//LOG_INFO_LLADDR(&dest_addr);
		//LOG_INFO_("\n");
		
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