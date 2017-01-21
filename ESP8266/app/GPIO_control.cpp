/*
 * GPIO_control.cpp
 *
 *  Created on: Jul 19, 2016
 *      Author: Karthikeyan Natarajan
 */

#include <SmingCore/SmingCore.h>
#define N_RELAYS 5
unsigned RELAY_PINS[N_RELAYS] = { 4, 5, 12, 13, 14 };
bool state[N_RELAYS] = {true, true, true, true, true};

bool toggle_relay(unsigned relay_index) {
	if (relay_index < N_RELAYS) {
		digitalWrite(RELAY_PINS[relay_index], state[relay_index]);
		state[relay_index] = !state[relay_index];
		//TODO send to MQTT with retain
		return true;
	}
	return false;
}

bool set_relay(unsigned relay_index, bool _state)
{
	if (relay_index < N_RELAYS) {
		digitalWrite(RELAY_PINS[relay_index], _state);
		state[relay_index] = _state;
		//TODO send to MQTT with retain
		return true;
	}
	return false;
}

bool get_relay(signed relay_index)
{
	if (relay_index < N_RELAYS) {
		return state[relay_index] ;
	}
	return false;
}
//DROPPED read last status from SPIFFS filesystem.

bool init_relays () {
	//TODO get from device01/status in MQTT server with retain.
	for(unsigned ri=0; ri<N_RELAYS; ri++) {
		pinMode(RELAY_PINS[ri], OUTPUT);
		pullup(RELAY_PINS[ri]);
		digitalWrite(RELAY_PINS[ri], state[ri]);
	}
	return true;
}
