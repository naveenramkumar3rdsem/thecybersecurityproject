#include <types.h>
#include <uECC.h>
#include <uECC_vli.h>


//#include <Base64.h>

/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <CryptoAES_CBC.h>
#include <Crypto.h>
#include <AES.h>
#include <Base64.h>
#include <uECC.h>
AES128 aesciphertexts;
// Update these with values suitable for your network.
AES256 theaesciphertexts2;
byte aesKey[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0xcf, 0x15, 0x88, 0x09, 0xcf, 0x4f};
const char* ssid = "NaveensGalaxyA535G";
String thekeys="1111111111"; 
const char* password = "12345678";
const char* mqtt_server = "192.168.16.168";
byte thekeysoftheaesencryptions[16]={0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0xcf, 0x15, 0x88, 0x09, 0xcf, 0x4f};
byte theinitializationsvectors[16]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}; 
 // 16-byte key for AES-128
//byte key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0xcf, 0x15, 0x88, 0x09, 0xcf, 0x4f}; // 16-byte key for AES-128


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(64)
#define MSG_BUFFER_SIZE2 (128)
char msg[MSG_BUFFER_SIZE];
char msg2[MSG_BUFFER_SIZE2];
int value = 0;
uint8_t public1[64];
uint8_t private1[32];
const struct uECC_Curve_t * thecurves=uECC_secp256k1();
uint8_t thesecrets[32];
int theserversdatas=0;
int thedataspublished=0;
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/*
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();


  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    Serial.println("Led is on");
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    Serial.println("Led is off");
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED ofyf by making the voltage HIGH
  }

}
*/
uint8_t uint8Decodedarrays[64];
void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic, "servers")==0){
  if(theserversdatas==0){
  Serial.println("Entering the calbacks functions with the servers data=");
  Serial.println(theserversdatas);
  Serial.println(" and the data as ");
  Serial.println((char*)payload);
  Serial.print("\n");
  //Serial.print("Message received on topic:................................................................................ ");
  //Serial.println(topic);
  
  // Print the received payload (raw bytes)
  //Serial.print("Received ECC Public Key (raw bytes): ");
  //for (int i = 0; i < length; i++) {
  //  Serial.print(payload[i], HEX);  // Print the received raw byte as HEX
  //  Serial.print(" ");
  //}
  Serial.println();
  //int decodedLength = Base64.decodedLength((char*)payload, length);
  //uint8_t decodedKey[decodedLength];
  uint8_t uint8Decodedarrays2[length];
  Base64.decode((char*)uint8Decodedarrays2, (char*)payload, length);
  // Now you have the raw data in 'payload' as a byte array
  //uint8_t uint8Decoded[256];  // Array to store uint8_t values
  for(int i=0;i<64;i++){
    uint8Decodedarrays[i]=uint8Decodedarrays2[i];
  }
  // Copy the payload into the uint8_t array (if necessary, depending on your implementation)
  //for (int i = 0; i < length; i++) {
  //  uint8Decodedarrays[i] = payload[i];  // Copy the raw byte into uint8_t array
  //}

  // Print the uint8_t array (for debugging)
  //Serial.print("ECC Public Key (as uint8_t array): ");
  //for (int i = 0; i < length; i++) {
  //  Serial.print(uint8Decodedarrays[i], HEX);  // Print as HEX
  //  Serial.print(" ");
  //}
  Serial.println();
  uECC_make_key(public1, private1, thecurves);
  Serial.print("the public 1 ");
  for (int i = 0; i < 64; i++) {
    if (public1[i] < 0x10) { // Add leading zero for single-digit values
        Serial.print("0");
    }
    Serial.print(public1[i], HEX); // Print each byte as hexadecimal
}
Serial.print("\n");
Serial.print("the private 1 ");
  for (int i = 0; i < 32; i++) {
    if (private1[i] < 0x10) { // Add leading zero for single-digit values
        Serial.print("0");
    }
    Serial.print(private1[i], HEX); // Print each byte as hexadecimal
}
Serial.print("\n");
Serial.print("the uint8decodedarrays ");
  for (int i = 0; i < 64; i++) {
    if (uint8Decodedarrays[i] < 0x10) { // Add leading zero for single-digit values
        Serial.print("0");
    }
    Serial.print(uint8Decodedarrays[i], HEX); // Print each byte as hexadecimal
}
Serial.print("\n");
Serial.print("the uint8decodedarrays2 ");
  for (int i = 0; i < 64; i++) {
    if (uint8Decodedarrays2[i] < 0x10) { // Add leading zero for single-digit values
        Serial.print("0");
    }
    Serial.print(uint8Decodedarrays2[i], HEX); // Print each byte as hexadecimal
}
  int thenumbers=uECC_shared_secret(uint8Decodedarrays, private1, thesecrets, thecurves);

  theaesciphertexts2.setKey(thesecrets, 32);
  theserversdatas=1;
  Serial.println("the serversdatas is made as 1");
  Serial.print("Shared secret (HEX): ");
  for (int i = 0; i < 32; i++) {
    if (thesecrets[i] < 0x10) { // Add leading zero for single-digit values
        Serial.print("0");
    }
    Serial.print(thesecrets[i], HEX); // Print each byte as hexadecimal
}
Serial.println();
  // Now you can use the `uint8Decoded` array for further ECC operations
  // For example, you could use `uint8Decoded` as the public key in elliptic curve operations
  }
  }
  else if(strcmp(topic,"serversdecrypteddata")==0){
    if(theserversdatas==1){
      Serial.println("Now the encrypted data has come and the servers datas is 1 and the datas published is 0");
      theserversdatas=0;
      thedataspublished=0;
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe("inTopic");
      client.subscribe("servers");
      client.subscribe("serversdecrypteddata");
      Serial.println("Subscribed to the servers public keys");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
static int RNG(uint8_t *dest, unsigned size) {
  while (size) {
    *dest = random(0, 256); // Generate a random byte (0 to 255)
    ++dest;
    --size;
  }
  return 1; // Return 1 to indicate success
}
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(D2,INPUT);
  pinMode(D1,OUTPUT);
  aesciphertexts.setKey(thekeysoftheaesencryptions,  16);
  uECC_set_rng(&RNG);
}
//int theintegers=0;
void loop() {
  //uint8_t public1[64];
  //uint8_t private1[32];
  //const struct uECC_Curve_t * thecurves=uECC_secp256r1;
  //uECC_make_key(public1, private1, thecurves);
  //uint8_t thesecrets[32];
  //int thenumbers=uECC_shared_secret(uint8Decodedarrays, private1, thesecrets, thecurves);
  //theaesciphertexts2.setKey(thesecrets, 32);
  //client.publish("the public keys of the node mcu esp8266", public1);


  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(Serial.available()>0){
  //theintegers=Serial.read()-48;
  //Serial.println("theintegers="+String(theintegers));
  }
  //pinMode(10,INPUT);
  //    pinMode(12,OUTPUT);
      digitalWrite(D1,LOW);
      delay(2);
      digitalWrite(D1,HIGH);
      delay(10);
      digitalWrite(D1,LOW);
      //pinMode(10,INPUT);
      float time=pulseIn(D2,HIGH);
      float val=340.0*time*(1/10000.0)/2.0;
      //val=509.0;
      //val=59343.00;
      //val=57373833.00;
      //val=1201.118042;
      //val=1;
      //val=120;
      //val=34333439.56;
      //val=120.56;
      snprintf (msg, MSG_BUFFER_SIZE, "%f",val);
      snprintf(msg2, MSG_BUFFER_SIZE2, "%f", val);
      //Serial.println("Publish the message");
      //Serial.print(val);
      unsigned char encrypted[sizeof(msg)];
      unsigned char encrypted2[sizeof(msg2)];
      unsigned char * theoriginalmessages=(unsigned char *)msg;
      unsigned char * theoriginalmessages2=(unsigned char *)msg2;
      //int inputStringLength = strlen(inputString);
      //int encodedLength = Base64.encodedLength(inputStringLength);
      //char encodedString[encodedLength + 1];
      //Base64.encode(encodedString, inputString, inputStringLength);
      aesciphertexts.encryptBlock(encrypted, (const uint8_t *)theoriginalmessages);
      theaesciphertexts2.encryptBlock(encrypted2, (const uint8_t *)theoriginalmessages2);
      int therunningvariables;
      //Serial.print("the length of the cipher texts is");
      //Serial.print(sizeof(encrypted)/sizeof(encrypted[0]));
      //Serial.print("\n");
      //for(therunningvariables=0;therunningvariables<50;therunningvariables++){
      //  Serial.print(encrypted[therunningvariables], HEX);
      //  Serial.print(" ");
      //}
      Serial.print("\n");
      int inputStringLength = sizeof(encrypted);
      int inputStringLength2=sizeof(encrypted2);
      int inputStringLength3=sizeof(public1);
      int encodedLength = Base64.encodedLength(inputStringLength);
      int encodedLength2=Base64.encodedLength(inputStringLength2);
      int encodedLength3=Base64.encodedLength(inputStringLength3);
      //int encodedLength3=Bas
      char encodedString[encodedLength + 1];
      char encodedString2[encodedLength2+1];
      char encodedString3[encodedLength3+1];
      Base64.encode(encodedString, (char *)encrypted, inputStringLength);
      Base64.encode(encodedString2, (char *)encrypted2, inputStringLength2);
      Base64.encode(encodedString3, (char *)public1, inputStringLength3);
      Serial.print("\n");
      //for(therunningvariables=0;therunningvariables<strlen(encodedString);therunningvariables++){
      //  Serial.print(encodedString[therunningvariables]);
      //  Serial.print(" ");
      //}
      Serial.print("\n");
      //Serial.print("before decoding is ");
      //Serial.print(encodedString);
      //Serial.print("\n");
      client.publish("outTopic", (const char *)encodedString);
      if(theserversdatas==1 && thedataspublished==0){
        client.publish("esp8266", (const char *)encodedString3);
        Serial.println("Published to the topics esp 8266");
        client.publish("outTopic3", (const char *)encodedString2);
        Serial.println("Published to the outtopics3");
        thedataspublished=1;
        //theserversdatas=0;
        Serial.println("Encoded the strings and published them and the servers datas is 0");
      }
      char thedecodedstrings[inputStringLength];
      Base64.decode(thedecodedstrings, encodedString, encodedLength);
      //Serial.print("the encoded string is ");
      //Serial.print(encodedString);
      Serial.print("\n");
      //Serial.print("The decoded strings");
      //Serial.print("\n");
      //for(therunningvariables=0;therunningvariables<strlen(thedecodedstrings);therunningvariables++){
      //  Serial.print(thedecodedstrings[therunningvariables]);
      //  Serial.print(" ");
      //}
      //Serial.print("\n");
      unsigned char theunencryptedstrings[sizeof(msg)];
      aesciphertexts.decryptBlock(theunencryptedstrings, (const uint8_t *)thedecodedstrings);
      //Serial.print("Publishing the first topic ");
      client.publish("theouttopics",(const char *)msg);
      //Serial.print("Publishing the second topic ");
      //Serial.print("(");
      //Serial.print(encodedString);
      //Serial.print(")");
      //client.publish("outTopic", (const char *)encodedString);
      //Serial.print("Publishing the third topic");
      client.publish("theintopics",(const char *)theunencryptedstrings);
      Serial.print("\n");
      int ledState = digitalRead(BUILTIN_LED); // Read the state of the built-in LED
  //if (ledState == LOW) {
  //  client.publish("theouttopics","0");
  //} else {
  //  client.publish("theouttopics", "1");
  //}
      /*lcd.clear();
      lcd.setCursor(0,0);	
      lcd.print("DISTANCE=");
      lcd.setCursor(0,1);
      lcd.print(val);
      lcd.print("CM");
    }*/
  //unsigned long now = millis();
  //if (now - lastMsg > 2000) {
  //  lastMsg = now;
  //  ++value;
    //snprintf (msg, MSG_BUFFER_SIZE, "%d hello world #%ld",theintegers, value);
    //Serial.print("Publish message: ");
    //Serial.println(msg);
    //client.publish("outTopic", msg);
    //client.publish("inTopic",msg);
    //if(theintegers==0){
    //  theintegers=1;
    //}
    //else{
    //  theintegers=0;
    //}
  //}
}
