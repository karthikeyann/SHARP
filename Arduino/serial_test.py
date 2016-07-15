import serial
import time
import threading

serial_port = serial.Serial('COM8', 115200, timeout=0)
connected = True

def handle_data(data):
	if data!="":
		print "\nResp:",data

def read_from_port(ser):
	global connected
	while connected:
		reading = ser.readline().decode()
		handle_data(reading)
		time.sleep(1)

thread = threading.Thread(target=read_from_port, args=(serial_port,))
thread.start()

while 1:
	var = raw_input("")
	if var == "x" :
		break
	try:
		cmd = int(var, 16)
		cmdchr = chr(cmd)
	except ValueError:
		print "invalid code:",var
		continue
	serial_port.write(cmdchr)
	serial_port.write('\n')
	print 'Cmd:',var

connected = False
serial_port.close()
