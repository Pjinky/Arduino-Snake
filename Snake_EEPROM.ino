#include "U8glib.h"
#include <EEPROM.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);
const int btns = 3;
const int resetbtn = 12;
int foodX;
int foodY;
byte posY1[256];
byte posX1[256];
int posX = 64;
int posY = 40;
int points = 1;
bool dead = false;
String dir;
unsigned long millisCheck;
unsigned long millisNow;

const int addr = 0;

void setup() {
  //Serial.begin(9600);
  points = 1;
  posX = 64;
  posY = 40;
  dir = "right";
  pinMode(btns, OUTPUT);
  pinMode(resetbtn, OUTPUT);
  u8g.setFont(u8g_font_unifont);
  spawnFood();
}

void spawnFood(){
  foodX = random(2, 126);
  foodY = random(18, 62);
}

void checkHighscore(){
  if(points - 1 > EEPROM.read(addr)){
    EEPROM.write(addr, points - 1);
  }
}

void draw(){
  millisNow = millis();
  if(millisNow - millisCheck >= 1000){
    if(dir == "left"){
      posX--;
    }
    if(dir == "right"){
      posX++;
    }
    if(dir == "up"){
      posY--;
    }
    if(dir == "down"){
      posY++;
    }

    if((foodX >= posX-2 && foodX <= posX+2) && (foodY >= posY-2 && foodY <= posY+2)){
      points++;
      posX1[points - 1] = posX;
      posY1[points - 1] = posY;
      spawnFood();
    }
   
    for(int i = points; i > 0; i--){
      posY1[i] = posY1[i-1];
      posX1[i] = posX1[i-1];     
    }      

    u8g.firstPage();
    do  {
      u8g.drawStr(0, 12, "Points: ");
      u8g.setPrintPos(64, 12);
      u8g.print(points-1);
      u8g.drawFrame(0, 16, 128, 48);
      u8g.drawBox(posX, posY, 3, 3);
      u8g.drawBox(foodX, foodY, 3, 3);

      for(int i = 1; i < points; i++){
        u8g.drawBox(posX1[i], posY1[i], 3, 3);
        if((posX >= posX1[i] && posX <= posX1[i]) && (posY >= posY1[i] && posY <= posY1[i])){
          dead = true;
          checkHighscore();
        }
      }
    } while( u8g.nextPage());
    posX1[0] = posX;
    posY1[0] = posY;
  }
  if(posX == 128 || posX == 0 || posY == 64 || posY == 16){
    dead = true;
    checkHighscore();
  }
}

void loop() {
  int button = analogRead(btns);
  int rButton = digitalRead(resetbtn);
  if(!dead){
    //Left
    if(button >= 200 && button <= 225){
  
      if(dir == "down" || dir == "up"){
        dir = "left";
      }
    }
  
    //UP
    if(button >= 150 && button <= 180){
  
      if(dir == "right" || dir == "left"){
        dir = "up";
      }
      
    }
  
    //Select
    if(button >= 240 && button <= 290){
  
    }
  
    //Right
    if(button >= 330 && button <= 345){
  
      if(dir == "up" || dir == "down"){
        dir = "right";
      }
    }
  
    //Down
    if(button >= 500 && button <= 520){
      
      if(dir == "right" || dir == "left"){
        dir = "down";
      }
    }
    draw();
  }else{
    u8g.firstPage();
    do{
      u8g.drawStr(32, 12, "GAME OVER");
      u8g.drawStr(0, 32, "Points: ");
      u8g.setPrintPos(60, 32);
      u8g.print(points-1);
      u8g.drawStr(0, 52, "Highscore: ");
      u8g.setPrintPos(84, 52);
      u8g.print(EEPROM.read(addr));
    }while(u8g.nextPage());
  }
  
  //Reset
  if(rButton > 0){
    setup();
    dead = false;
  }
}
