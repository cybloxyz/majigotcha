#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <SPI.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display(SCREEN_HEIGHT, SCREEN_WIDTH, OLED_RESET, &Wire);

#define BTN_SW1 D1
#define BTN_SW2 D0
#define BTN_SW3 D3

#define POTENTIO A2
#define BUZZ D10

#define TOUCHOUT_FINGERPRINT D6
#define RX_FINGERPRINT D4
#define TX_FINGERPRINT D5
#define TOUCH D9

#define SDA_OLED D7
#define SCK_OLED D8

struct Pet
{
    int hunger;
    int happiness;
    int energy;
    int health;
    int skill;
    int tame;
    unsigned long age;
};

Pet pet;

enum Screen 
{
    SCREEN_MAIN,
    SCREEN_FEED,
    SCREEN_PLAY,
    SCREEN_HOSPITAL,
    SCREEN_PRIVATE,
    SCREEN_SLEEP
};

Screen currentScreen = SCREEN_MAIN;

void setup() {
    pinMode(BTN_SW1, INPUT_PULLUP);
    pinMode(BTN_SW2, INPUT_PULLUP);
    pinMode(BTN_SW3, INPUT_PULLUP);
    pinMode(BUZZ, OUTPUT);
    pinMode(POTENTIO, INPUT);
    pinMode(TOUCH, INPUT);
    pinMode(TOUCHOUT_FINGERPRINT, INPUT);
    mySerial.begin(57600);
    Serial.begin(9600);

    display.begin(0x3c, true);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSH110X_WHITE);
    display.setCursor(0, 0);
    display.println('Majigotcha init...');
    display.display();
    delay(1000);

    pet.hunger = 80;
    pet.energy = 80;
    pet.health = 80;
    pet.happiness = 80;
    pet.skill = 0;
    pet.tame = 0;
    pet.age = 0;
}

unsigned long lastUpdateStat = 0;
unsigned long lastUpdateTame = 0;
unsigned long lastUpdateHealth = 0;

void updatePet() {
    if (millis() - lastUpdateStat > 10000) {
        pet.energy--;
        pet.hunger--;
        pet.happiness--;

        if (pet.hunger < 0) pet.hunger = 0;
        if (pet.energy < 0) pet.hunger = 0;
        if (pet.happiness < 0) pet.happiness = 0;

        pet.age += 5;
        lastUpdateStat = millis();
    }
    if (millis() - lastUpdateTame > 300000) {
        pet.tame--;

        if (pet.tame < 0) pet.tame = 0;
        lastUpdateTame = millis();
    }
    if (millis() - lastUpdateHealth > 720000) {
        pet.health--;

        if (pet.health < 0) pet.health = 0;
        lastUpdateHealth = millis();
    }
}

unsigned long lastButtonPress = 0;

void checkButtons() {
    if (millis() - lastButtonPress < 200) return;

    if (digitalRead(BTN_SW2) == LOW) {
        currentScreen = SCREEN_FEED;
        tone(BUZZ, 1000, 50);
        lastButtonPress = millis();
    }
    else if (digitalRead(BTN_SW3) == LOW) {
        currentScreen = SCREEN_PRIVATE;
        tone(BUZZ, 1200, 50);
        lastButtonPress = millis();
    }
}

bool menuOpen = false;
int menuSelection = 0;
const int NUM_MENU = 4; 

void checkMenu() {
    if (digitalRead(BTN_SW1) == LOW && (millis() - lastButtonPress > 300)) {
        if (!menuOpen) {
            menuOpen = true; 
            tone(BUZZ, 1500, 50);
        } else {
       
            if (menuSelection == 0) currentScreen = SCREEN_MAIN;
            else if (menuSelection == 1) currentScreen = SCREEN_PLAY;
            else if (menuSelection == 2) currentScreen = SCREEN_SLEEP;
            else if (menuSelection == 3) currentScreen = SCREEN_HOSPITAL;
            
            menuOpen = false; 
            tone(BUZZ, 2000, 100);
        }
        lastButtonPress = millis();
    }

    
    if (menuOpen) {
        int potValue = analogRead(POTENTIO); 
   
        menuSelection = map(potValue, 0, 4095, 0, NUM_MENU - 1);
    }
}

void handleScreenLogic() {
  switch(currentScreen) {

    case SCREEN_FEED:
      pet.hunger += 10;
      if (pet.hunger > 100) pet.hunger = 100;
      currentScreen = SCREEN_MAIN;
      break;

    case SCREEN_PLAY:
      pet.happiness += 10;
      pet.energy -= 5;
      if (pet.happiness > 100) pet.happiness = 100;
      if (pet.energy < 0) pet.energy = 0;
      currentScreen = SCREEN_MAIN;
      break;

    case SCREEN_SLEEP:
      pet.energy += 15;
      if (pet.energy > 100) pet.energy = 100;
      currentScreen = SCREEN_MAIN;
      break;

    case SCREEN_HOSPITAL:
      pet.health += 5;
      if (pet.health > 100) pet.health = 100;
      currentScreen = SCREEN_MAIN;
      break;

    case SCREEN_PRIVATE:
      pet.tame += 2;
      if (pet.tame > 100) pet.tame = 100;
      currentScreen = SCREEN_MAIN;
      break;

    case SCREEN_MAIN:
      break; 
  }
}

//stats
const unsigned char PROGMEM happy[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00010000, 0b00010000,
  0b00001010, 0b10100000,
  0b00010001, 0b00010000,
  0b00010000, 0b00010000,
  0b00001000, 0b00100000,
  0b00000111, 0b11000000,
  0b00001000, 0b00100000,
  0b00010100, 0b01010000,
  0b00010010, 0b10010000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00001100, 0b01100000
};

