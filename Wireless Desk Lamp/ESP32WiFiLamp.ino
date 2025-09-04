#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Verizon_LBQZ79";
const char* password = "lad-posy9-pet";
// Apt WiFi

const char* mqttBroker = "192.168.1.200"; // this is statically assigned, cool shit
const int mqttPort = 1883;
// MQTT info

WiFiClient espClient;
PubSubClient client(espClient);

static uint8_t ledState = 0;
static uint8_t ledPin = 2; 

static uint8_t relayPin = 22;
static uint8_t relayState = 0;

static uint8_t buttonPin = 15;
static const unsigned int debounceDelay = 50;
static unsigned int lastDebounceTime = 0;
static int buttonState = 1;
static int lastButtonState = 1;

void WiFiInit() {
  delay(10);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) { // Onboard LED slow flashes until the WiFi is connected
    if (ledState == 0) {
      ledState = 1;
      digitalWrite(ledPin, HIGH);
    } else {
      ledState = 0;
      digitalWrite(ledPin, LOW);
    }

    delay(500);
  }
  
  ledState = 0;
  for (int i = 0; i < 6; i++) { // Quick burst once it's connected
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(50);
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  String msg;

  Serial.println("Got message");

  for (int i = 0; i < length; i++) {
    msg += (char) message[i];
  }

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Msg: ");
  Serial.println(msg);

  if (String(topic) == "esp32/lamp") {
    if (msg == "on") {
      relayState = 1;
      digitalWrite(relayPin, HIGH);
    } else if (msg == "off") {
      relayState = 0;
      digitalWrite(relayPin, LOW);
    }
  }
}

void reconnect() { // loop to reconnect to MQTT broker
  while(!client.connected()) {
    Serial.println("Connecting to MQTT... ");
    String clientID = "esp32-client-";
    clientID += String(WiFi.macAddress());
    if (client.connect(clientID.c_str())) {
      client.subscribe("esp32/lamp");
    }
    if (ledState == 0) {
      ledState = 1;
      digitalWrite(ledPin, HIGH);
    } else {
      ledState = 0;
      digitalWrite(ledPin, LOW);
    }

    delay(500);
  }

  ledState = 0;
  for (int i = 0; i < 6; i++) { // Quick burst once it's connected
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(50);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // makes sure relay is off at first

  pinMode(buttonPin, INPUT_PULLUP);

  WiFiInit();
  client.setServer(mqttBroker, mqttPort); // Hosted on port 1883
  client.setCallback(callback); // callback function triggers when a message is received
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) { // button state has changed
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) { // not a bounce
    if (reading != buttonState) { // button is in a different state than before
      buttonState = reading;

      if (buttonState == LOW) { // pressed down, active low
        if (relayState == 0) relayState = 1;
        else if (relayState == 1) relayState = 0;

        digitalWrite(relayPin, relayState);
      }
    }
  }

  lastButtonState = reading;
}
