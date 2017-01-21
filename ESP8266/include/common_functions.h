/*
 * common_functions.h
 *
 *  Created on: Jul 19, 2016
 *      Author: knataraj
 */

#ifndef INCLUDE_COMMON_FUNCTIONS_H_
#define INCLUDE_COMMON_FUNCTIONS_H_

// Debug printing functions
#define SERIAL 1
#define MQTT   2

void printTo(const String str, char source);

template<typename T, size_t N>
size_t size(T (&ra)[N]) {
    return N;
}

// GPIO functions
bool toggle_relay(unsigned relay_index);
bool set_relay(unsigned relay_index, bool _state);
bool get_relay(unsigned relay_index);
bool init_relays ();

#endif /* INCLUDE_COMMON_FUNCTIONS_H_ */
