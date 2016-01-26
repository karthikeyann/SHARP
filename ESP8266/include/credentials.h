#ifndef __CREDENTIALS_H__
#define __CREDENTIALS_H__

#ifdef __cplusplus
extern "C" {
#endif

// WIFI Network connects to when 'connect' command is given.
#ifndef WIFI_SSID
	#define WIFI_SSID "wifiName" // Put you SSID and Password here
	#define WIFI_PASS "password"
#endif
// List of SSIDs to connect to if last connected network is not found.
String WIFI_SSIDS[4] = {"ssid1", "ssid2", "ssid3", "ssid4"};
String WIFI_PASSD[4] = {"pass1", "pass2", "pass3", "pass4"};

// MQTT Server name and port.
#define MQTT_SERVER "m1020.cloudmqtt.com"
#define MQTT_PORT 10781
// MQTT user credentials
String DEVICE_NAME = "devicename";
String DEVICE_PASS = "password";


#ifdef __cplusplus
}
#endif

#endif
