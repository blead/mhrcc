#include <user_interface.h>

#define DISABLE 0
#define ENABLE  1
#define MAX_DATA_LENGTH 112
#define TYPE_DATA 0x08
#define TYPE_DATA_NULL 0x48

int state = 0;
int emerSignal = 0;

struct RxControl {
  signed rssi:8; // signal intensity of packet
  unsigned rate:4;
  unsigned is_group:1;
  unsigned:1;
  unsigned sig_mode:2; // 0:is 11n packet; 1:is not 11n packet;
  unsigned legacy_length:12; // if not 11n packet, shows length of packet.
  unsigned damatch0:1;
  unsigned damatch1:1;
  unsigned bssidmatch0:1;
  unsigned bssidmatch1:1;
  unsigned MCS:7; // if is 11n packet, shows the modulation and code used (range from 0 to 76)
  unsigned CWB:1; // if is 11n packet, shows if is HT40 packet or not
  unsigned HT_length:16;// if is 11n packet, shows length of packet.
  unsigned Smoothing:1;
  unsigned Not_Sounding:1;
  unsigned:1;
  unsigned Aggregation:1;
  unsigned STBC:2;
  unsigned FEC_CODING:1; // if is 11n packet, shows if is LDPC packet or not.
  unsigned SGI:1;
  unsigned rxend_state:8;
  unsigned ampdu_cnt:8;
  unsigned channel:4; //which channel this packet in.
  unsigned:12;
};

struct SnifferPacket {
  struct RxControl rx_ctrl;
  uint8_t data[MAX_DATA_LENGTH];
};

struct DataPacket {
  uint8_t *frame_ctrl; // 2
  uint8_t *duration; // 2
  uint8_t *destination; // 6
  uint8_t *sender; // 6
  uint8_t *bssid; // 6
  uint8_t *seq; // 2
  uint8_t *data; // 4
};

// MAC Addresses
uint8_t self[6] = {0x5A,0xDB,0x5A,0xDB,0x5A,0xDB};
uint8_t ap[6] = {0xFC,0x5B,0x39,0xE0,0xFC,0x9F};

uint8_t destination[6] = {0xF0,0x18,0x98,0x1C,0x47,0x72};
uint8_t real_peaw[6] = {0xF0,0x18,0x98,0x1C,0x47,0x72};
uint8_t peaw[6] = {0xa0,0x88,0xb4,0xb7,0x63,0xd8};

