# RPi-MQTT-Broker
---

# Introduction

MQTT (Message Queuing Telemetry Transport) is a simple, lightweight messaging protocol, designed for machines that may have bandwidth limitations or high latencies. A common use of this protocol is to communicate between small devices, such as microcontroller, to create a scalable, wireless system. MQTT defines two types of roles within the systems, the broker, and the clients. Client must connect to the broker and subscribe to various topics. Any messages sent from the client are passed to the broker, and then passed to all clients who are subscribed to the topic of the message. 

# Setup

As a broker, I am using a Raspberry Pi Model 2B running the [Mosquitto](https://mosquitto.org/) broker. However, many other options are available for brokers, such as [EMQX](https://www.emqx.com/en), more suitable for systems with many clients, or [NanoMQ](https://nanomq.io/), which takes advantage of multithreading and multi-core processors. Firstly the Moquitto broker had to be installed onto the RPi. In this case, I am using SSH to access a remote terminal to my Raspberry Pi: 

```bash
ssh username@hostname_or_ip
> sudo apt install -y mosquitto mosquitto-client
```

After the broker has finished downloading, you can use ```mosquitto -v``` to test if the installation was successful. For this usage of an MQTT broker, it is important for Mosquitto to begin running when the Raspberry Pi boots, so you must enable the Mosquitto service in systemd. Similarly you can check that current status of the Mosquitto service

```bash
> sudo systemctl enable mosquitto
> sudo systemctl status mosquitto
```

Finally, the configuration file for Mosquitto must be setup. Mosquitto has the capabilities to have either anonymous or authenticated communications. For a small IoT system, authentication was not necessary, so I configured Mosquitto to allow for anonymous connections. The configuration file is located at ```/etc/mosquitto/mosquitto.conf```, and two lines must be added at the bottom:

```bash
> sudo nano /etc/mosquitto/mosquitto.conf
```
Append these two lines:
```
allow_anonymous true
listener 1883
```
This first line enables the anonymous connections, and the second specifies the port the broker is listening on. Port 1883 is the default unencrypted channel, but port 8883 may be used for encrypted communications. For the same reasons as above, I will simply be using port 1883. The final step is the restart the Mosquitto service, to ensure the changes take place:
```bash
> sudo systemctl restart mosquitto
```

# IP Addresses

In order for clients to communicate with the broker, they must have the associated IP address of the broker. For Linux systems, such as a Raspberry Pi, you can obtain this by using ```hostname -I```. From experience, I learned that it is better to statically assign the IP address of the machine hosting the broker. When I began this project I was the only person in my apartment. After my roomates returned from Summer break, more devices connected to our internet. At some point my Raspberry Pi turned off and lost the IP address that all of the clients were connected to. As a result, I learned to statically assign the IP address of my Raspberry Pi. This was simple to do from the network settings page of my router. The only requirement is that the network must have a DHCP server, as this is the system that assigns IP addresses to connected devices.
