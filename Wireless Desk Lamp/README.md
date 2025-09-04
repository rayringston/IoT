# Wireless Desk Lamp

This project uses an ESP32 microcontroller to act as a client to a MQTT broker. The ESP32 uses its GPIO pins to control a relay, switching the state of a desk lamp. It also features an external push button, allowing you to manually control the lamp. The client can be communicated with via a smartphone app, [IoT MQTT Panel](https://apps.apple.com/pl/app/iot-mqtt-panel/id6466780124).

# How it Works
The MQTT broker is hosted by a Raspberry Pi, see [RPi MQTT Broker](IoT/RPi%20MQTT%20Broker) for more information, while the client is an ESP32. The ESP32 

## MQTT Client
The most important component of this project, was the ability to control the lamp wirelessly. To achieve this, I opted to use MQTT, since it is designed for small, low-distance networks of devices. A Raspberry Pi Model 2B is hosting a 
## Relay Switching
The relay is connected to an output pin on the ESP32 board, and is on a shield that includes a flyback diode and an optocoupler for reverse current protection. The relay is controlled by both the MQTT connection and the physical button on the lamp. The state of the relay is toggled by a button press, and can specific by an MQTT message.

## Button Control
The button was the last feature included, and was initially an oversight of mine. Since I removed the physical switch on the lamp and replaced it with a relay, the lamp could only be controlled wirelessly, and as such was useless without an internet connection. The button is simply connected between GND and a GPIO pin of the ESP32, which must be configured to use the interal pullup resistor.

Physical buttons tend to have a short period, called a bounce, where the signal from the button is unstable. In this bounce, there may be multiple instances where the button goes LOW-HIGH-LOW, and button presses may be counted multiple times with one press. The following code is my implentation of debouncing the push button:>

```c++
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
```
The ```debounceDelay``` variable is set to 50 ms, and is the minumum amount of time between input changes to register a button press. Since the bouncing only happens for a few milliseconds, all of the addtional signal changes will be ingored.


# Previous Iterations
The first iteration of the project was implemented using an HTTP server, hosted on an Arduino MKR 1000 WiFi. This system was unnecessarily complicated for a small network of devices, and using HTTP to communicate had too much latency. At times, it could take 2-3 tries just to turn the lamp on or off. 

As an attempt to reduce latency, I replaced the Arduino MKR 1000 WiFi with a Raspberry Pi 2, which has considerably more processing power. This iteration successfully reduced the latency issue, but presented more problems. First, the Raspberry Pi must be configured a certain way to running services on startup, and for this system there are additional constraints, such as a WiFi connection being established before the system begins. However, other issues did not have such a solution. The Raspberry Pi takes long to boot, so if the lamp is unplugged while not in use, it will take 1-2 minutes just to start working. And since it was still hosting an HTTP server, it was overcompicated.

This final iteration has solved the remainder of these issues. The Raspberry Pi is now being used as the MQTT broker, which means it will not be turned on as often. The ESP32 has greater processing power than the Arduino MKR 1000, and MQTT is much more lightweight than HTTP, resulting in a much lower latency. And using MQTT removes the need to use a web browser just to turn a lamp on and off.
