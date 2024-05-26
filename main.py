import cv2
from paho.mqtt import client as mqtt_client
import random
import time
import threading

# python 3.11

import random
import time

from paho.mqtt import client as mqtt_client


broker = 'broker.emqx.io'
port = 1883
topic = "python/mqtt/ollie"
# Generate a Client ID with the publish prefix.
pub_client_id = f'publish-{random.randint(0, 1000)}'
sub_client_id = f'sub-{random.randint(0, 1000)}'
# username = 'emqx'
# password = 'public'

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


def publish(client):
    msg_count = 1
    while True:
        time.sleep(1)
        msg = f"messages: {msg_count}"
        result = client.publish(topic, msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            print(f"Send `{msg}` to topic `{topic}`")
        else:
            print(f"Failed to send message to topic {topic}")
        msg_count += 1
        if msg_count > 5:
            break
        
        
def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

    client.subscribe(topic)
    client.on_message = on_message

def pub():
    pub = connect_mqtt(pub_client_id)
    
    pub.loop_start()
    publish(pub)
    pub.loop_stop()
    
def sub():
    sub = connect_mqtt(sub_client_id)
    subscribe(sub)
    sub.loop_forever()


if __name__ == '__main__':
    
    sub()
    
    # t1 = threading.Thread(pub)
    # t2 = threading.Thread(sub)
    
    # t2.start()
    # t1.start()
    
    # t1.join()
    # t2.join()


    
"""
image = cv2.imread("test.png")

cv2.imshow("Display", image)

cv2.waitKey(0)

cv2.destoryAllWindows()
"""