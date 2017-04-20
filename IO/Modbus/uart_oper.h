/********************************************************************
	created:	2017/03/18
	created:	18:3:2017   15:02
	filename: 	Z:\pd\pd_modbus\src_bin\uart_oper.h
	file path:	Z:\pd\pd_modbus\src_bin
	file base:	uart_oper
	file ext:	h
	author:		
	purpose:	uart operation
*********************************************************************/
#ifndef _UART_OPER_H_
#define _UART_OPER_H_

int uart_set (int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity);
int uart_open (char * port, int speed, int flow_ctrl, int databits, int stopbits, int parity);
void uart_close (int fd);
int uart_recv (int fd, unsigned char * recv_buf, int data_len);
int uart_send (int fd, unsigned char * send_buf, int data_len);

#endif /* _UART_OPER_H_ */