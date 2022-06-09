/* 
 * 
 * 
 * coordinate system
 * 
 *       ^ y(up)
 *       |
 *       |
 *   ----+----> x (right)
 * (left)|       
 *       | (down)
 * 
 */

#include<stdint.h>

#include<PS4BT.h>
#include<usbhub.h>
#include<SPI.h>

USB Usb;
BTD Btd(&Usb);

PS4BT ps4(&Btd, PAIR);

#define EMERGENCY_STOP -1

struct Axis{
	int8_t x = 0,
	       y = 0,
		   omega = 0;
} axis;

void shot();

void setup() {
	pinMode(39, OUTPUT);
	pinMode(41, OUTPUT);

	Serial.begin(115200);
	Serial1.begin(115200);
	Serial2.begin(115200);
	Serial3.begin(115200);
	
	#if !defined(__MIPSEL__)
	while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
	#endif
	if (Usb.Init() == -1) {
		Serial.print(F("\r\nOSC did not start"));
		while (1); // Halt
	}
	Serial.print(F("\r\nPS4 Bluetooth Library Started"));

}

void loop() {
  Usb.Task();

	if(ps4.connected()){

		if(ps4.getButtonClick(PS)){
			Serial1.write(EMERGENCY_STOP);
			ps4.disconnect();

			while(1);
		}

		if(ps4.getButtonClick(CIRCLE))shot();

		axis.y = (ps4.getButtonPress(UP)?1:0) + (ps4.getButtonPress(DOWN)?-1:0);
		axis.x = (ps4.getButtonPress(RIGHT)?1:0) + (ps4.getButtonPress(LEFT)?-1:0);
		axis.omega = (ps4.getButtonPress(L1)?1:0) + (ps4.getButtonPress(R1)?-1:0);

		Serial1.write((char*)&axis);

  	}
}

void shot(){
	digitalWrite(39, LOW);
	digitalWrite(41, HIGH);
	delay(1000);

	digitalWrite(39, HIGH);
	digitalWrite(41, LOW);
	delay(1000);

	digitalWrite(39, LOW);
}
