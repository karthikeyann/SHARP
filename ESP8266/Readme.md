# ESP8266 code for SHARP

Device Code
-------
The code connected to preset list of wifi networks in the code and connected to MQTT server.
Then I can receive command from MQTT server. Make sure, you set the correct credentials before compiling. 

For help, send command 'help' in topic 'devicename/command' or 'main/command' from MQTT server.
For debugging, you can connect directly to serial port and send commands. 

		available commands:
		  help - display this message
		  ip - show current ip address
		  connect - connect to wifi
		  restart - restart the esp8266
		  switch - switch to the other rom and reboot
		  ota - perform ota update, switch rom and reboot
          ota http://#ip - perform ota update from this server ip
		  rom - print currently running rom id
          version - to print version and compile date, time.
		  info - show esp8266 info
          ON ##  - switch on  the relay ## (## can be 01, 02, 03, 04)
          OFF ## - switch off the relay ## (## can be 01, 02, 03, 04)


#### TODO
- Upload schematics (ESP8266, Arduino & Relay)
- Commands upgrade (ESP8266))
- Upload Android App code 
- Design Front end web UI.


rBoot Sming   
-------
This is readme from rBoot Sming sample. I kept it to help configure rBoot for your ESP module.

This sample integrates rBoot and Sming, for the many people who have been asking
for it. It demonstrates dual rom booting, big flash support, OTA updates and
dual spiffs filesystems. You must enable big flash support in rBoot and use on
an ESP12 (or similar device with 4mb flash). When using rBoot big flash support
with multiple 1mb slots only one rom image needs to be created. If you don't
want to use big flash support (e.g. for a device with smaller flash) see the
separate instructions below. You can easily take the ota files and add them to
your own project to add OTA support.

To build any Sming project (in threory) with rBoot support you should use
Makefile-rboot.mk instead of Makefile-project.mk. In the sample the correct mk
file is chosen automatically because of the setting RBOOT_ENABLED=1 in
Makefile-user.mk

Building
--------
 1) Set ESP_HOME & SMING_HOME, as environment variables or edit Makefile-user.mk
    as you would for general Sming app compiling.
 2) Set ESPTOOL2 (env var or in Makefile-user.mk) to point to the esptool2
    binary. Source for esptool2 is [here](https://github.com/raburton/esp8266)
 3) Set WIFI_SSID & WIFI_PWD environment variables with your wifi details.
 4) Edit the OTA server details in include/user_config.h
 5) Check overridable variables in Makefile-user.mk, or set as env vars.
 6) make && make flash
 7) Put rom0.bin and spiff_rom.bin in the root of your webserver for OTA.
 8) Interact with the sample using a terminal, sorry no web-gui (yet).

Flashing
--------
If flashing manually use esptool.py to flash rBoot, rom & spiffs e.g.:
 esptool.py --port <port> write_flash -fs 32m 0x00000 rboot.bin 0x02000 rom0.bin
   0x100000 spiffs.rom

Using the correct -fs parameter is important. This will be -fs 32m on an ESP12.

You can also flash rom0.bin to 0x202000, but booting and using OTA is quicker!

In the current setup, the ``rom1.ld`` is modified to suit for ESP-01 512KB version. 

Technical Notes
---------------
spiffs_mount_manual(address, length) must be called from init. The address must
be 0x40200000 + physical flash address. Sming does not use memory mapped flash
so the reason for this strange addressing is not clear.

Important compiler flags used:
BOOT_BIG_FLASH - when using big flash mode, ensures flash mapping code is built
  in to the rom.
RBOOT_INTEGRATION - ensures Sming specific options are pulled in to the rBoot
  source at compile time.
SPIFF_SIZE=value - passed through to code for mounting the filesystem. Also used
  in the Makefile to create the spiffs.

Disabling big flash
-------------------
If you want to use, for example, two 512k roms in the first 1mb block of flash
(old style) then follow these instructions to produce two separately linked
roms. If you are flashing a single rom to multiple 1mb flash blocks (using big
flash) you only need one linked rom that can be used on each.

This assumes you understand the concepts explained in the rBoot readme about
memory mapping and setting linker script address. This is not covered here, just
how to use this sample without bigflash support.

- Copy ``rom0.ld`` to ``rom1.ld``.
- Adjust the rom offsets and length as appropriate in each ld file.
- Uncomment 'RBOOT_TWO_ROMS ?= 1' in ``Makefile-user.mk`` (or set as an environment
  variable).
- Ensure RBOOT_BIG_FLASH is set to 0 in ``Makefile-user.mk``
- If using a very small flash (e.g. 512k) there may be no room for a spiffs
  fileystem, disable it with DISABLE_SPIFFS = 1
- If you are using spiffs set RBOOT_SPIFFS_0 & RBOOT_SPIFFS_1 to indicate where
  the filesystems are located on the flash. This is the real flash offset, not
  the address + 0x40200000 used in the mount call.
- After building copy all the rom*.bin files to the root of your web server.

If you want more than two roms you must be an advanced user and should be able
to work out what to copy and edit to acheive this!

Credits
-------
This rBoot code is taken and modified from SMING rBoot sample code.