#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <credentials.h>
#include "common_functions.h"

bool debug_mode = false;

//MQTT SSL
#define ENABLE_SSL

// Listen topics
// DEVICE_NAME/command
// main/command
// Publish
// DEVICE_NAME/response
// DEVICE_NAME/status
// main/status
// main/log

// Forward declarations
void startMqttClient();
void onMessageReceived(String topic, String message);

Timer procTimer;

// MQTT client
// For quickly check you can use: http://www.hivemq.com/demos/websocket-client/ (Connection= test.mosquitto.org:8080)
MqttClient mqtt(MQTT_SERVER, MQTT_PORT, onMessageReceived);

void printTo(const String str, char source) {
	if((source&SERIAL)==SERIAL || debug_mode) {
		Serial.println(str);
	}
	if ((source&MQTT)==MQTT) {
		mqtt.publish( DEVICE_NAME + "/response", str);
	}
}

void keepMQTTConnected()
{
	if (mqtt.getConnectionState() != eTCS_Connected)
		startMqttClient(); // Auto reconnect
}

// Publish our message
void publishStartMessage()
{
	//TODO put a for loop 10 times try and until connected.
	keepMQTTConnected();
	Serial.println("Let's publish message now!");
	//TODO debug this to know why this is not published.
	mqtt.publish("main/status", DEVICE_NAME + " connected"); // or publishWithQoS
	mqtt.publish(DEVICE_NAME + "/status", DEVICE_NAME + " connected");
	mqtt.publish(DEVICE_NAME + "/status", "Connected to SSID " + WifiStation.getSSID());
}

int CommandProcessor(String str, char source);
// Callback for messages, arrived from MQTT server
void onMessageReceived(String topic, String message)
{
	if(debug_mode) {
		Serial.print(topic);
		Serial.print(":\r\n\t"); // Prettify alignment for printing
		Serial.println(message);
	}
	if(topic == DEVICE_NAME + "/command" || topic == "main/command") {
		// TODO Reply in MQTT.
		if(!CommandProcessor(message, MQTT)) {
			//mqtt.publish(device_name+"/response","command "+ message + " failed");
		} else {
			if(debug_mode) {
				Serial.println("Command:" + message + " executed.");
			}
		}
	}
}

// Run MQTT client
void startMqttClient()
{
	mqtt.connect("esp8266", DEVICE_NAME, DEVICE_PASS, true);
#ifdef ENABLE_SSL
	//mqtt.addSslOptions(SSL_SERVER_VERIFY_LATER);

	#include <ssl/private_key.h>
	#include <ssl/cert.h>

	mqtt.setSslClientKeyCert(default_private_key, default_private_key_len,
							  default_certificate, default_certificate_len, NULL, true);

#endif
	mqtt.subscribe(DEVICE_NAME + "/command");
	mqtt.subscribe("main/command");
	//mqtt.subscribe("main/status/#");
	//mqtt.subscribe("main/status/+");
}

// Will be called when WiFi station was connected to AP
void wifiConnectOk()
{
	Serial.println("I'm CONNECTED");

	// Run MQTT client
	startMqttClient();

	// Start publishing loop
	publishStartMessage();
	procTimer.initializeMs(20 * 1000, keepMQTTConnected).start(); // every 20 seconds
}

void listWifiNetworks(bool succeeded, BssList list);
// Will be called when WiFi station timeout was reached
void wifiConnectFail()
{
	Serial.println("MQTT is NOT CONNECTED. Need help :(");

	// .. some you code for device configuration ..
	// Print available access points
	WifiStation.startScan(listWifiNetworks); // In Sming we can start network scan from init method without additional code
}

// Will be called when WiFi station network scan was completed
void listWifiNetworks(bool succeeded, BssList list)
{
	if (!succeeded)
	{
		Serial.println("Failed to scan networks");
		return;
	}

	for (int i = 0; i < list.count(); i++)
	{
		Serial.print("\tWiFi: ");
		Serial.print(list[i].ssid);
		Serial.print(", ");
		Serial.print(list[i].getAuthorizationMethodName());
		if (list[i].hidden) Serial.print(" (hidden)");
		Serial.println();
		//Check if saved Wifi network is available.
		for(int w=0; w<size(WIFI_SSIDS);w++) {
			if ( list[i].ssid == WIFI_SSIDS[w]) {
				// connect to wifi
				WifiStation.config(WIFI_SSIDS[w], WIFI_PASSD[w]);
				WifiStation.enable(true);
				Serial.println("Connecting to " + list[i].ssid);
				WifiStation.waitConnection(wifiConnectOk, 20, wifiConnectFail); // We recommend 20+ seconds for connection timeout at start
				return;
			}
		}
	}
}

