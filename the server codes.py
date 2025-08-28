import paho.mqtt.client as mqtt
from ecdsa import SECP256k1, ellipticcurve, util
from ecdsa import SigningKey, VerifyingKey
from Crypto.Cipher import AES
import base64
import threading
import time
broker_ip = "127.0.0.1"
#topic = "theintopics"
client = mqtt.Client()
node_mcu_public_key = None  # This will be populated when a message is received
shared_secret = None
server_private_key=None
server_public_key=None
allowthepublishings=True
def publish_keys_periodically():
    global server_public_key, allowthepublishings
    while True:
        #print("the server's public key is ",base64.b64encode(server_public_key).decode('utf-8'))
        if server_public_key and allowthepublishings==True:
            #print("The server public key is ", server_public_key)
            #print(len(list(server_public_key)), len(server_public_key.hex()))
            server_public_key_base64 = base64.b64encode(server_public_key).decode('utf-8')
            #print("Publishing the keys ",server_public_key_base64)
            client.publish("servers", server_public_key_base64)
            #print("Publishing public key...")
        time.sleep(0.5)  # Wait for 5 seconds before publishing again
def calculate_shared_secret(private_key, peer_public_key_bytes):
    #print("Entered into the shared secrets functions .......................................")
    peer_public_key = VerifyingKey.from_string(peer_public_key_bytes, curve=SECP256k1)
    shared_secret_point = private_key.privkey.secret_multiplier * peer_public_key.pubkey.point
    # Use only the x-coordinate of the shared point as the shared secret
    shared_secret = shared_secret_point.x().to_bytes(32, byteorder="big")
    #print("the shared secret is ", shared_secret)
    print("Shared Secret (Hex):", shared_secret.hex())
    return shared_secret
def generate_keys():
    private_key = SigningKey.generate(curve=SECP256k1)
    public_key = private_key.get_verifying_key()
    #print("We have made the keys")
    return private_key, public_key.to_string()

def on_connect(client, userdata, flags, rc):
    global server_private_key
    global server_public_key
    global allowthepublishings
    #print("The topics are coming .........................................................................")
    print(f"Connected with result code: {rc}")
    #client.subscribe(topic)
    client.subscribe("outTopic3")
    client.subscribe("esp8266")
    server_private_key, server_public_key = generate_keys()
    server_public_key_base64 = base64.b64encode(server_public_key).decode('utf-8')
    print("Publishing the keys ",''.join(f'{byte:02X}' for byte in server_public_key))
    print("The base 64 is ", server_public_key_base64)
    client.publish("servers", server_public_key_base64)
    allowthepublishings=True
    key_publish_thread = threading.Thread(target=publish_keys_periodically, daemon=True)
    key_publish_thread.start()

def on_message(client, userdata, msg):
    #print(f"Received message: {msg.payload.decode()} from the topic................................", msg.topic,"\n\n")
    global node_mcu_public_key
    global shared_secret
    global server_private_key
    global server_public_key
    global allowthepublishings
    if(msg.topic=="esp8266"):
        #print("We have received the node mcus public keys which is ",base64.b64decode(msg.payload).hex())
        node_mcu_public_key=base64.b64decode(msg.payload) # Decode the public key from base64
        #print("The servers private keys is",server_private_key, node_mcu_public_key)
        shared_secret = calculate_shared_secret(server_private_key, node_mcu_public_key)
    if(msg.topic=="outTopic3" and shared_secret!=None):
        allowthepublishings=False
        #print("the publishing is stopped.............................")
        base64_ciphertext = msg.payload
        ciphertext = base64.b64decode(base64_ciphertext)
        cipher = AES.new(shared_secret, AES.MODE_ECB)
        decrypted_data = cipher.decrypt(ciphertext)
        #print("The decrypted data from the servers is", decrypted_data)
        client.publish("serversdecrypteddata",decrypted_data)
        print("The decrypted data from the servers is", decrypted_data)
        server_private_key, server_public_key=generate_keys()
        server_public_key_base64 = base64.b64encode(server_public_key).decode('utf-8')
        print("Publishing the keys ",''.join(f'{byte:02X}' for byte in server_public_key))
        print("The base 64 is ", server_public_key_base64)
        client.publish("servers", server_public_key_base64)
        #print("The publishing is going to start and the server's public key is changed...........")
        allowthepublishings=True
client.on_connect = on_connect
client.on_message = on_message

# Connect to the broker
client.connect(broker_ip)

# Start the MQTT client loop
client.loop_forever()
