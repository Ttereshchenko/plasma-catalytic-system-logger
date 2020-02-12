import paho.mqtt.client as mqtt_client
import time
import os
import json

Connected = False  # global variable for the state of the connection

FILE_OUT = open(f'/data_out/{os.environ.get("OUT_PUT_FILE_NAME")}.csv', 'w+')
HEADER = os.environ.get('HEADER').split(',')
print(str(HEADER))
FILE_OUT.write(','.join(HEADER))
FILE_OUT.write('\n')
FILE_OUT.flush()


def on_connect(client, userdata, flags, rc):
	if rc == 0:
		print("Connected to broker")
		global Connected  # Use global variable
		Connected = True  # Signal connection
	else:
		print("Connection failed")


def on_message(client, userdata, message):
	msg = json.loads(message.payload)
	print("Message received: " + str(msg))
	global FILE_OUT, HEADER
	FILE_OUT.write(','.join([f'"{msg[k]}"' for k in HEADER]))
	FILE_OUT.write('\n')
	FILE_OUT.flush()


def on_log(client, userdata, level, buf):
	print("log: ", buf)

# broker_address = "localhost"
# port = 1883
# user = "test"
# password = "test"


broker_address = os.environ.get('MQTT_HOST')
port = int(os.environ.get('MQTT_PORT'))
user = os.environ.get('MQTT_USER_NAME')
password = os.environ.get('MQTT_USER_PASS')


client = mqtt_client.Client("Python")  # create new instance
client.username_pw_set(user, password=password)  # set username and password
client.on_log = on_log

client.on_connect = on_connect  # attach function to callback
client.on_message = on_message  # attach function to callback

client.connect(broker_address, port=port)  # connect to broker

client.loop_start()  # start the loop

while Connected:  # Wait for connection
	time.sleep(0.1)
client.subscribe("esp32/temperature")
try:
	while True:
		time.sleep(1)

except KeyboardInterrupt:
	client.disconnect()
	client.loop_stop()
	FILE_OUT.close()
