// ArduFlu v2.7b
// Alex Rudyk
// 18.06.2017

#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "Config.h"

byte msg[8];
const uint64_t pipe = 0xF0F1F2F3F4FF;
RF24 radio(9, 10);


// Pin for controler
int Right_joy_horizon = 0;
int Right_joy_vertical = 1;
int Left_joy_horizon = 2;
int Left_joy_vertical = 3;
int Right_knob = 6;
int Left_knob = 7;

byte Right_tumb_2pos = 8;
byte Left_tumb_2pos = 6;


LiquidCrystal_I2C lcd(0x3F, 16, 2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

byte buzzer = 3;

byte menuButton = 7;
byte plassButton = 4;
byte minusButton = 5;

// Var for Chanels
int endPointCH1 = 0;
bool reversCH1 = false;
int endPointCH2 = 0;
bool reversCH2 = false;
int endPointCH3 = 0;
bool reversCH3 = false;
int endPointCH4 = 0;
bool reversCH4 = false;
int endPointCH5 = 0;
bool reversCH5 = false;
int endPointCH6 = 0;
bool reversCH6 = false;

#define setPos(pin, chanel) (map(constrain(analogRead(pin), 95, 890), 95, 890, chanel, 180 - chanel))
#define setReversPos(pin, chanel) (map(constrain(analogRead(pin), 95, 890), 95, 890, 180 - chanel, chanel))

// Var for Menu
byte key = 0;
byte tim = 4;
byte pas = 0;
byte vst = 10;
byte mix = 1;
byte del = 250;
bool flag = false;
long previousMillis = 0;
boolean lcd_0 = 0; boolean lcd_1 = 0;
char* myStrings[] = {"Menu", "End Points", "Revers", "Default Settings", "Return", "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "ON", "OFF", "Save", "Clean?", "NO", "YES"};

void setup() {
	// Load var
	load();

	// Init display
	lcd.init();
	lcd.backlight();
	lcd.setCursor(0, 0);
	lcd.print("ArduFly6");
	lcd.setCursor(11, 0);
	lcd.print(version);
	lcd.setCursor(3, 1);
	lcd.print("Loading...");

	// Configuration pin
	pinMode(buzzer, OUTPUT);
	pinMode(menuButton, INPUT);
	pinMode(plassButton, INPUT);
	pinMode(minusButton, INPUT);
	pinMode(Right_tumb_2pos, INPUT);
	pinMode(Left_tumb_2pos, INPUT);

	digitalWrite(menuButton, HIGH);
	digitalWrite(plassButton, HIGH);
	digitalWrite(minusButton, HIGH);
	digitalWrite(Right_tumb_2pos, HIGH);
	digitalWrite(Left_tumb_2pos, HIGH);
	
	beep();
	
	radio.begin();
	delay(50);
	radio.setChannel(1);
	radio.setPayloadSize(8);

	radio.setRetries(1, 1);
	radio.setDataRate(RF24_250KBPS);
	radio.setPALevel(RF24_PA_HIGH);
	radio.openWritingPipe(pipe);

	beep();
	beep();
	lcd.clear();
}

void loop() {
	if (lcd_0 == 1) {
		lcd.setCursor(0, 0); lcd.print(String(myStrings[vst])); lcd.print("                ");
		lcd_0 = 0;
	}
	if (lcd_1 == 1) {
		lcd.setCursor(0, 1); lcd.print(String(myStrings[vst])); lcd.print("                ");
		lcd_1 = 0;
	}

	if (digitalRead(menuButton) == LOW && flag == false) { 
		if (millis() - previousMillis > 500) {
			previousMillis = millis();
			pas++;
			
		} 
	} else { pas = 0; }

	if (digitalRead(menuButton) == HIGH && flag == true) {
		flag = false;
	}

	switch (key)
	{
	case 1: //Menu
		//delay(del);
		if (digitalRead(minusButton) == LOW) { mix--; beep(); } if (mix == 0) { mix = 5; }
		if (digitalRead(plassButton) == LOW) { mix++; beep(); } if (mix == 6) { mix = 1; }
		if (mix == 1) { lcd_1 = 1; vst = 1; } //End Points
		if (mix == 2) { lcd_1 = 1; vst = 2; } //Revers
		//if (mix == 3) { lcd_1 = 1; vst = 13; } //Save
		if (mix == 3) { lcd_1 = 1; vst = 3; } //Deffoult Settings
		if (mix == 4) { lcd_1 = 1; vst = 4; } //Return
		if (digitalRead(menuButton) == LOW && mix == 1) { key = 2; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 1; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 2) { key = 3; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 2; beep(); }
		//if (digitalRead(menuButton) == LOW && mix == 3) { key = 0; mix = 1; lcd_1 = 0; lcd.clear(); beep(); save(); }
		if (digitalRead(menuButton) == LOW && mix == 3) { key = 4; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 14; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 4) { key = 0; mix = 1; lcd_1 = 0; lcd.clear(); beep(); }
		break;
	case 2: //End Points
		if (digitalRead(minusButton) == LOW) { mix--; beep(); } if (mix == 0) { mix = 7; }
		if (digitalRead(plassButton) == LOW) { mix++; beep(); } if (mix == 8) { mix = 1; }
		if (mix == 1) { lcd_1 = 1; vst = 5; }
		if (mix == 2) { lcd_1 = 1; vst = 6; }
		if (mix == 3) { lcd_1 = 1; vst = 7; }
		if (mix == 4) { lcd_1 = 1; vst = 8; }
		if (mix == 5) { lcd_1 = 1; vst = 9; }
		if (mix == 6) { lcd_1 = 1; vst = 10; }
		if (mix == 7) { lcd_1 = 1; vst = 4; }
		if (digitalRead(menuButton) == LOW && mix == 1) { key = 5; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 5; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 2) { key = 6; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 6; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 3) { key = 7; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 7; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 4) { key = 8; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 8; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 5) { key = 9; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 9; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 6) { key = 10; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 10; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 7) { key = 1; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 0; beep(); }
		break;
	case 3: //Revers
		if (digitalRead(minusButton) == LOW) { mix--; beep(); } if (mix == 0) { mix = 7; }
		if (digitalRead(plassButton) == LOW) { mix++; beep(); } if (mix == 8) { mix = 1; }
		if (mix == 1) { lcd_1 = 1; vst = 5; }
		if (mix == 2) { lcd_1 = 1; vst = 6; }
		if (mix == 3) { lcd_1 = 1; vst = 7; }
		if (mix == 4) { lcd_1 = 1; vst = 8; }
		if (mix == 5) { lcd_1 = 1; vst = 9; }
		if (mix == 6) { lcd_1 = 1; vst = 10; }
		if (mix == 7) { lcd_1 = 1; vst = 4; }
		if (digitalRead(menuButton) == LOW && mix == 1) { key = 11; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 5; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 2) { key = 12; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 6; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 3) { key = 13; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 7; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 4) { key = 14; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 8; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 5) { key = 15; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 9; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 6) { key = 16; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 10; beep(); }
		if (digitalRead(menuButton) == LOW && mix == 7) { key = 1; mix = 2; lcd_1 = 0; lcd_0 = 1; vst = 0; beep(); }
		break;
	case 4: //Deffoult Settings
		if (digitalRead(minusButton) == LOW) { mix--; beep(); } if (mix == 0) { mix = 2; }
		if (digitalRead(plassButton) == LOW) { mix++; beep(); } if (mix == 3) { mix = 1; }
		if (mix == 1) { lcd_1 = 1; vst = 15; }
		if (mix == 2) { lcd_1 = 1; vst = 16; }
		if (digitalRead(menuButton) == LOW && mix == 1) { key = 0; mix = 1; lcd_1 = 0; lcd.clear(); beep(); }
		if (digitalRead(menuButton) == LOW && mix == 2) { key = 0; mix = 1; lcd_1 = 0; lcd.clear(); beep(); defset(); }
		break;
	case 5: //End Point CH1
		if (digitalRead(minusButton) == LOW) { endPointCH1--; beep(); } if (endPointCH1 == -1) { endPointCH1 = maxEndPoint; }
		if (digitalRead(plassButton) == LOW) { endPointCH1++; beep(); } if (endPointCH1 == maxEndPoint + 1) { endPointCH1 = 0; }
		lcd.setCursor(0, 1); lcd.print(endPointCH1); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(0, endPointCH1); key = 2; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 1; beep(); }
		break;
	case 6: //End Point CH2
		if (digitalRead(minusButton) == LOW) { endPointCH2--; beep(); } if (endPointCH2 == -1) { endPointCH2 = maxEndPoint; }
		if (digitalRead(plassButton) == LOW) { endPointCH2++; beep(); } if (endPointCH2 == maxEndPoint + 1) { endPointCH2 = 0; }
		lcd.setCursor(0, 1); lcd.print(endPointCH2); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(2, endPointCH2); key = 2; mix = 2; lcd_1 = 0; lcd_0 = 1; vst = 1; beep(); }
		break;
	case 7: //End Point CH3
		if (digitalRead(minusButton) == LOW) { endPointCH3--; beep(); } if (endPointCH3 == -1) { endPointCH3 = maxEndPoint; }
		if (digitalRead(plassButton) == LOW) { endPointCH3++; beep(); } if (endPointCH3 == maxEndPoint + 1) { endPointCH3 = 0; }
		lcd.setCursor(0, 1); lcd.print(endPointCH3); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(4, endPointCH3); key = 2; mix = 3; lcd_1 = 0; lcd_0 = 1; vst = 1; beep(); }
		break;
	case 8: //End Point CH4
		if (digitalRead(minusButton) == LOW) { endPointCH4--; beep(); } if (endPointCH4 == -1) { endPointCH4 = maxEndPoint; }
		if (digitalRead(plassButton) == LOW) { endPointCH4++; beep(); } if (endPointCH4 == maxEndPoint + 1) { endPointCH4 = 0; }
		lcd.setCursor(0, 1); lcd.print(endPointCH4); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(6, endPointCH4); key = 2; mix = 4; lcd_1 = 0; lcd_0 = 1; vst = 1; beep(); }
		break;
	case 9: //End Point CH5
		if (digitalRead(minusButton) == LOW) { endPointCH5--; beep(); } if (endPointCH5 == -1) { endPointCH5 = maxEndPoint; }
		if (digitalRead(plassButton) == LOW) { endPointCH5++; beep(); } if (endPointCH5 == maxEndPoint + 1) { endPointCH5 = 0; }
		lcd.setCursor(0, 1); lcd.print(endPointCH5); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(8, endPointCH5); key = 2; mix = 5; lcd_1 = 0; lcd_0 = 1; vst = 1; beep(); }
		break;
	case 10: //End Point CH6
		if (digitalRead(minusButton) == LOW) { endPointCH6--; beep(); } if (endPointCH6 == -1) { endPointCH6 = maxEndPoint; }
		if (digitalRead(plassButton) == LOW) { endPointCH6++; beep(); } if (endPointCH6 == maxEndPoint + 1) { endPointCH6 = 0; }
		lcd.setCursor(0, 1); lcd.print(endPointCH6); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(10, endPointCH6); key = 2; mix = 6; lcd_1 = 0; lcd_0 = 1; vst = 1; beep(); }
		break;
	case 11: //Revers CH1
		if (digitalRead(minusButton) == LOW) { reversCH1 = !reversCH1; beep(); }
		if (digitalRead(plassButton) == LOW) { reversCH1 = !reversCH1; beep(); }
		lcd.setCursor(0, 1); lcd.print(reversCH1 ? myStrings[11] : myStrings[12]); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(1, reversCH1); key = 3; mix = 1; lcd_1 = 0; lcd_0 = 1; vst = 2; beep(); }
		break;
	case 12: //Revers CH2
		if (digitalRead(minusButton) == LOW) { reversCH2 = !reversCH2; beep(); }
		if (digitalRead(plassButton) == LOW) { reversCH2 = !reversCH2; beep(); }
		lcd.setCursor(0, 1); lcd.print(reversCH2 ? myStrings[11] : myStrings[12]); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(3, reversCH2); key = 3; mix = 2; lcd_1 = 0; lcd_0 = 1; vst = 2; beep(); }
		break;
	case 13: //Revers CH3
		if (digitalRead(minusButton) == LOW) { reversCH3 = !reversCH3; beep(); }
		if (digitalRead(plassButton) == LOW) { reversCH3 = !reversCH3; beep(); }
		lcd.setCursor(0, 1); lcd.print(reversCH3 ? myStrings[11] : myStrings[12]); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(5, reversCH3); key = 3; mix = 3; lcd_1 = 0; lcd_0 = 1; vst = 2; beep(); }
		break;
	case 14: //Revers CH4
		if (digitalRead(minusButton) == LOW) { reversCH4 = !reversCH4; beep(); }
		if (digitalRead(plassButton) == LOW) { reversCH4 = !reversCH4; beep(); }
		lcd.setCursor(0, 1); lcd.print(reversCH4 ? myStrings[11] : myStrings[12]); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(7, reversCH4); key = 3; mix = 4; lcd_1 = 0; lcd_0 = 1; vst = 2; beep(); }
		break;
	case 15: //Revers CH5
		if (digitalRead(minusButton) == LOW) { reversCH5 = !reversCH5; beep(); }
		if (digitalRead(plassButton) == LOW) { reversCH5 = !reversCH5; beep(); }
		lcd.setCursor(0, 1); lcd.print(reversCH5 ? myStrings[11] : myStrings[12]); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(9, reversCH5); key = 3; mix = 5; lcd_1 = 0; lcd_0 = 1; vst = 2; beep(); }
		break;
	case 16: //Revers CH6
		if (digitalRead(minusButton) == LOW) { reversCH6 = !reversCH6; beep(); }
		if (digitalRead(plassButton) == LOW) { reversCH6 = !reversCH6; beep(); }
		lcd.setCursor(0, 1); lcd.print(reversCH6 ? myStrings[11] : myStrings[12]); lcd.print("                ");
		if (digitalRead(menuButton) == LOW && mix == 1) { EEPROM.write(11, reversCH6); key = 3; mix = 6; lcd_1 = 0; lcd_0 = 1; vst = 2; beep(); }
		break;
	}

	if (pas >= 5) {
		flag = true;
		key = 1;
		lcd_0 = 1;
		vst = 0;
		pas = 0;
		lcd.clear();
		beep();
		delay(del);
	}

	if (lcd_0 == 0 && lcd_1 == 0 && key == 0) {
		lcd.setCursor(0, 0);
		lcd.print("ArduFly6");
		lcd.setCursor(14, 0);
		lcd.print(digitalRead(Right_tumb_2pos) != onTumbPos ? "X2" : "X1");
		lcd.setCursor(0, 1);
		lcd.print(digitalRead(Left_tumb_2pos) != onTumbPos ? "Arm   " : "Disarm");
	}

	//Send date
	if (digitalRead(Left_tumb_2pos) == onTumbPos) {   // Arm
		int LJV = constrain(analogRead(Left_joy_vertical), 95, 890);
		msg[2] = reversCH3 ? map(LJV, 95, 890, 180 - endPointCH3, endPointCH3) : map(LJV, 95, 890, endPointCH3, 180 - endPointCH3);
	} else {
		msg[2] = 0;
	}

	int ePCH1 = endPointCH1;
	int ePCH2 = endPointCH2;
	int ePCH4 = endPointCH4;
	int ePCH5 = endPointCH5;
	int ePCH6 = endPointCH6;

	if (digitalRead(Right_tumb_2pos) == onTumbPos) {
		ePCH1 = ePCH1 * 0.5;
		ePCH2 = ePCH2 * 0.5;
		ePCH4 = ePCH4 * 0.5;
		ePCH5 = ePCH5 * 0.5;
		ePCH6 = ePCH6 * 0.5;
	}

	if (reversCH1 == false) { // Eleron
		msg[0] = setPos(Right_joy_horizon, ePCH1);
	} else {
		msg[0] = setReversPos(Right_joy_horizon, ePCH1);
	}

	if (reversCH2 == false) { // Visota
		msg[1] = setPos(Right_joy_vertical, ePCH2);
	} else {
		msg[1] = setReversPos(Right_joy_vertical, ePCH2);
	}

	if (reversCH4 == false) { // Napravlena
		msg[3] = setPos(Left_joy_horizon, ePCH4);
	} else {
		msg[3] = setReversPos(Left_joy_horizon, ePCH4);
	}

	if (reversCH5 == false) { // CH5
		msg[4] = setPos(Left_knob, ePCH5);;
	} else {
		msg[4] = setReversPos(Left_knob, ePCH5);
	}

	if (reversCH6 == false) { // CH6
		msg[5] = setPos(Right_knob, ePCH6);
	} else {
		msg[5] = setReversPos(Right_knob, ePCH6);
	}

	radio.write(&msg, sizeof(msg));
}

