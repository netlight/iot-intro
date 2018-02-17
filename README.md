# iot-intro
Intro presentation to the internet of things.

ABC, SIMPLE AS 1,2,3...

***How to get started:***

**DEVICE:**
 - Ensure you have [PlatformIO Core](http://docs.platformio.org/en/latest/installation.html) installed on your local machine.
 - Connect the ESP8266 to your computer using a USB cable.
 - Edit the wifi and ip-address settings in the esp8266-dht/src/main.cpp code.
 - In the esp8266-dht folder of this repo run `platformio run --target upload`.
 - This should upload the device code to the ESP8266, you should see it's led flashing during the process.


**STORAGE:**
```
docker run -d -p 8083:8083 -p 8086:8086 \
 -e PRE_CREATE_DB="wadus" \
 --expose 8090 --expose 8099 \
 --name influxdb \
 tutum/influxdb
 ```

**PRESENTATION:**
```
docker run -d -p 3000:3000 \
 --link influxdb:influxdb \
 --name grafana \
 grafana/grafana
 ```

**TRANSFORMATION:**

```
docker run -ti -p 1883:1883 -p 9001:9001 toke/mosquitto
```


```
go run mqtt-influx-bridge/main.go
```

Import the grafana dashboard from grafana.json if you are feeling lazy...


***Rejoice...***
