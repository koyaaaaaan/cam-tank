#include "esp_camera.h"
#include <WiFi.h>

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
//#define CAMERA_MODEL_M5STACK_ESP32CAM
#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_TTGO_T_JOURNAL

#include "camera_pins.h"

// ***** Wifi Mode *****
// true:  ESP32 becomes wifi access point + httpd. 
//        You can connect with wifi "ESP32-TANK-CAM" to access http://192.168.0.12/ (default pass is tankcam).
//
// false: ESP32 becomes httpd with your wifi router. 
//        IP address of ESP32 will be shown on console when it waking up to access it http://xxxxxxxxxx/ .
const bool WifiMode = true; 

// ***** Configure for WifiMode = true *****
const char s_ssid[] = "ESP32-TANK-CAM";
const char s_pass[] = "tankcam";
const IPAddress s_ip(192,168,0,12);
const IPAddress s_subnet(255,255,255,0);

// ***** Configure for WifiMode = false *****
const char* c_ssid = "please put your wifi SSID here";
const char* c_pass = "please put your wifi PASS here";

// PIN SETTING
const int PIN_MOTOR_A_FW = 2;
const int PIN_MOTOR_A_BW = 14;
const int PIN_MOTOR_B_FW = 15;
const int PIN_MOTOR_B_BW = 13;
const int PIN_LED = 4;

// # If you wanna implement Camera XY Control System with Servo
// # Please remove comment out at "# Servo" (4 in camtank.ino, 1 in app_httpd.cpp).
// # But please note that PWM output noise can cause camera shut down.
// const int SERVO_BASE = 12; // # Servo
// const int SERVO_CAM = 16;  // # Servo

// void setServo(int sv1, int sv2); // # Servo
void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  Serial.println("Set Up Cam...");
  cameraSetUp();
  
  Serial.println("Set Up User Pins...");
  pinSetting();  

  Serial.println("Starting HTTP Server...");
  if(WifiMode){
    startSoftWareAP();
  }else{
    startWifiConnection();
  }

  setUpComplete();
  Serial.println("Set Up Completed.");
}



void cameraSetUp(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }else{
    s->set_brightness(s, 0.6);
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);
}

void startSoftWareAP(){
  WiFi.softAP(s_ssid,s_pass);
  delay(100);
  WiFi.softAPConfig(s_ip,s_ip,s_subnet);
  IPAddress myIP = WiFi.softAPIP();
  
  Serial.println("ESP32 SoftAP Mode start.");
  Serial.print("SSID:");
  Serial.println(s_ssid);

  startCameraServer();
  
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(myIP);
  Serial.println("' to connect");
}

void startWifiConnection(){
  Serial.print("ESP32 Wifi Connection Mode start. (SSID)");
  Serial.println(c_ssid);
  WiFi.begin(c_ssid, c_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  startCameraServer();
  
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void pinSetting(){
  // Please put your gpio assign here
  pinMode(PIN_MOTOR_A_FW, OUTPUT);
  pinMode(PIN_MOTOR_A_BW, OUTPUT);
  pinMode(PIN_MOTOR_B_FW, OUTPUT);
  pinMode(PIN_MOTOR_B_BW, OUTPUT);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_MOTOR_A_FW ,LOW);
  digitalWrite(PIN_MOTOR_A_BW ,LOW);
  digitalWrite(PIN_MOTOR_B_FW ,LOW);
  digitalWrite(PIN_MOTOR_B_BW ,LOW);
  
  // setServo(SERVO_BASE, SERVO_CAM); // # Servo
}

void setUpComplete(){
  // When Set up is completed, built in LED will blink 2 times here.
  digitalWrite(PIN_LED, HIGH);
  delay(50);
  digitalWrite(PIN_LED, LOW);
  delay(100);
  
  digitalWrite(PIN_LED, HIGH);
  delay(50);
  digitalWrite(PIN_LED, LOW);
  delay(100);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000000);
}
