# iot-intro
Intro presentation to the internet of things.

ABC, SIMPLE AS 1,2,3

***How to get started:***

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

***Run the presentation:***

Latidatida...
