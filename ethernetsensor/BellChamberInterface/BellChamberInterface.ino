// Send sensor information via UDP, port 5702.
// The remote IP address to send the sensor information to is determined by the
// remote client sending us a UDP packet on port 5703.
// The remote client can change the IP address at any time by sending another packet on port 5702
// (although normally that will never happen).
// There is currently no facility to track whether the remote client is still
// receiving packets.

#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip;
IPAddress RemoteIP = IPAddress(0, 0, 0, 0);
bool RemoteIPValid = false;
EthernetUDP Udp;

int const ListenPort = 5703;
int const SendPort = 5702;

enum DetectMode { Rise       = 0b01,
                  Fall       = 0b10,
                  Both       = 0b11 };

struct SensorInfo {
  int Pin;
  DetectMode Mode;
  char Char;
};

int const NumSensors = 12;

// Sensor I/O pin, mode, and character to send
// Pins 0,1 are for the USB serial.  We could use them for sensors if necessary, but then we can't do serial debugging.
// Pins 10,11,12,13 are used for ethernet.
// This leaves digital pins 2-9 available (8 pins), as well as analog pins A0 - A5 (pins 14 - 19, 6 pins).
// This gives a maximum of 14 sensors that we can connect.
// We also have our own LED pin (since pin 13 is no longer available),
// leaving 13 possible pins (12 bells plus a dumbbell!)
SensorInfo Sensors[NumSensors] = {
  { 2, Rise, '1' },
  { 3, Rise, '2' },
  { 4, Rise, '3' },
  { 5, Rise, '4' },
  { 6, Rise, '5' },
  { 7, Rise, '6' },
  { 8, Rise, '7' },
  { 9, Rise, '8' },
  { 14, Rise, '9' },
  { 15, Rise, '0' },
  { 16, Rise, 'E' },
  { 17, Rise, 'T' }
};

// Flashing LED indicator when we get a sensor signal
int const LED = 18;
int const LED_DURATION = 100;

// Reed switches bounce.
unsigned long const DEBOUNCE_MS = 2;

unsigned long time_of_signal = 0;
unsigned long time_of_last_signal[NumSensors] = {
  0,
};
int SensorState[NumSensors] = {
  0,
};

void SensorTriggered(int i) {
  // the function to send the packet
  if (RemoteIPValid) {
    Udp.beginPacket(RemoteIP, SendPort);
    Udp.write(Sensors[i].Char);
    Udp.endPacket();
  } else {
    Serial.print("Sensor output but no remote IP: ");
    Serial.println(i);
  }
}

void setup() {
  delay(50);  // allow the ethernet to reset
  for (auto const& x : Sensors) {
    pinMode(x.Pin, INPUT);
  }
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.begin(9600);

  Serial.println("Initalizing.");

  // Turn on the LED until we get a DHCP address
  digitalWrite(LED, HIGH);
  while (Ethernet.begin(mac) == 0) {
    Serial.println("Retry DHCP.");
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
  }

  ip = Ethernet.localIP();
  Serial.print("Got a DHCP address: ");
  Serial.println(ip);

  Udp.begin(ListenPort);
}

void loop() {
  unsigned long now = millis();

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
  digitalWrite(LED, (millis() - time_of_signal < LED_DURATION) ? HIGH : LOW);

  int PacketSize = Udp.parsePacket();
  if (PacketSize) {
    RemoteIP = Udp.remoteIP();
    RemoteIPValid = true;
    Serial.print("Got a UDP packet from client ");
    Serial.println(RemoteIP);
  }

  Ethernet.maintain();
}
