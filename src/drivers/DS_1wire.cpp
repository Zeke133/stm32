#include "DS_1wire.h"


DS_1Wire_controller::DS_1Wire_controller(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) 
	: GPIOx(GPIOx), GPIO_Pin(GPIO_Pin) {

	uint32_t RCC_APB2Periph_GPIOx;

	if (GPIOx == GPIOA) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
	else if (GPIOx == GPIOB) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
	else if (GPIOx == GPIOC) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOC;
	else if (GPIOx == GPIOD) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOD;
	else if (GPIOx == GPIOE) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOE;
	else if (GPIOx == GPIOF) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOF;
	else if (GPIOx == GPIOG) RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOG;
	else return;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx , ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA , &GPIO_InitStructure);
}

void DS_1Wire_controller::WriteTimeslot(bool bit) {

	//Both  types  of  write  time  slots  are  initiated  by  the
		//master pulling the 1-Wire bus low
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		DelayMC(3); // should release within 15us

		if(bit) {
			//To  generate  a  Write  1  time  slot,  after  pulling  the  1-Wire
			//bus  low,  the  bus  master  must  release  the  1-Wire  bus
			//within 15us
			GPIO_SetBits(GPIOA, GPIO_Pin_8);
		}
		else {
			//To generate a Write 0 time
			//slot, after pulling the 1-Wire bus low, the bus master must
			//continue  to  hold  the  bus  low  for  the  duration  of  the  time
			//slot (at least 60us).
		}
		DelayMC(90);

		//All  write time slots must be a minimum of 60us in duration with a minimum of a 1us recovery time between individual write
		//slots.  Both  types  of  write  time  slots  are  initiated  by  the master pulling the 1-Wire bus low
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		DelayMC(5);	// minimum 1us recovery between time slots

}

bool DS_1Wire_controller::ReadTimeslot(void) {

	//A read time slot is initiated by the master device pulling the
		//1-Wire bus low for a minimum of 1us and then releasing the  bus
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		DelayMC(1);
		//After  the  master  initiates  the  read  time  slot,  the  DS18B20  will  begin  transmitting  a  1
		//or  0  on  bus
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		//Output data from the  DS18B20  is  valid  for  15us  after  the  falling  edge  that
		//initiated  the  read  time  slot.  Therefore,  the  master  must
		//release  the  bus  and  then  sample  the  bus  state  within
		//15us from the start of the slot.
		DelayMC(7);
		auto stat = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
		DelayMC(90);
		return stat;
}

// true - success
// false - error
bool DS_1Wire_controller::Initialization(void) {

	// reset  pulse  by  pulling  the  1-Wire  bus  low for  a  minimum  of  480us
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		DelayMC(600);

		// When the DS18B20 detects this rising edge, it waits 15us to 60us and then transmits a presence pulse by pulling the 1-Wire bus low for 60us to 240us.
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		DelayMC(100);

		auto stat = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
		DelayMC(500);

		return !stat;
}

void DS_1Wire_controller::WriteByte(unsigned char byte) {

	while(byte)
		{
			WriteTimeslot(byte & 0x01);
			byte >>= 1;
		}
}

unsigned char DS_1Wire_controller::ReadByte(void) {

	unsigned char result = 0;
	for(int i = 0; i < 8; i++)
	{
		result += ReadTimeslot();
		result <<= 1;
	}
	return result;
}

bool DS_1Wire_controller::GetCodes(unsigned char req, unsigned char * resp) {

	if (Initialization()) {
		WriteByte(req);
		for(int i = 0; i < 8; i++) {
			resp[i] = ReadByte();
		}
		return true;
	} else {
		return false;
	}
}