uint8_t broadcast[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

uint8_t packet_buffer[64];

static void printDataSpan(uint16_t start, uint16_t size, uint8_t* data) {
  for(uint16_t i = start; i < MAX_DATA_LENGTH && i < start+size; i++) {
    Serial.printf("%02x", data[i]);
  }
  Serial.println();
}

/* Creates a packet.
 * 
 * buf - reference to the data array to write packet to;
 * destination - destination MAC address;
 * sender - sender MAC address;
 * bssid - BSSID;
 * seq - sequence number of 802.11 packet;
 * data - data being sent, repeated 4 times for no reason;
 * 
 * Returns: size of the packet
 */
uint16_t create_packet(uint8_t *buf, uint8_t *destination, uint8_t *sender, uint8_t *bssid, uint16_t seq, uint8_t data)
{
  int i=0;

  // Frame type
  buf[0] = TYPE_DATA;
  // Control flags
  buf[1] = 0x00;
  // Duration
  buf[2] = 0x00;
  buf[3] = 0x00;
  // Destination
  for (i=0; i<6; i++) buf[i+4] = destination[i];
  // Sender
  for (i=0; i<6; i++) buf[i+10] = sender[i];
  // BSSID    
  for (i=0; i<6; i++) buf[i+16] = bssid[i];
  // Sequence
  buf[22] = seq % 0xFF;
  buf[23] = seq / 0xFF;
  // Body
  buf[24] = data;
  buf[25] = data;
  buf[26] = data;
  buf[27] = data;

  return 28;
}

static void showMetadata(struct SnifferPacket *snifferPacket) {
  struct DataPacket *dataPacket;

  Serial.print("RSSI: ");
  Serial.println(snifferPacket->rx_ctrl.rssi, DEC);

  dataPacket->frame_ctrl = snifferPacket->data;
  dataPacket->duration = snifferPacket->data+2;
  dataPacket->destination = snifferPacket->data+4;
  dataPacket->sender = snifferPacket->data+10;
  dataPacket->bssid = snifferPacket->data+16;
  dataPacket->seq = snifferPacket->data+22;
  dataPacket->data = snifferPacket->data+24;
  
  Serial.print("full frame: ");
  printDataSpan(0, 20, snifferPacket->data);
  Serial.print("frame_ctrl: ");
  printDataSpan(0, 2, dataPacket->frame_ctrl);
  Serial.print("duration: ");
  printDataSpan(0, 2, dataPacket->duration);
  Serial.print("destination: ");
  printDataSpan(0, 6, dataPacket->destination);
  Serial.print("sender: ");
  printDataSpan(0, 6, dataPacket->sender);
  Serial.print("bssid: ");
  printDataSpan(0, 6, dataPacket->bssid);
  Serial.print("seq: ");
  printDataSpan(0, 2, dataPacket->seq);
  Serial.print("data:");
  printDataSpan(0, 4, dataPacket->data);
}

/**
 * Callback for promiscuous mode
 */
static void ICACHE_FLASH_ATTR sniffer_callback(uint8_t *buffer, uint16_t length) {
  struct SnifferPacket *snifferPacket = (struct SnifferPacket*) buffer;

  bool eq = true;
  for(int i=0; i<6; i++) {
    eq &= (snifferPacket->data[16+i] == ap[i]);
  }
  
  if (length == 60 && eq) {
//    Serial.printf("raw length: %d\n", length);
//    Serial.print("raw data: ");
//    printDataSpan(0, length, buffer);
    showMetadata(snifferPacket);
    Serial.println("driving with");
    Serial.printf("%c", snifferPacket->data[24]);
    drive(snifferPacket->data[24]);
  }
}

void setup() {
  // set the WiFi chip to "promiscuous" mode aka monitor mode
  pinMode(D1,OUTPUT); //LEFT 
  pinMode(D2,OUTPUT); //LEFT 
  //+-==front -+==back
  pinMode(D3,OUTPUT); //RIGHT
  pinMode(D4,OUTPUT); //RIGHT
  //+-==front -+==back
  pinMode(D5,OUTPUT); //led
  pinMode(D6,OUTPUT); //led
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(10);
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(6);
  wifi_promiscuous_enable(DISABLE);
  delay(10);
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  delay(10);
  wifi_promiscuous_enable(ENABLE);
}

void loop() {
  delay(100);
  resetOutput();
  if(emerSignal == 1)emer();
  uint8_t data = 0x42;
  uint16_t size = create_packet(packet_buffer, broadcast, self, ap, 0x10, data);
  wifi_send_pkt_freedom(packet_buffer, size, 0);
}

void drive(char c) {
//  resetOutput();
    if (c == 'w') {
      //do stuff
      Serial.println("GO"); 
      forward();     
     }  
    else if(c == 'a'){     
      Serial.println("LEFT"); 
      left(); 
    }
    else if(c == 'd'){     
      Serial.println("RIGHT"); 
      right(); 
    }
    else if(c == 's'){     
      Serial.println("BACK");  
      backward();
    }
    else if(c == 'e'){     
      Serial.println("BACK");  
      if(emerSignal == 0) emerSignal = 1;
      else emerSignal = 0;
    }
//  if(emerSignal == 1)emer();
}


void forward(){
  analogWrite(D1,768);
  analogWrite(D2,1023);
  analogWrite(D3,768);
  analogWrite(D4,1023);
  digitalWrite(LED_BUILTIN,LOW);
  digitalWrite(D5,HIGH);
  Serial.print("WTF");
}
void left(){
  analogWrite(D1,1023);
  analogWrite(D2,768);
  analogWrite(D3,768);
  analogWrite(D4,1023);
  digitalWrite(LED_BUILTIN,LOW);
}
void right(){
  analogWrite(D1,768);
  analogWrite(D2,1023);
  analogWrite(D3,1023);
  analogWrite(D4,768);
  digitalWrite(LED_BUILTIN,LOW);
}
void backward(){
  analogWrite(D1,1023);
  analogWrite(D2,768);
  analogWrite(D3,1023);
  analogWrite(D4,762);
  digitalWrite(LED_BUILTIN,LOW);
}
void emer(){
  if(state==0){
    digitalWrite(D5,HIGH);
    digitalWrite(D6,LOW);
    state=1;
  }
  else{
    digitalWrite(D5,LOW);
    digitalWrite(D6,HIGH);
    state=0;
  }  
}
void resetOutput(){
  analogWrite(D1,1023);
  analogWrite(D2,1023);
  analogWrite(D3,1023);
  analogWrite(D4,1023);
  digitalWrite(LED_BUILTIN,HIGH);
  digitalWrite(D5,LOW);
  digitalWrite(D6,LOW);
}
