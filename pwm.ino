#include <ESP8266WebServer.h>

#define LEDn    12
#define LEDpwm   2

//Define name of the Wifi & password for creating an access point
const char* ssid = "ledTest";;
//!!!Your password MUST be a minimum of 8 characters...otherwise neither password nor ssid will be accepted -> default or old ssid&pwd will show up!!! 
const char* password = "12341234";


ESP8266WebServer server(80);

char webpage[] = R"=(
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>

<h2>NodeMCU Server Control</h2>

<div class="slidecontainer">
  <p>GPIO 5 PWM Value: <span id="slider_p"></span></p>
  <input type="range" min="0" max="1023" value="0" class="slider" id="myRange">

<p>GPIO 15 LED Switch: <span id="led_state">OFF</span></p> 
<label class="switch">
  <input type="checkbox" onchange="state_change(this)">
  <span class="slider1 round"></span>
</label>
</div>

<script src="https://canvasjs.com/assets/script/canvasjs.min.js"></script>


<script>
var slider = document.getElementById("myRange");
var output = document.getElementById("slider_p");
output.innerHTML = slider.value;

slider.onchange = function() {
  output.innerHTML = this.value;
  pwm_change(this.value);
}
</script>

<script>
function pwm_change(val) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "setPWM?PWMval="+val, true);
  xhttp.send();
}
</script>

<script>
function state_change(element) {
  var xhttp = new XMLHttpRequest();
  
  if (element.checked){
    xhttp.open("GET", "setButton?button_state=1", true);
    document.getElementById("led_state").innerHTML = "ON";
  } else if (!element.checked){
    xhttp.open("GET", "setButton?button_state=0", true);
    document.getElementById("led_state").innerHTML = "OFF";
  }
  xhttp.send();
}
</script>

</body>

</html>
)=";

bool led;
int pwm;
int pwmCount;

void handle_button() {
 String button_state = server.arg("button_state"); // on page ./setButton we are reading the variable 'button_state'
 
 String curr_state = "OFF"; // for sending back to the page
 if(button_state == "1")
 {
  digitalWrite(button_pin,HIGH); //LED ON
  curr_state = "ON";
 }
 else
 {
  digitalWrite(button_pin,LOW); //LED OFF
  curr_state = "OFF";  
 }
 
 server.send(200, "text/plane", curr_state); //Send state back to page
}
void handle_pwm() {
 String pwm_val = server.arg("PWMval"); // reading from slider on html pagae
 Serial.print("slider val: ");
 Serial.println(pwm_val);
 analogWrite(PWM_pin,pwm_val.toInt()); // chaning the value on the NodeMCU board
 server.send(200,"text/plane","0"); // handling the webpage update
}
//----------------------------------------------------------------------------------
void handleRoot(){
  server.send(200, "text/html", webpage);  
}

void setup() {
  Serial.begin(9600);
  //Set LED pins to output________________
  pinMode(LEDn,OUTPUT);
  pinMode(LEDpwm,OUTPUT);
  //Wifi as access point__________________
  WiFi.mode(WIFI_AP);
  IPAddress apLocalIp(2,2,2,2);
  IPAddress apSubnetMask(255,255,255,0);
  WiFi.softAPConfig(apLocalIp,apLocalIp,apSubnetMask);
  WiFi.softAP(ssid, password);
  //Server________________________________
  server.begin();
  server.on("/", handleRoot);
  server.on("/setPWM", handle_pwm); // handles the PWM values
  server.on("/setButton", handle_button); // handles button values   
  pwm=0;
  pwmCount=0;
  led=0;
}

void loop() {
  server.handleClient();
}