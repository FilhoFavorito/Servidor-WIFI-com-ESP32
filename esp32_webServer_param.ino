#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "A15 de João Pedro";
const char* password = "opala.10";

WebServer server(80);
const int led = 2;
bool ledState = true;

// Controle do Led
void ligarLed(){
  ledState = false;
  digitalWrite(led, ledState);
  Serial.println("Ligar led");
}

void desligarLed(){
  ledState = true;
  digitalWrite(led, ledState);
  Serial.println("Desligar led");  
}

bool isLedOn(){
  return !ledState;
}


void handleRoot() {
  String msg;

  if (server.hasArg("led")) {
    if (server.arg("led") == "on" ) {
      desligarLed();
    } else if (server.arg("led") == "off" ) {
      ligarLed();
    }
  }
 
  String content = "<html>";
  content += "<body>";
  content += "<form action='' method='post'>";
  content += " <fieldset>";
  content += " <legend>LED</legend>";
  
  if (isLedOn()){
    content += " <input type='radio' name='led' value='on' checked='checked'> ON";
    content += "<input type='radio' name='led' value='off' > OFF<br>";
  }else{
    content += " <input type='radio' name='led' value='on'> ON";
    content += "<input type='radio' name='led' value='off' checked='checked'> OFF<br>";
  }
  content += " </fieldset>";
  content += "<input type='submit' value='Submit'>";
  content += "</form>";
  content += "</body>";
  content += "</html>";


  server.send(200, "text/html", content);
}

void handleNotFound() {
  String message = "Not Found\n\n";
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
  pinMode(led, OUTPUT);
  desligarLed();

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(10);
}
