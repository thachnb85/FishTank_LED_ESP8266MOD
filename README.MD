# 1. Intro
 - Fish tank LED controller with auto OFF at 9PM, auto ON at 6AM
 - Control LED On/Off via http

For example, IP of ESP8266 is 10.0.0.26

Switching ON the LED:

 ```
    http://10.0.0.26/led/on
 ```

 Switching OFF the LED:

  ```
     http://10.0.0.26/led/off
  ```

Change mode to AUTO (Off at 9PM, On at 6AM)

  ```
     http://10.0.0.26/led/auto
  ```
  
# 2. Hardware
 - ESP8266 MOD
 - LED String (5V with 5V, GND, Signal input)

# 3. Case
 - 3D printed from https://www.thingiverse.com/thing:5369444
