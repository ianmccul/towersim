
// Send sensor information via UDP, port 5702.
// The remote IP address to send the sensor information to is determined by the
// remote client sending us a UDP packet on port 5703.
// The remote client can change the IP address at any time by sending another packet on port 5702
// (although normally that will never happen).
// There is currently no facility to track whether the remote client is still
// receiving packets.
//
// This version uses MDNS to arvertise the service.
// We can obtain the arduino IP address using:
// $ avahi-resolve -n bell_arduino.local
//
// We can also use avahi-browse (or some other mdns browser), eg
// $ avahi-browse -crp _bellsensors._udp
// +;eth0;IPv4;_sensor1;_bellsensors._udp;local
// =;eth0;IPv4;_sensor1;_bellsensors._udp;local;bell_arduino.local;10.42.0.242;5703;


// 

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <ArduinoMDNS.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip;
IPAddress RemoteIP = IPAddress(0, 0, 0, 0);
bool RemoteIPValid = false;
EthernetUDP Udp;

EthernetUDP mdnsUdp;
MDNS mdns(mdnsUdp);

const int ListenPort = 5703;  // Port to listen for incoming UDP packets to set remote IP
const int SendPort = 5702;    // Port to send UDP packets with sensor data

enum DetectMode { Rise = 0b01, Fall = 0b10, Both = 0b11 };

struct SensorInfo {
  int Pin;
  DetectMode Mode;
  char Char;
};

// Sensor I/O pin, mode, and character to send
// On a standard Arduino, pins 0,1 are for the USB serial.
// We could use them for sensors if necessary, but then we can't do serial debugging.
// Pins 10,11,12,13 are used for ethernet.
// This leaves digital pins 2-9 available (8 pins), as well as analog pins A0 - A5 (pins 14 - 19, 6 pins).
// This gives a maximum of 14 sensors that we can connect.
// We also have our own LED pin (since pin 13 is no longer available),
// leaving 13 possible pins (12 bells plus a dumbbell!)

const int NumSensors = 6;

SensorInfo Sensors[NumSensors] = {
  { 2, Rise, '1' },
  { 3, Rise, '2' },
  { 4, Rise, '3' },
  { 5, Rise, '4' },
  { 6, Rise, '5' },
  { 7, Rise, '6' },
};

const int LED = 18;  // LED indicator pin
const int LED_DURATION = 100;  // LED on duration in ms

const unsigned long DEBOUNCE_MS = 5;  // Debounce time for sensors in ms

unsigned long time_of_signal = 0;
unsigned long time_of_last_signal[NumSensors] = { 0 };
int SensorState[NumSensors] = { 0 };

void SensorTriggered(int i) {
  // Send sensor data via UDP
  if (RemoteIPValid) {
    Udp.beginPacket(RemoteIP, SendPort);
    Udp.write(Sensors[i].Char);
    Udp.endPacket();
  } else {
    Serial.print("Sensor triggered but no remote IP: ");
    Serial.println(Sensors[i].Char);
  }
}

void setup() {
  delay(50);  // Allow the Ethernet to reset
  for (const auto& sensor : Sensors) {
    pinMode(sensor.Pin, INPUT_PULLUP);  // Set sensor pins as input
  }
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.begin(9600);
  delay(100);
  Serial.println("Initializing.");

  // Turn on the LED until we get a DHCP address
  digitalWrite(LED, HIGH);
  while (Ethernet.begin(mac) == 0) {
    Serial.println("Retrying DHCP.");
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
  }

  ip = Ethernet.localIP();
  Serial.print("Got a DHCP address: ");
  Serial.println(ip);

  Udp.begin(ListenPort);

  // Initialize the sensor state to avoid spurious signals on startup
  for (int i = 0; i < NumSensors; ++i) {
    SensorState[i] = digitalRead(Sensors[i].Pin);
  }

  mdns.begin(Ethernet.localIP(), "bell_arduino");
  mdns.addServiceRecord("_sensor1._bellsensors", ListenPort, MDNSServiceUDP);
}

void loop() {
  unsigned long now = millis();

  // Check sensors for state changes
  for (int i = 0; i < NumSensors; ++i) {
    int state = digitalRead(Sensors[i].Pin);
    if (state != SensorState[i] && now - time_of_last_signal[i] > DEBOUNCE_MS) {
      SensorState[i] = state;
      time_of_last_signal[i] = now;
      if ((state && (Sensors[i].Mode & Rise)) || (!state && (Sensors[i].Mode & Fall))) {
        time_of_signal = now;
        SensorTriggered(i);
      }
    }
  }

  // Control LED indicator
  digitalWrite(LED, (now - time_of_signal < LED_DURATION) ? HIGH : LOW);

//  for debugging
//  if (now - time_of_signal > 5000)
//  {
//    SensorTriggered(1);
//    time_of_signal = now;
//  }

  // Check for incoming UDP packets to update remote IP
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    RemoteIP = Udp.remoteIP();
    RemoteIPValid = true;
    Serial.print("Got a UDP packet from client ");
    Serial.println(RemoteIP);
  }

  // Maintain Ethernet and Mdns services
  Ethernet.maintain();
  mdns.run();
}