// OTA functions
void OtaUpdate_CallBack(bool result) ;
void OtaUpdate(String URL) ;
void Switch() ;
//

void ShowInfo() {
    Serial.printf("\r\nSDK: v%s\r\n", system_get_sdk_version());
    Serial.printf("Free Heap: %d\r\n", system_get_free_heap_size());
    Serial.printf("CPU Frequency: %d MHz\r\n", system_get_cpu_freq());
    Serial.printf("System Chip ID: %x\r\n", system_get_chip_id());
    Serial.printf("SPI Flash ID: %x\r\n", spi_flash_get_id());
    Serial.printf("SPI Flash Size: %d\r\n", (1 << ((spi_flash_get_id() >> 16) & 0xff)));
}

int CommandProcessor(String str, char source) {
	if (str == "debug on") {
		debug_mode = true;
		printTo( "debug on", source);
	} else if (str == "debug off") {
		debug_mode = false;
		printTo( "debug off", source);
	} else if (str == "connect") {
		// connect to wifi
		WifiStation.config(WIFI_SSID, WIFI_PASS);
		WifiStation.enable(true);
		WifiStation.waitConnection(wifiConnectOk, 20, wifiConnectFail); // We recommend 20+ seconds for connection timeout at start
	} else if (str == "ip") {
			//Serial.printf("ip: %s mac: %s\r\n", WifiStation.getIP().toString().c_str(), WifiStation.getMAC().c_str());
			printTo( "ip: " + WifiStation.getIP().toString() + " mac: " + WifiStation.getMAC(), source);
	} else if (str == "rom") {
			printTo( "Currently running rom " + String(rboot_get_current_rom()), source );
	} else if (str.startsWith("ota ")) {
		printTo( "Stating OTA update", source);
		OtaUpdate(str.substring(4));
	} else if (str == "ota") {
		OtaUpdate("");
	} else if (str == "switch") {
		Switch();
	} else if (str == "restart") {
		System.restart();
	} else if (str == "status") {
		//connected wifi status, ssid, ip, mac
		//connected mqtt server status
		//currently running rom #
		//Relay status
	} else if (str.startsWith("OFF ") || str.startsWith("ON ") || str.startsWith("TOGGLE ") || str.startsWith("STATUS ")) {
		printTo("Switching devices.", source);
		//TODO implement GPIO
	} else if (str == "version") {
		printTo("V1.1", source);
		printTo(" Dated "+ String(__DATE__) + " Timed " + String(__TIME__), source);
	}  else if (str == "info") {
		ShowInfo();
	} else if (str == "help") {
		Serial.println();
		Serial.println("available commands:");
		Serial.println("  help - display this message");
		Serial.println("  ip - show current ip address");
		Serial.println("  connect - connect to wifi");
		Serial.println("  restart - restart the esp8266");
		Serial.println("  switch - switch to the other rom and reboot");
		Serial.println("  ota - perform ota update, switch rom and reboot");
		Serial.println("  rom - print currently running rom id");
		Serial.println("  version - print version and firmware compiled time");
		Serial.println("  info - show esp8266 info");
		Serial.println();
		printTo( "Available commands\r\nconnect, ip, rom, version, ota, switch, restart, OFF ##, ON ##, TOGGLE ##, STATUS ##, PING, help", source);
	} else {
		printTo("unknown command", source);
		return 0;
	}
	return 1;
}

int serialCommandProcessor(String str, char source) {
	return CommandProcessor( str, source);
}

void serialCallBack(Stream& stream, char arrivedChar, unsigned short availableCharsCount) {

	if (arrivedChar == '\n') {
		String str;
		str.reserve(availableCharsCount);
		char c;
		int done=0;
		for (int i = 0; i < availableCharsCount; i++) {
			c = (char) stream.read();
			if (c == '\n') {
				done=1;
			}
			if(!done && c!='\n' && c!='\r')
				str += c;
		}
		serialCommandProcessor(str, SERIAL);
	}
}

void init() {
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(debug_mode); // Debug output to serial

	// mount spiffs
	int slot = rboot_get_current_rom();
	debugf("spiffs disabled");
	WifiAccessPoint.enable(false);
	//WifiAccessPoint.config("Sming InternetOfThings", "", AUTH_OPEN);
	
	init_relays ();

	Serial.printf("\r\nCurrently running rom %d.\r\n", slot);
	Serial.println("Type 'help' and press enter for instructions.");
	Serial.println();
	
	Serial.setCallback(serialCallBack);

	// Run our method when station was connected to AP (or not connected)
	WifiStation.waitConnection(wifiConnectOk, 10, wifiConnectFail); // We recommend 20+ seconds for connection timeout at start
}