void beep() {
	tone(buzzer, 2000);
	delay(100);
	noTone(buzzer);
	delay(200);
}

void save() {
	// Save to EEPROM
	EEPROM.write(0, endPointCH1);
	EEPROM.write(1, reversCH1);
	EEPROM.write(2, endPointCH2);
	EEPROM.write(3, reversCH2);
	EEPROM.write(4, endPointCH3);
	EEPROM.write(5, reversCH3);
	EEPROM.write(6, endPointCH4);
	EEPROM.write(7, reversCH4);
	EEPROM.write(8, endPointCH5);
	EEPROM.write(9, reversCH5);
	EEPROM.write(10, endPointCH6);
	EEPROM.write(11, reversCH6);

	beep(); 
	delay(100);
	beep();
}

void load () {
	// Load of EEPROM
	endPointCH1 = EEPROM.read(0);
	reversCH1 = EEPROM.read(1);
	endPointCH2 = EEPROM.read(2);
	reversCH2 = EEPROM.read(3);
	endPointCH3 = EEPROM.read(4);
	reversCH3 = EEPROM.read(5);
	endPointCH4 = EEPROM.read(6);
	reversCH4 = EEPROM.read(7);
	endPointCH5 = EEPROM.read(8);
	reversCH5 = EEPROM.read(9);
	endPointCH6 = EEPROM.read(10);
	reversCH6 = EEPROM.read(11);
}

void defset() {
	lcd.setCursor(3, 0);
	lcd.print("Loading...");
	for (int i = 0; i < EEPROM.length(); i++) 
		EEPROM.write(i, 0);

	load();
	lcd.clear();
	beep(); beep();
}