const unsigned char PROGMEM sleep[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00010000, 0b00010000,
  0b00001110, 0b11100000,
  0b00010001, 0b00010000,
  0b00010000, 0b00010000,
  0b00001000, 0b00100000,
  0b00000111, 0b11000000,
  0b00001000, 0b00100000,
  0b00010100, 0b01010000,
  0b00010010, 0b10010000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00001100, 0b01100000
};

const unsigned char PROGMEM hungry[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00010000, 0b00010000,
  0b00001010, 0b10100000,
  0b00010000, 0b00010000,
  0b00010011, 0b10010000,
  0b00001011, 0b10100000,
  0b00000111, 0b11000000,
  0b00001000, 0b00100000,
  0b00010100, 0b01010000,
  0b00010010, 0b10010000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00001100, 0b01100000
};

const unsigned char PROGMEM badMood[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00010000, 0b00010000,
  0b00001010, 0b10100000,
  0b00010000, 0b00010000,
  0b00010011, 0b10010000,
  0b00001000, 0b00100000,
  0b00000111, 0b11000000,
  0b00001000, 0b00100000,
  0b00010100, 0b01010000,
  0b00010010, 0b10010000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00001100, 0b01100000
};

const unsigned char PROGMEM sick[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00010000, 0b00010000,
  0b00001010, 0b10100000,
  0b00010100, 0b00010000,
  0b00010011, 0b10010000,
  0b00001000, 0b00100000,
  0b00000111, 0b11000000,
  0b00001000, 0b00100000,
  0b00010100, 0b01010000,
  0b00010010, 0b10010000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00001100, 0b01100000
};

const unsigned char PROGMEM sad[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00010000, 0b00010000,
  0b00001110, 0b11100000,
  0b00010100, 0b01010000,
  0b00010011, 0b10010000,
  0b00001000, 0b00100000,
  0b00000111, 0b11000000,
  0b00001000, 0b00100000,
  0b00010100, 0b01010000,
  0b00010010, 0b10010000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00001100, 0b01100000
};

const unsigned char PROGMEM curious[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00010000, 0b00010000,
  0b00001010, 0b10100000,
  0b00010010, 0b10010000,
  0b00010000, 0b00010000,
  0b00001000, 0b00100000,
  0b00000111, 0b11000000,
  0b00001000, 0b00100000,
  0b00010100, 0b01010000,
  0b00010010, 0b10010000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00001100, 0b01100000
};

const unsigned char PROGMEM aBitHappy[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00010000, 0b00010000,
  0b00001000, 0b00100000,
  0b00010010, 0b10010000,
  0b00010001, 0b00010000,
  0b00001000, 0b00100000,
  0b00000111, 0b11000000,
  0b00001000, 0b00100000,
  0b00010100, 0b01010000,
  0b00010010, 0b10010000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00001100, 0b01100000
};

const unsigned char PROGMEM notTame[] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00010000, 0b00010000,
  0b00100100, 0b01001000,
  0b00010010, 0b10010000,
  0b00010000, 0b00010000,
  0b00001000, 0b00100000,
  0b00000111, 0b11000000,
  0b00001000, 0b00100000,
  0b00010100, 0b01010000,
  0b00010010, 0b10010000,
  0b00001100, 0b01100000,
  0b00010011, 0b10010000,
  0b00001100, 0b01100000
};

void render() {
  display.clearDisplay();

  // Choose the right sprite based on pet stats
  const unsigned char* sprite;
  if (currentScreen = SCREEN_SLEEP) {
    sprite = sleep;
  }
  else if (pet.health < 70) {
    sprite = sick;
  } 
  else if (pet.hunger < 30 || pet.happiness < 30 || pet.energy < 30) {
    sprite = sad;
  }  
  else if (pet.hunger < 40 || pet.happiness < 20 || pet.energy < 30) {
    sprite = badMood;
  }  
  else if (currentScreen = SCREEN_PRIVATE || pet.tame < 30) {
    sprite = notTame;
    if (pet.tame > 30) sprite = curious;
    if (pet.tame > 80) sprite = happy;
  }  
  else if (pet.hunger > 50 && pet.happiness > 50 && pet.energy > 50) {
    sprite = happy;
  } else {
    sprite = aBitHappy;
  }

  // Draw the pet sprite (centered horizontally, near the top)
  display.drawBitmap(56, 2, sprite, 16, 16, SSH1106_WHITE);

  // Draw stat bars below the pet
  display.setTextSize(1);

  display.setCursor(0, 24);
  display.print("HUN ");
  drawBar(24, 24, pet.hunger);

  display.setCursor(0, 34);
  display.print("HAP ");
  drawBar(24, 34, pet.happiness);

  display.setCursor(0, 44);
  display.print("ENG ");
  drawBar(24, 44, pet.energy);

  // Button labels at the bottom
  display.setCursor(0, 56);
  display.println("[Feed] [Play] [Sleep] [Health] [Tame]");

  display.display();
}

// Draws a stat bar: empty rectangle with a filled portion based on value (0 to 100)
void drawBar(int x, int y, int value) {
  int barWidth = 100;
  int barHeight = 6;
  int fillWidth = map(value, 0, 100, 0, barWidth);

  display.drawRect(x, y, barWidth, barHeight, SSH1106_WHITE);       // outline
  display.fillRect(x, y, fillWidth, barHeight, SSH1106_WHITE);      // filled portion
}

void loop() {
    checkButtons();
    checkMenu();
    updatePet();
    handleScreenLogic();
    render();
    delay(1000);
}