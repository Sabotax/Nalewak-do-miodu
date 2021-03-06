#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef STASSID
#define WIFI_SSID "Nalewak"
#endif

const char* ssid     = WIFI_SSID;

ESP8266WebServer server(80);

/*
const String postForms = "<html>\
  <head>\
    <title>Nalewak zarzadzanie</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Ustaw wartosci:</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
      <input type=\"number\" name=\"waga\" value=\"1000\"><br>\
      <input type=\"number\" name=\"opoznienie\" value=\"30\"><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </body>\
</html>";
*/
const String odp_root = "<html>\
  <head>\
    <title>Nalewak zarzadzanie</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Ustaw wartosci:</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/set\">\
      Y_ref: <input type=\"number\" name=\"waga\" placeholder=\"1000\"><br>\
      Opoznienie: <input type=\"number\" name=\"opoznienie\" placeholder=\"30\"><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    <hr>\
    <h1>Ustaw wspolczynnik kalibracji:</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/calib\">\
      Wspolczynnik kalibracji: <input type=\"number\" name=\"factor\"><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    <hr>\
    <h1>Pokaz wspolczynnik kalibracji</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/show\">\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </body>\
</html>";
const String odp1 = "<html>\
  <head>\
    <title>Nalewak zarzadzanie</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <a href=\"/\" >Informacje do zmiany parametrow y_ref i opoznienie wyslano - nacisnij by wrocic na glowna strone</a>\
  </body>\
</html>";
const String odp2 = "<html>\
  <head>\
    <title>Nalewak zarzadzanie</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <a href=\"/\" >Informacje do zmiany parametru wspolczynnika kalibracji wyslano - nacisnij by wrocic na glowna strone</a>\
  </body>\
</html>";
const String odp3 = "<html>\
  <head>\
    <title>Nalewak zarzadzanie</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <a href=\"/\" >Informacje do pokazania wsp kalibracji wyslano - nacisnij by wrocic na glowna strone</a>\
  </body>\
</html>";
void handleRoot() {
  if( server.method() == 1 ) {
    server.send(200, "text/html", odp_root);
  }
  else {
    server.send(200, "text/html", "oops"+server.method() );
  }
  
  // TODO czy da sie glowny form zrobic zeby odsylal tu i go odpowiednio ogarnac
}
void handleSet() {
  server.send(200, "text/html", odp1);
  Serial.print("y"+server.arg(0) + ";");
  delay(50);
  Serial.print("o"+server.arg(1) + ";");
}
void handleCalib() {
  server.send(200, "text/html", odp2);
  Serial.print("s"+server.arg(0) + ";");
}
void handleShow() {
  server.send(200, "text/html", odp3);
  Serial.print("c;");
}
/*
void handlePlain() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
    digitalWrite(led, 0);
  }
}
*/
/*
void handleForm() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
//    for (uint8_t i = 0; i < server.args(); i++) {
//      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//    }
    Serial.println(server.arg(0) + "\t" + server.arg(1));
    String message = "Udalo sie, cofnij by wypelnic formularz ponownie";
    server.send(200, "text/plain", message+server.method());
    digitalWrite(led, 0);

    // 1 to brak metody
    // 3 to POST
  }
}
*/
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  Serial.begin(38400);
  Serial.println("Zaczynam");
  /* example
   *    Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("ESPsoftAP_01") ? "Ready" : "Failed!");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
   */
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("AP stworzone");
  server.on("/", handleRoot);
  server.on("/show",HTTP_POST, handleShow);
  server.on("/calib",HTTP_POST, handleCalib);
  server.on("/set",HTTP_POST, handleSet);
  
  //server.on("/postform/", handleForm);
  // mozna tez chyba uzyc .on z 3 argumentami, gdzie srodkowy to method, moze to lepiej zamiast if else if w root
  // https://links2004.github.io/Arduino/d3/d58/class_e_s_p8266_web_server.html

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
