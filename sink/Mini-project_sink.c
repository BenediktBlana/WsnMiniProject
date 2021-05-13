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
/*---------------------------------------------------------------------------*/
/*https://stackoverflow.com/questions/41322192/how-can-i-send-integer-array-in-unicast-example-of-contiki */

/*---------------------------------------------------------------------------*/
PROCESS(prnt_proc, "Print process");
PROCESS(rcv_proc, "Receiver process");

AUTOSTART_PROCESSES(&prnt_proc, &rcv_proc);

/*---------------------------------------------------------------------------*/

void Xinput_callbackX(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest)
{
	uint8_t count[6];
	memcpy(&count, data, sizeof(count));
	if(count[0] == 0)
	{
		count[0] = src->u8[0];
	}
	if(count[5] == 0)
	{
		printf("temp: %d.%d, light: %d.%d at node %d\n",count[1],count[2],count[3],count[4],count[0]);
	}
	else
	{
		printf("temp: %d.%d, light: %d.%d at node %d\n",count[1],count[2],count[3]+count[4],count[5],count[0]);
	}
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(prnt_proc, ev, data)
{
	PROCESS_BEGIN();
	
	
	PROCESS_END();
}

PROCESS_THREAD(rcv_proc, ev, data)
{
	static struct etimer timer;
	
	PROCESS_BEGIN();
	
	etimer_set(&timer, CLOCK_SECOND * 2);
	
	nullnet_set_input_callback(Xinput_callbackX);
	
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
		etimer_reset(&timer);
	}
	
	PROCESS_END();
}