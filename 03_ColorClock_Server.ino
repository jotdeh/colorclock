#include <FastLED.h>

void setup_server(){
  // *** SERVER CODE

  if (MDNS.begin("colorclock")) {              // Start the mDNS responder for colorclock.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/config", HTTP_POST, handleConfig); // Call the 'handleConfig' function when a POST request is made to URI "/config"
  server.on("/randomcolors", HTTP_POST, handleRandomColors);  // Call the 'handleRandomColors' function when a POST request is made to URI "/randomcolors"
  server.on("/demo", HTTP_POST, handleDemo); // Call the 'handleConfig' function when a POST request is made to URI "/login"
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");
}  
  
void loop_server(){
  server.handleClient();                     // Listen for HTTP requests from clients
  MDNS.update();                             // This was not in the code, found https://github.com/esp8266/Arduino/issues/4790
}
  
String configForm(){
  String msg;
  char buffer [16];
  msg = "<form action=\"/config\" method=\"POST\"> <label for=\"backgroundcolor\">Background</label> <input type=\"color\" id=\"backgroundcolor\" name=\"backgroundcolor\" value=\"";
  msg += longToHtml(settings.backgroundcolor) + "\"></br>";
  msg += "<label for=\"hourcolor\">Hours</label>  <input type=\"color\" id=\"hourcolor\" name=\"hourcolor\" value=\"";
  msg += longToHtml(settings.hourcolor) + "\"></br>";
  msg += "<label for=\"minutecolor\">Minutes</label>  <input type=\"color\" id=\"minutecolor\" name=\"minutecolor\" value=\"";
  msg += longToHtml(settings.minutecolor) + "\"></br>";  
  msg += "<label for=\"secondcolor\">Seconds</label>  <input type=\"color\" id=\"secondcolor\" name=\"secondcolor\" value=\"";
  msg += longToHtml(settings.secondcolor) + "\"></br> </br>";
  msg += "<label for=\"displayseconds\">Display seconds</label> <input type=\"checkbox\" id=\"displayseconds\" name=\"displayseconds\" value=\"displayseconds\" ";
  msg += String(settings.displayseconds ? "checked": "") + "></br>  </br>";
  msg += "<label for=\"normalbrightness\">Normal brightness level</label>    <input type=\"range\" id=\"normalbrightness\" name=\"normalbrightness\" min=\"0\" max=\"255\" value=\"";
  msg += String(settings.normalbrightness,DEC) + "\"></br> </br>";
  msg += String("<label for=\"dimon\">Dim on</label> <input type=\"radio\" id=\"dimon\" name=\"dim\" value=\"") + (DIMON) + "\" ";
  msg += String(settings.dim == DIMON ? " checked": "") +">";
  msg += String("<label for=\"dimoff\">Dim off</label> <input type=\"radio\" id=\"dimoff\" name=\"dim\" value=\"") + (DIMOFF) + "\" ";
  msg += String(settings.dim == DIMOFF ? " checked": "") + ">";
  msg += String("<label for=\"dimscheduled\">Dim scheduled</label> <input type=\"radio\" id=\"dimscheduled\" name=\"dim\" value=\"") + (DIMSCHEDULED) + "\" ";
  msg += String(settings.dim == DIMSCHEDULED ? " checked": "") + ">  </br>";
  msg += "<label for=\"dimmedbrightness\">Dimmed brightness level</label>    <input type=\"range\" id=\"dimmedbrightness\" name=\"dimmedbrightness\" min=\"0\" max=\"255\" value=\"";
  msg += String(settings.dimmedbrightness,DEC) + "\"></br>";
  msg += "<label for=\"dimstart\">Start to dim brightness</label> <input type=\"time\" id=\"dimstart\" name=\"dimstart\" value=\"";
  sprintf (buffer, "%02u:%02u", settings.dimstarthour, settings.dimstartminute);
  msg += String(buffer) + "\"><br> </br>";
  msg += "<label for=\"dimstop\">Stop to dim brightness</label> <input type=\"time\" id=\"dimstop\" name=\"dimstop\" value=\"";
  sprintf (buffer, "%02u:%02u", settings.dimstophour, settings.dimstopminute);
  msg += String(buffer) + "\"><br> </br>";

  msg += "<input type=\"submit\" value=\"Apply\"></br></br>    <input type=\"reset\" value=\"Reset\"></br></form>";

  msg += "<form action=\"/randomcolors\" method=\"POST\">";
  msg += "<input type=\"submit\" value=\"Random Colors\"></br></form>";

  msg += "<form action=\"/demo\" method=\"POST\">";
  msg += "<input type=\"submit\" value=\"" + String(demomode ? "Stop Demo": "Start Demo") + "\"></br></form>";
  return msg;
}

void handleRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", configForm());
}

