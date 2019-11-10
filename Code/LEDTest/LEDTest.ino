//#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define D0 16;
#define D1 5;
#define D2 4;
#define D3 0;
#define D4 2;
#define D5 14;
#define D6 12;
#define D7 13;
#define D8 15;
#define D9 3;
#define D10 1;

const int _LedPin1 = D3;

bool _LedState1 = LOW;

const int _SwitchPin1 = D2;

bool _LastSwitchState1 = false;
uint16_t _SwitchPressed1 = 0;

const char* ssid = "Wince";
const char* password = "manbearpig";
const char* hostName = "LEDTest";

ESP8266WebServer server(80);

void handleRoot() {
	String cmd;     
	cmd += "<!DOCTYPE HTML>\r\n";
	cmd += "<html>\r\n";
	//cmd += "<header><title>ESP8266 Webserver</title><h1>\"ESP8266 Web Server Control\"</h1></header>";
	cmd += "<head>";
	cmd += "<meta http-equiv='refresh' content='5'/>";
	cmd += "</head>";

	cmd += "<body>" + String(hostName) + "</body>";
         
	cmd += "</html>\r\n";
	server.send(200, "text/html", cmd);
}

void handleNotFound(){
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET)?"GET":"POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i=0; i<server.args(); i++){
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}

void setup(void)
{
	pinMode(_LedPin1, OUTPUT);
	pinMode(_SwitchPin1, INPUT_PULLUP);

	Serial.begin(9600);
	WiFi.begin(ssid, password);
	Serial.println("Starting...");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.print("Connected to ");
	WiFi.hostname(hostName);
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	if (MDNS.begin(hostName)) {
		Serial.println("MDNS responder started");
	}

	server.on("/", handleRoot);

	server.on("/power", [](){
		String _switch = server.arg("switch");

		if (_switch == "1")
		{
			_LedState1 = !_LedState1;
			digitalWrite(_LedPin1, _LedState1);
			server.send(404, "text/plain", String("The switch is ") + _LedState1);

			Serial.println("Pressed!");
		}
		else
			server.send(404, "text/plain", String("Could not find switch."));
	});

	server.on("/state", []() {
		String _switch = server.arg("switch");

		if (_switch == "1")
			server.send(200, "text/plain", String(_LedState1));
		else
			server.send(404, "text/plain", String("Could not find switch."));
	});

	server.onNotFound(handleNotFound);
	server.begin();
	Serial.println("HTTP server started");
}

void loop(void){
	server.handleClient();

	if (
		millis() > _SwitchPressed1 + 500 // Current time is greater than the last time it was pressed + 500 milliseconds
		&& digitalRead(_SwitchPin1) // The switch is currently pressed
		&& !_LastSwitchState1 // Last we checked, the switch hasn't been pressed
	)
	{
		_LedState1 = !_LedState1;
		digitalWrite(_LedPin1, _LedState1);
		_SwitchPressed1 = millis();

		Serial.println("Pressed!");
	}

	// Store the current switch state for next frame
	_LastSwitchState1 = digitalRead(_SwitchPin1);
}
