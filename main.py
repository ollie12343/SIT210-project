from paho.mqtt import client as mqtt_client
import random
import time
import RPi.GPIO as GPIO
import threading


#pins
buzzer = 8
button = 10

GPIO.setmode(GPIO.BOARD)
GPIO.setup(buzzer, GPIO.OUT)
GPIO.setup(button, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)



broker = 'broker.emqx.io'
port = 1883
topic = "python/mqtt/ollie"
# Generate a Client ID with the publish prefix.
pub_client_id = f'publish-{random.randint(0, 1000)}'
sub_client_id = f'sub-{random.randint(0, 1000)}'



def connect_mqtt(client_id):
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def publish(client, msg):
	
    for i in range(2):
        time.sleep(1)
        result = client.publish(topic, msg)
        status = result[0]
        if status == 0:
            print(f"Send `{msg}` to topic `{topic}`")
        else:
            print(f"Failed to send message to topic {topic}")
        
        
def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        if msg.payload.decode() == "On" and msg.topic == "python/mqtt/ollie":
        	print("On")
        	GPIO.output(buzzer, GPIO.HIGH)
        	time.sleep(1)
        	GPIO.output(buzzer, GPIO.LOW)


    client.subscribe(topic)
    client.on_message = on_message


if __name__ == '__main__':

    sub = connect_mqtt(sub_client_id)
    subscribe(sub)
    
    t1 = threading.Thread(target=sub.loop_forever)

    	
    t1.start()
    toggle = False
    while True:
    	if GPIO.input(button) == GPIO.HIGH:
    		if not toggle:
    			publish(connect_mqtt(pub_client_id), "Unlock")
    			toggle = True
    		else:
    			publish(connect_mqtt(pub_client_id), "Lock")
    			toggle = False
    		
    t1.join()
    
    

    
    GPIO.cleanup()