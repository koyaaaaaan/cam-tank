日本語話者のかたには [README.ja.md](/README.ja.md) を用意してあります。  
If you are Japanese speaker, we have [README.ja.md](/README.jp.md) for you.

# Cam Tank
Cam tank is Wifi control tank with camera streaming.

<img src="./camtank.jpg" width="480px" />  
  
<img src="./manubat.png" width="360px" />

# Implementation

## Components
This project need to components below.

* ESP32-CAM
* TANK Kit with Dual Motor
* 2ch Motor Driver 
* Battery x 2 
* Servo Motor x 2 (Optional but... I don't recommend.)
* Cam Servo Mount (Optional but... I don't recommend.)

ESP32<br />
<img src="./esp32cam.jpg" width="240px" /><br />
TANK Kit <br />
<img src="./tamiya_tank_basic.jpg" width="240px" /><br />
Dual Motor<br />
<img src="./tamiya_dual_motor.jpg" width="240px" /><br />
Motor Driver<br />
<img src="./motordriver.jpg" width="240px" /><br />
Cam Servo Mount <br />
<img src="./servomount.jpg" width="240px" /><br />

## ESP32 SetUP
### Install Source Code
1. Clone Git Project in your PC.<br />
Exec git clone command to download souce code in your pc.<br />
`git clone https://github.com/koyaaaaaan/cam-tank`<br />
<br />
<br />

2. Connect ESP32 and PC with Serial.<br />
In case of use USB Serial, connect like this.  
  
Connect each row.  <br />
| USB Serial | ESP32 CAM |
| ---------- | --------- | 
| 5V         | 5V        |
| RX         | UoT       |
| TX         | UoR       | 
| GND        | GND       |  

ESP32 Self Connection (This is for Installing the program) <br />
Connect each row.  <br />
| ESP32 CAM   | ESP32 CAM |
| ----------- | --------- |
| IO 0 (ZERO) | GND       |
<br />
<br />

3. Open Source Code with Arduino IDE<br />
Open camtank.ino file.<img src="./arduinoide.jpg" width="240px" />
<br />
<br />

4. Compile and Install<br />
Do "Upload" command on Arduino IDE.<br /> 
When you see "Connecting..." in executing console, you may need to push reset button on ESP32-CAM to start installing.
<br />
<br />

5. Finish  
Remove the connection between GND and IO0(ZERO).
<br />
<br />
<br />

## Motor Circuit Implements
### Electric Circuit
Make circuit like this.  
*Note: Motor Driver may be diffirent behavior. Then please change connection.*
  
Connect each row.
| ESP32 CAM | Motor Driver           | Battery         | Motor           | 
| --------- | ---------------------- | --------------- | --------------- | 
| IO2       | INT1 (MotorA Forward)  |                 |                 | 
| IO14      | INT2 (MotorA Backward) |                 |                 | 
| IO15      | INT3 (MotorB Forward)  |                 |                 | 
| IO13      | INT4 (MotorB Backward) |                 |                 | 
|           | MororA 1               |                 | MotorA Positive | 
|           | MororA 2               |                 | MotorA Negative | 
|           | MororB 1               |                 | MotorB Positive | 
|           | MororB 1               |                 | MotorB Negative | 
|           | Positive               | Positive (3-5V) |                 | 
|           | Negative               | Negative        |                 | 
<br />
<br />
<br />

## Servo Circuit Implements (not Recommended)
*Warning: Camera of My ESP32-CAM didn't work with Servo PWM outputting. I guess its noise causes confusing camera behaviour. So I recommend to make test the circuit with breadboard before you make the circuit fixed.*

### Electric Circuit
Connect each row.
| ESP32 CAM | Servo Base (Horizontal) | Servo Cam (Vertical) | Battery       | 
| --------- | ----------------------- | -------------------- | ------------- | 
| IO 12     | PWM Controll            |                      |               | 
|           | VCC                     |                      | Positive (5V) | 
|           | GND                     |                      | Negative      | 
| IO 16     |                         | PWM Controll         |               | 
|           |                         | VCC                  | Positive (5V) | 
|           |                         | GND                  | Negative      | 

### Source code Modifying
You can find "# Servo" in camtank.ino and app_httpd.ccp.  
Please remove the comment outs around that.  
After do it, please install source code into ESP32 again.  
<br />
<br />
<br />

## Testing
1. Turn TANK CAM on.
2. Connecting the Wifi AP "ESP32-TANK-CAM" with password "tankcam".
3. Access http://192.168.0.12/ with Webbrowser.
4. You can see Controll Buttons and Cam Streaming.  
<img src="./manubat.png" width="360px" />  
  
* Save Button -> Download Cam Screen as JPG format.  
* RED Buttons -> Controll Servo.  
* Blue Buttons - > Conrtoll Motors.  
<br />
<br />
<br />

## Hints
* Recommend to connect different battery sources ESP32 and Motors.  
Because energy drain by Motors is so much to restart ESP32 due to lack of Electricity when you connect only one bettery source.
* If you implement Servo, please give up it if Cam or Wifi was freezing when servo working.  
There can be some conflicting between cam and wifi and servo.  
Servo work(PWM) is a biggest issue.
* When you learn the system, you should use another components if you wanna implements more stable one.
