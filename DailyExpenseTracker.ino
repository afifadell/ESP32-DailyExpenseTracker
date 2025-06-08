#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int count, save, lastUP, lastDOWN, lastreset;
int steps = 1;
void OLEDprint(String line1, String line2 = "");

void setup() {
  pinMode(17, INPUT_PULLUP);pinMode(16, INPUT_PULLUP);pinMode(18, INPUT_PULLUP);
  Serial.begin(115200);
  INTOLED();
  OLEDprint("pemasukan = ", String(count));
}
void loop() {
  int UP = digitalRead(17);
  int DOWN = digitalRead(16);
  int reset = digitalRead(18);
  switch(steps){                                    // ini biar ngeloop tiap step soalnya aku pake button, SWITCH GAK NGELOOP SENDIRI ini cuman karena di dalem loopnya ESP32 makanya bisa nge loop
    case 1: {                                       // step 1 cari masukan dari pengguna
    if (UP == 0&& lastUP == 1){
      count++;
      OLEDprint("pemasukan = ", String(count));
      delay(100);                                    
    }
    if (DOWN == 0&& lastDOWN == 1){ 
      if (count > 0) {                               // ini biar angkanya gak minus
        count--;
      }
      OLEDprint("pemasukan = ", String(count));
      delay(100);
    }
    if (reset == 0&& lastreset == 1) {
      save = count;
      if (save > 0){                                  // check kalo penghasilannya lebih dari 0,
      count = 0;
      steps++;
      OLEDprint("pengeluaran = ", String(count));
     }
      else {OLEDprint("twin... penghasilan mu gak boleh 0", "");delay(10000);ESP.restart();}
    }

    delay(10);
    break;
    }

  case 2: {                                          // step 2 cari pengeluaran dari pengguna
    if (UP == 0&& lastUP == 1){
      count++;
      OLEDprint("pengeluaran = ", String(count));
      delay(100);
    }
    if (DOWN == 0&& lastDOWN == 1){ 
     if (count > 0) {                                 // ini biar angkanya gak minus
        count--;
      }
      OLEDprint("pengeluaran = ", String(count));
      delay(100);
    }
    if (reset == 0&& lastreset == 1) {
      steps++;
    }
    delay(10);
    break;
    }

  case 3: {                                           // step 3 total nabungnya brp dan skor
    OLEDprint("Total nabung twin:", String((save - count)*1000));
    delay(2000);  // Show for 2s
    int nilai = score();
    OLEDprint("Score kamu:", String(nilai) + " %");
    steps++;
    break;
    }

    default:
      break;
  }
  lastUP = UP;                                        //debounce
  lastDOWN = DOWN;
  lastreset = reset;
}

void OLEDprint(String line1, String line2) {
  Serial.println(line1);
  if (line2 != "") Serial.println(line2);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(line1);
  if (line2 != "") {
    display.setCursor(0, 16);  // Line 2 a bit lower
    display.println(line2);
  }
  display.display();
}

int score(){
  int truescore = (100*(save - count)/save);
  if (truescore < 0) {
    OLEDprint("Lah twin ini boros amat ", "");
  }else if (truescore < 20) {
    OLEDprint("Belum mulai twin ", "");
  }else if (truescore < 40) {
    OLEDprint("Lumayan lah ", "");
  }else if (truescore < 60) {
    OLEDprint("Bagusss ", "");
  }else if (truescore < 80) {
    OLEDprint("Hebat banget! ", "");
  }else if (truescore < 100) {
    OLEDprint("mantep twin", "");
  }else if (truescore == 100) {
    OLEDprint("makan dulu twin" ,"");
  }

  delay(5000);
  
  Serial.print("score kamu ini twin = ");
  return truescore;
}

void INTOLED(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  Serial.println("OLED failed");
  while(true); // Freeze
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Ready Twin!");
  display.display();
  delay(1000);
}
