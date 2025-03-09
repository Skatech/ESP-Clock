Connections of ESP12/NodeMCU:

ESP12-PIN	NAME		TYPE	DESCRIPTION
-------------------------------------------
5  		D5/GPIO14	OUT	WS2812 strip display data line
19 		D2/GPIO4	SDA	I2C display data line
20 		D1/GPIO5	SCL   I2C display clock line
-------------------------------------------
1  				RST   pull-up 10k (RESET-button)
3  				EN	pull-up 10k
8				VCC	3.3V 500mA
15				GND
18 		D3/GPIO0		pull-up 10k (FLASH-button)
16 		D8/GPIO15  		pull-dn 10k
21 		   GPIO3    RXD	UART receive line
22 		   GPIO1	TXD	UART transmit line