long htmlToLong(String incoming){
  String colorString = incoming.substring(1); // remove Q:
  long color = strtol(colorString.c_str(), NULL, 16);
  return color;
}

String byteToString(byte incoming){
  char buffer[3];
  itoa(incoming, buffer, 8);
  return String(buffer);
}

String longToHtml(long incoming){
  char buffer[15];
  
  //ltoa(incoming, buffer, 16);
  sprintf(buffer,"#%06X",incoming);
  return buffer;
}

void handleDemo() {
  demomode = !demomode;
  server.send(200, "text/html", "all ok </br>" + configForm());
}

void handleRandomColors() {
  CHSV background_hsv; CRGB background_rgb;
  CHSV hour_hsv; CRGB hour_rgb;
  CHSV minute_hsv; CRGB minute_rgb;
  
  background_hsv = CHSV( random8(), random8(120, 255), random8(120,255));
  hour_hsv = CHSV( background_hsv.hue + random8(40,120), random8(120, 255), random8(180,255));
  minute_hsv = CHSV( hour_hsv.hue + random8(40,120), random8(120, 255), random8(180,255));
  
  hsv2rgb_rainbow(background_hsv, background_rgb);
  hsv2rgb_rainbow(hour_hsv, hour_rgb);
  hsv2rgb_rainbow(minute_hsv, minute_rgb);

  settings.backgroundcolor = ((long)background_rgb.r << 16L) | ((long)background_rgb.g << 8L) | (long)background_rgb.b;
  settings.hourcolor = ((long)hour_rgb.r << 16L) | ((long)hour_rgb.g << 8L) | (long)hour_rgb.b;
  settings.minutecolor = ((long)minute_rgb.r << 16L) | ((long)minute_rgb.g << 8L) | (long)minute_rgb.b;
  
  server.send(200, "text/html", "all ok </br>" + configForm());
  put_settings_to_eeprom();
}

void handleConfig() {                         // If a POST request is made to URI /login
  
  if(server.arg("backgroundcolor") != NULL ){
    settings.backgroundcolor = htmlToLong(server.arg("backgroundcolor"));
    Serial.println(settings.backgroundcolor);
    Serial.println("1");
    
  }
  if(server.arg("hourcolor") != NULL ){
    settings.hourcolor = htmlToLong(server.arg("hourcolor"));
    Serial.println("2");
  }
  if(server.arg("minutecolor") != NULL ){
    settings.minutecolor = htmlToLong(server.arg("minutecolor"));
    Serial.println("3");
  }
  if(server.arg("secondcolor") != NULL ){
    settings.secondcolor = htmlToLong(server.arg("secondcolor"));
    Serial.println("4");
  }
  if(server.arg("displayseconds") != NULL ){
    settings.displayseconds = true;
    Serial.println(settings.displayseconds);
    Serial.println("5");
  }
  else {
    settings.displayseconds = false;
    Serial.println(settings.displayseconds);
    Serial.println("5");
  }
  
  if(server.arg("normalbrightness") != NULL ){
    settings.normalbrightness = String(server.arg("normalbrightness")).toInt();
    Serial.println("6");
  }
  if(server.arg("dim") != NULL ){
    settings.dim = String(server.arg("dim")).charAt(0);
    Serial.println("7");
  }
  if(server.arg("dimmedbrightness") != NULL ){
    settings.dimmedbrightness = String(server.arg("dimmedbrightness")).toInt();
    Serial.println("8");
  }
  if(server.arg("dimstart") != NULL ){
    settings.dimstarthour = String(server.arg("dimstart")).substring(0,2).toInt();
    settings.dimstartminute = String(server.arg("dimstart")).substring(3,5).toInt();
    Serial.println(settings.dimstarthour);
    Serial.println(settings.dimstartminute);
    Serial.println("9");
  }

  if(server.arg("dimmedbrightness") != NULL ){
    settings.dimmedbrightness = String(server.arg("dimmedbrightness")).toInt();
    Serial.println("10");
  }
  
  if(server.arg("dimstop") != NULL ){
    settings.dimstophour = String(server.arg("dimstop")).substring(0,2).toInt();
    settings.dimstopminute = String(server.arg("dimstop")).substring(3,5).toInt();
    Serial.println("11");
  }

  if(server.arg("demomode") != NULL ){
    demomode = true;
    Serial.println(demomode);
    Serial.println("12");
  }
  else {
    demomode = false;
    Serial.println(demomode);
    Serial.println("12");
  }

  server.send(200, "text/html", "all ok </br>" + configForm());
  put_settings_to_eeprom();
  Serial.println("put");
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
