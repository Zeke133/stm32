#include <DS_1wire.h>


DS_1Wire_controller::DS_1Wire_controller(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) 
	: GPIOx(GPIOx),
		GPIO_Pin(GPIO_Pin),
		InitTOs{60, 10, 50},	// *10
		WriteTOs{3, 90, 5},
		ReadTOs{1, 7, 90} {

	GPIO_Init_My(GPIOx, GPIO_Pin, GPIO_Mode_Out_OD, GPIO_Speed_50MHz);

}

DS_1Wire_controller::DS_1Wire_controller(GPIO_TypeDef* GPIOx,
										uint16_t GPIO_Pin,
										uint8_t InitTOs[3],
										uint8_t WriteTOs[3],
										uint8_t ReadTOs[3])
	:  GPIOx(GPIOx),
	GPIO_Pin(GPIO_Pin),
	InitTOs {InitTOs[0], InitTOs[1], InitTOs[2]},
	WriteTOs {WriteTOs[0], WriteTOs[1], WriteTOs[2]},
	ReadTOs {ReadTOs[0], ReadTOs[1], ReadTOs[2]} {

	GPIO_Init_My(GPIOx, GPIO_Pin, GPIO_Mode_Out_OD, GPIO_Speed_50MHz);

}

void DS_1Wire_controller::WriteTimeslot(uint8_t bit) {

	Delay& wait = *delay;
	//Both  types  of  write  time  slots  are  initiated  by  the
	//master pulling the 1-Wire bus low
	GPIO_ResetBits(GPIOx, GPIO_Pin);
	wait.us(WriteTOs[0]); // should release within 15us

	if(bit) {
		//To  generate  a  Write  1  time  slot,  after  pulling  the  1-Wire
		//bus  low,  the  bus  master  must  release  the  1-Wire  bus
		//within 15us
		GPIO_SetBits(GPIOx, GPIO_Pin);
	}
	//To generate a Write 0 time
	//slot, after pulling the 1-Wire bus low, the bus master must
	//continue  to  hold  the  bus  low  for  the  duration  of  the  time
	//slot (at least 60us).
	wait.us(WriteTOs[1]);

	//All  write time slots must be a minimum of 60us in duration with a minimum of a 1us recovery time between individual write
	//slots.  Both  types  of  write  time  slots  are  initiated  by  the master pulling the 1-Wire bus low
	GPIO_SetBits(GPIOx, GPIO_Pin);
	wait.us(WriteTOs[2]);	// minimum 1us recovery between time slots

}

uint8_t DS_1Wire_controller::ReadTimeslot(void) {

	Delay& wait = *delay;
	//A read time slot is initiated by the master device pulling the
	//1-Wire bus low for a minimum of 1us and then releasing the  bus
	GPIO_ResetBits(GPIOx, GPIO_Pin);
	wait.us(ReadTOs[0]);
	//After  the  master  initiates  the  read  time  slot,  the  DS18B20  will  begin  transmitting  a  1
	//or  0  on  bus
	GPIO_SetBits(GPIOx, GPIO_Pin);
	//Output data from the  DS18B20  is  valid  for  15us  after  the  falling  edge  that
	//initiated  the  read  time  slot.  Therefore,  the  master  must
	//release  the  bus  and  then  sample  the  bus  state  within
	//15us from the start of the slot.
	wait.us(ReadTOs[1]);
	uint8_t stat = GPIO_ReadInputDataBit(GPIOx, GPIO_Pin);
	wait.us(ReadTOs[2]);
	return stat;
}

// 0 - success
// 1 - error
uint8_t DS_1Wire_controller::Initialization(void) {

	Delay& wait = *delay;
	// reset  pulse  by  pulling  the  1-Wire  bus  low for  a  minimum  of  480us
	GPIO_ResetBits(GPIOx, GPIO_Pin);
	wait.us(InitTOs[0]*10);

	// When the DS18B20 detects this rising edge, it waits 15us to 60us and then transmits a presence pulse by pulling the 1-Wire bus low for 60us to 240us.
	GPIO_SetBits(GPIOx, GPIO_Pin);
	wait.us(InitTOs[1]*10);

	uint8_t stat = GPIO_ReadInputDataBit(GPIOx, GPIO_Pin);
	wait.us(InitTOs[2]*10);

	return stat;
}

void DS_1Wire_controller::WriteByte(uint8_t byte) {

	for(int i = 0; i < 8; i++) {
		WriteTimeslot(byte & 0x01);
		byte >>= 1;
	}
}

uint8_t DS_1Wire_controller::ReadByte(void) {

	uint8_t result = 0;
	for(int i = 0; i < 8; i++) {
		result += ReadTimeslot();
		result <<= 1;
	}
	return result;
}

uint8_t DS_1Wire_controller::GetCodes(uint8_t req, uint8_t * resp) {

	if (Initialization())
		return 1;
	
	WriteByte(req);
	for(int i = 0; i < 8; i++) {
		resp[i] = ReadByte();
	}
	return 0;
}

