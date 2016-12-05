#include <ESP8266WiFi.h>
#include <Time.h>  

const int buttonPin = 2; // the number of the pushbutton pin
const int ledPin = 0; // the number of the LED pin

int buttonState = 0; // variable for reading the pushbutton status

// your wifi details
const char * ssid = "{FUSE WIFI ID}}";
const char * password = "{{FUSE WIFI PASSWORD}}";

// slack webhook host and endpoint information
const char * host = "hooks.slack.com";
const char * SlackWebhookURL = "{{REPLACE WITH URL}}";

// SSL Certificate finngerprint for the host
const char * fingerprint = "‎‎ab f0 5b a9 1a e0 ae 5f ce 32 2e 7c 66 67 49 ec dd 6d 6a 38";

void setup() {
  Serial.begin(115200);

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);

  delay(100);
  // We start by connecting to a WiFi network

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void sendToSlack() {

  // String channel = "#coffee";
  // String username = "fuse-coffee-bot";
  String message = " <!channel> :coffee: Coffee is ready :raised_hands:";

  Serial.print("connecting to ");
  Serial.println(host);

  // create a secure connection using WiFiClientSecure
  WiFiClientSecure client;
  const int httpPort = 443;
  if (client.connect(host, httpPort)) {

    // verify the signature of the ssl certificate
    if (client.verify(fingerprint, host)) {
      Serial.println("ssl cert matches");
    } else {
      Serial.println("ssl cert mismatch");
    }

    String PostData = "payload={\"text\": \"" + message + "\"}";
    Serial.println(PostData);

    client.print("POST ");
    client.print(SlackWebhookURL);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(host);
    client.println("User-Agent: ArduinoIoT/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);

    delay(500);

    // Read all the lines of the reply from server and print them to Serial for debugging
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");
  }

}

bool buttonPressed = false;
void loop() {

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:    
    digitalWrite(ledPin, HIGH);
    Serial.println("=====CIRCUIT_OPEN=====");
    buttonPressed = false;
  } else if (buttonPressed == false) {
    // turn LED off:
    digitalWrite(ledPin, LOW);
    Serial.println("=====CIRCUIT_COMPLETE=====");
    buttonPressed = true;
    sendToSlack();
  }
}
