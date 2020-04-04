#define RS485_ENABLE


#ifdef RS485_ENABLE
#   define MAX485_RECV do{HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);}while(0);
#   define MAX485_TRAN do{HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);}while(0);
#else
#   define MAX485_RECV do{}while(0);
#   define MAX485_TRAN do{}while(0);
#endif
	

