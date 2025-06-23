#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int count, save, lastUP, lastDOWN, lastreset;
int steps, choices = 0;
void OLEDprint(String line1, String line2 = "", String line3 = "");


void setup() {
  pinMode(17, INPUT_PULLUP);pinMode(16, INPUT_PULLUP);pinMode(18, INPUT_PULLUP);
  Serial.begin(115200);
  InitOLED();
}
void loop() {
  int UP = digitalRead(17);
  int DOWN = digitalRead(16);
  int reset = digitalRead(18);
  static bool IsMenuDisplayed = false;
  
  switch(steps){                                    // ini biar ngeloop tiap step soalnya aku pake button, SWITCH GAK NGELOOP SENDIRI ini cuman karena di dalem loopnya ESP32 makanya bisa nge loop
    case 0: {                                       // MENU
      String MENU[] = {"Tracker", "Upload", "Settings"};
      int menuSize = sizeof(MENU) / sizeof(MENU[0]);

      if (!IsMenuDisplayed) {
      OLEDprint(MENU[(choices - 1 == -1) ? menuSize - 1 : choices - 1], String("> ") + MENU[choices], MENU[(choices + 1 == menuSize) ? 0 : choices + 1]);
      IsMenuDisplayed = true;
      } 
      if (UP == 0&& lastUP == 1){
        if (choices == (sizeof(MENU)/sizeof(MENU[0]))-1){
          choices=0;
        }
        else {
          choices++;
        }
        OLEDprint(MENU[(choices - 1 == -1) ? menuSize - 1 : choices - 1], String("> ") + MENU[choices], MENU[(choices + 1 == menuSize) ? 0 : choices + 1]);
        Serial.print(choices);
        delay(100);                                    
      }
      if (DOWN == 0&& lastDOWN == 1){ 
        if (choices > 0){
          choices--;
        }
        else {
          choices=(sizeof(MENU)/sizeof(MENU[0]))-1;
        }
        OLEDprint(MENU[ (choices - 1 == -1) ? menuSize - 1 : choices - 1] ,  String("> ") + MENU[choices],  MENU[(choices + 1 == menuSize) ? 0 : choices + 1]);
        Serial.print(choices);
        delay(100);
      }
      if (reset == 0&& lastreset == 1) {
        if (choices == 0) {
          steps++;
        }
        else if (choices == 1) {
          steps = 5;
        }
        else if (choices == 2) {
          steps = 6;
        }
        else {
          OLEDprint("wow a bug", "", "");
          delay(1000);
          ESP.restart();
        }
      }

      delay(10);
      break;
      
    }
    case 1: {                                       // step 1 cari masukan dari pengguna
    OLEDprint("pemasukan = ", String(count), "");
    if (UP == 0&& lastUP == 1){
      count++;
      delay(100);                                    
    }
    if (DOWN == 0&& lastDOWN == 1){ 
      if (count > 0) {                               // ini biar angkanya gak minus
        count--;
      }
      delay(100);
    }
    if (reset == 0&& lastreset == 1) {
      save = count;
      if (save > 0){                                  // check kalo penghasilannya lebih dari 0,
      count = 0;
      steps++;
      OLEDprint("pengeluaran = ", String(count), "");
     }
      else {OLEDprint("twin... penghasilan mu gak boleh 0", "", "");delay(5000);steps = 0; IsMenuDisplayed = false;}
    }

    delay(10);
    break;
    }

  case 2: {                                          // step 2 cari pengeluaran dari pengguna
    if (UP == 0&& lastUP == 1){
      count++;
      OLEDprint("pengeluaran = ", String(count), "");
      delay(100);
    }
    if (DOWN == 0&& lastDOWN == 1){ 
     if (count > 0) {                                 // ini biar angkanya gak minus
        count--;
      }
      OLEDprint("pengeluaran = ", String(count), "");
      delay(100);
    }
    if (reset == 0&& lastreset == 1) {
      steps++;
    }
    delay(10);
    break;
    }

  case 3: {                                           // step 3 total nabungnya brp dan skor
    OLEDprint("Total nabung twin:", String((save - count)*1000), "");
    delay(2000);  // Show for 2s
    int nilai = score();
    OLEDprint("Score kamu:", String(nilai) + " %", "");
    delay(5000);  
    steps++;
    break;
    }
  case 4: {
    OLEDprint("save ga?", "atau langsung ke menu", "");
    if (UP == 0&& lastUP == 1){                       // OK
      delay(5000);
      steps = 0;
    }
    if (DOWN == 0&& lastDOWN == 1){                   // NOT OK
      delay(5000);
      steps = 0;
    }
    IsMenuDisplayed = false;
    break;
  }
  default: {
    OLEDprint("something broke", "returning to menu", "");
    delay(5000);
    steps = 0;
    IsMenuDisplayed = false;
    break;
    }    
  }
  lastUP = UP;                                        //debounce
  lastDOWN = DOWN;
  lastreset = reset;
}

void OLEDprint(String line1, String line2, String line3) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(line1);
  if (line2 != "") {
    display.setCursor(0, 16);  // Line 2 a bit lower
    display.println(line2);
  }
  if (line3 != "") {
  display.setCursor(0, 32);
  display.println(line3);
  }
  GUIbutton();
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

void InitOLED(){
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

void GUIbutton(){
  display.drawFastHLine(0, 53, 127, SSD1306_WHITE);
  display.setCursor(0,56);
  display.cp437(true);
  display.write(0x1F);
  display.write(0x20);
  display.write(0x20);
  display.write(0xB3);
  display.write(0x20);
  display.write(0x20);
  display.write(0x1E);
  display.write(0x20);
  display.write(0x20);
  display.write(0xB3);
  display.write(0x20);
  if (steps != 0){display.write(0x20);}
  if (steps == 0){display.print("MENU");}
  else {display.print("OK");}
  if (steps != 0){display.write(0x20);}
  display.write(0x20);
  display.write(0xB3);
  display.write(0x20);
  display.write(0x20);
  if (steps == 0) {display.print(choices);}
  else if (steps > 4){}
  else {display.print(steps);}
}
