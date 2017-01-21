/*
 * OTA_functions.cpp
 *
 *  Created on: Jul 19, 2016
 *      Author: Karthikeyan Natarajan
 */

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "common_functions.h"

rBootHttpUpdate* otaUpdater = 0;

void OtaUpdate_CallBack(bool result) {

	printTo("In callback...", SERIAL);
	if(result == true) {
		// success
		uint8 slot;
		slot = rboot_get_current_rom();
		if (slot == 0) slot = 1; else slot = 0;
		// set to boot new rom and then reboot
		printTo("Firmware updated, rebooting to rom " + String(slot), SERIAL);
		printTo("Firmware updated, rebooting to rom " + String(slot), MQTT);
		rboot_set_current_rom(slot);
		System.restart();
	} else {
		// fail
		printTo("Firmware update failed!", SERIAL);
		printTo("Firmware update failed!", MQTT);
	}
}

void OtaUpdate(String URL="") {

	uint8 slot;
	rboot_config bootconf;

	Serial.println("Updating...");

	// need a clean object, otherwise if run before and failed will not run again
	if (otaUpdater) delete otaUpdater;
	otaUpdater = new rBootHttpUpdate();

	// select rom slot to flash
	bootconf = rboot_get_config();
	slot = bootconf.current_rom;
	if (slot == 0) slot = 1; else slot = 0;

#ifndef RBOOT_TWO_ROMS
	if (URL == "")
		URL = ROM_0_URL;
	else
		URL = URL + "/rom0.bin";
#else
	// flash appropriate rom
	if (slot == 0) {
		if (URL == "")
			URL = ROM_0_URL;
		else
			URL = URL + "/rom0.bin";
	} else {
		if (URL == "")
			URL = ROM_1_URL;
		else
			URL = URL + "/rom1.bin";
	}
#endif
	printTo("update URL: " + URL, SERIAL|MQTT);
	// flash rom to position indicated in the rBoot config rom table
	otaUpdater->addItem(bootconf.roms[slot], URL);

#ifndef DISABLE_SPIFFS
	// use user supplied values (defaults for 4mb flash in makefile)
	if (slot == 0) {
		otaUpdater->addItem(RBOOT_SPIFFS_0, SPIFFS_URL);
	} else {
		otaUpdater->addItem(RBOOT_SPIFFS_1, SPIFFS_URL);
	}
#endif

	// request switch and reboot on success
	//otaUpdater->switchToRom(slot);
	// and/or set a callback (called on failure or success without switching requested)
	otaUpdater->setCallback(OtaUpdate_CallBack);

	// start update
	otaUpdater->start();
}

void Switch() {
	uint8 before, after;
	before = rboot_get_current_rom();
	if (before == 0) after = 1; else after = 0;
	printTo("Swapping from rom " + String(before) + " to rom " + String(after), SERIAL);
	printTo("Swapping from rom " + String(before) + " to rom " + String(after), MQTT);
	rboot_set_current_rom(after);
	printTo("Restarting...\r\n", SERIAL);
	printTo("Restarting...\r\n", MQTT);
	//TODO wait for sometime before restart to know that publish succeeded.
	System.restart();
}

