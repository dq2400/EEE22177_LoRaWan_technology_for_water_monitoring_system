# EEE22177_LoRaWan_technology_for_water_monitoring_system
 A URECA project on LoRaWan

# Objectives
The objective of this project is to build a water monitoring system that monitors the water level and temperature with the constraints:
 - Explore different methods
 - Select the best method for use case
 - Create a working prototype
 - Send data via LoRaWan at an interval
 - Include time stamp of data collected
 - Easily scalable on the amount of data sent from sensors
 - Easily scalable on the number of sensors
 - Send data collected to a cloud
 - Long battery life

# Platform
There is a wide range of platforms that can be used to send data wirelessly, such as bluetooth, wifi and LoRaWan. 
The range of bluetooth is too small and has high power consumption.
The use of wifi and internet is costly to maintain although it can travel a further distance.
LoRaWan is of low maintainence cost and could be used for a long distances.

Therefore, LoRaWan was decided to be used.

# Transmitter
The Arduino MKR 1310 is used to send and receive data at the sensor end. This is beacuse of its high reliability and flexibility to program and send the data. 

# Receiver
As the MKR 1310 is used, there is a wide range of receivers/gateways available. The Things Network (TTN) provides commonly used gateways for both indoors and outdoors. Due to the limited and diminishing supply of such gateways, it is a concern if such gateways will be in operation for a long time. In addition, such gateways are very costly and requires a constant AC power supply.
As such, TTGO ESP32 with LoRa and WiFi was used as it is lower in cost and provides a greater flexibility like the Arduino MKR 1310. It is disadvantaged by the smaller range as compared to the TTN gateways.

# Cloud
In order to send the data from the ESP32 to the cloud, IF This Then That (IFTTT) was used to create an applet, and the ESP32 was connected to the internet. This allows the ESP32 to send the data via HTTP and JSON to a google sheet. This google sheet is used to store, process and redistribute the data collect.

# Important Notes
1_Antenna_Testing, 2_Sending_Cloud, 3_Power_Optimisation, 4_Data_Verification uses a DHT11 sensor as a simple sample sensor to measure temperature and humidity in the air. Since this porject is about water measurements, 5_Completed_System integrates all with the actual sensor.