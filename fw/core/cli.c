/*
 *  File: cli.c
 *  Description: Provides a command line interface and offers possibility to register function as new command
 *  Author:
 */

/* Board Header files */
#include "../../Board.h"

#include "cli.h"
#include "system.h"
#include "../peripherals/gps.h"
#include "../peripherals/navigation.h"
#include "../peripherals/hal/hx1.h"
#include "../peripherals/hal/rockblock.h"
#include "../lib/printf.h"


//which uart index to use for the CLI
#define CLI_UART Board_UART0_DEBUG
//buffer sizes
#define CLI_BUFFER 300
#define PRINTF_BUFFER 300



static UART_Handle uart = NULL;
static UART_Params uartParams;




void cli_init(){

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_NEWLINE;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uart = UART_open(CLI_UART, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

}


static void putc_callback(void* p,char c){
	*(*((char**)p))++ = c;
}

//can be called from any function to queue output strings (currently only integer support)
void cli_printf(char *print_format, ...){
	static char printf_output_buffer[PRINTF_BUFFER];
	char *strp = &printf_output_buffer[0];

	// clear buffer from any previous messages
	memset(strp, 0, sizeof(printf_output_buffer));

	va_list ap;
	va_start(ap, print_format);
	tfp_format(&strp, putc_callback, print_format, ap);
	putc_callback(&strp, 0);
	va_end(ap);

	//post message to mailbox
    Mailbox_post(cli_print_mailbox, printf_output_buffer, BIOS_NO_WAIT);  //from this context, timeouts are not allowed
}



//allows sending log messages to the console
void cli_print_task(){
	char printf_output_buffer[PRINTF_BUFFER];

    //clear buffer from any previous messages
	memset(&printf_output_buffer[0], 0, sizeof(printf_output_buffer));

	while(1){

		while(Mailbox_pend(cli_print_mailbox, printf_output_buffer, BIOS_WAIT_FOREVER)){
			//while mailbox contains messages, print out to uart
			if(uart != NULL){
				//print message
				UART_write(uart, printf_output_buffer, sizeof(printf_output_buffer));
			}

		}

	}


}


//runs with lowest priority
void cli_task(){

	char input[CLI_BUFFER];
	char output[CLI_BUFFER];
	const char consolePrompt[] = "octanis Rover Console:\r\n";

	//initialises UART0 handle
	cli_init();

	//prints welcome message
	UART_write(uart, consolePrompt, sizeof(consolePrompt));

	/* loop forever */
	while (1) {

		//clears buffers
		memset(&input[0], 0, sizeof(input));
		memset(&output[0], 0, sizeof(output));

		//blocks until command received
		UART_read(uart, &input, sizeof(input));


		if(strncmp ("mot", input, 3) == 0){ //motor command was sent
			if(navigation_bypass(input[3],(input[4]-'0')))
				tfp_sprintf(output, "okm\n");
			else
				tfp_sprintf(output, "inv\n");
			UART_write(uart, output, sizeof(output));
		}
		else if(strcmp("gps\n", input) == 0){
		   tfp_sprintf(output, "fq %d", gps_get_fix_quality());
		   UART_write(uart, output, sizeof(output));
		}else if(strcmp("lat\n", input) == 0){
			   ftoa(gps_get_lat(), output, 4);
			   UART_write(uart, output, sizeof(output));
		}else if(strcmp("lon\n", input) == 0){
			   ftoa(gps_get_lon(), output, 4);
			   UART_write(uart, output, sizeof(output));
		}else if(strcmp("sat\n", input) == 0){
			   tfp_sprintf(output, "sat %d \n", gps_get_satellites_tracked());
			   UART_write(uart, output, sizeof(output));
		}else if(strcmp("valid\n", input) == 0){
			   tfp_sprintf(output, "valid %d \n", gps_get_validity());
			   UART_write(uart, output, sizeof(output));
		}else if(strcmp("hdop\n", input) == 0){
			   tfp_sprintf(output, "hdop %d \n", gps_get_hdop());
			   UART_write(uart, output, sizeof(output));
		}else if(strcmp("lastgps\n", input) == 0){
			   tfp_sprintf(output, "lu %d \n", gps_get_last_update_time());
			   UART_write(uart, output, sizeof(output));
		}else if(strcmp("tasks\n", input) == 0){
			   system_listTasks();
		}else if(strcmp("rbs\n", input) == 0){
			   tfp_sprintf(output, "rb sleep? %d \n", rockblock_get_sleep_status());
			   UART_write(uart, output, sizeof(output));
		}else if(strcmp("rbn\n", input) == 0){
			   tfp_sprintf(output, "rb net? %d \n", rockblock_get_net_availability());
			   UART_write(uart, output, sizeof(output));
		}
//		else if(strcmp("hoff\n", input) == 0){
//			   hx1_off();
//		}else if(strcmp("hon\n", input) == 0){
//			   hx1_on();
//		}






	}


}
