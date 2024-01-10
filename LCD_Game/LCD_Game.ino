#include <LiquidCrystal.h>
#include <EEPROM.h>
#define VRX_PIN A0
#define VRY_PIN A1
#define Contrast 0
int xValue=0;
int yValue=0;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
byte nave[8] = {B10000, B11000, B11100, B11110, B11110, B11100, B11000, B10000};
byte ast[8] = {B00000, B00000, B01110, B11111, B11111, B01110, B00000, B00000};
byte bul[8] = {B00000, B00000, B00000, B11111, B11111, B00000, B00000, B00000};
byte exp1[8] = {B00000, B00000, B00000, B00100, B00100, B00000, B00000, B00000};
byte exp2[8] = {B00000, B00000, B01010, B00100, B00100, B01010, B00000, B00000};
byte exp3[8] = {B00000, B10001, B01010, B00100, B00100, B01010, B10001, B00000};
int xNave = 0, yNave = 0;
double xAst=13, yAst=0;
int xTiro=xNave+1, yTiro=yNave;
int explosion = 0;
int xExplosion=xAst, yExplosion=yAst;
boolean Explosion=false;
boolean spawn=true, mens = true;
String game = "MENU";
double vida = 100, spd = 300;
int pontuacao = 0, counter = 0,high =0;
boolean ligado = false, tiro = false, atirando= false,atirou=false;
int buttonState, som=1000, somd=300, somc=150;

int overTime = 0;
void setup() {
 
  Serial.begin(9600);
  analogWrite(6,Contrast);
  high = EEPROM.read(1);
  lcd.begin(16, 2);
  lcd.createChar(1,nave);
  lcd.createChar(2,ast);
  lcd.createChar(3,bul);
  lcd.createChar(4,exp1);
  lcd.createChar(5,exp2);
  lcd.createChar(6,exp3);
  pinMode(8, INPUT_PULLUP);
  pinMode(9,OUTPUT);
  randomSeed(analogRead(0));
}

void movement (){
  xValue=analogRead(VRX_PIN);
  yValue=analogRead(VRY_PIN);
  if(yValue>700 && yNave!=1){
    yNave++;
  }else if(yValue<300 && yNave!=0){
    yNave--;
  }
  if(xValue>700 && xNave!=15){
    xNave++;
  }else if(xValue<300 && xNave!=0){
    xNave--;
  }
  lcd.setCursor(xNave, yNave);
  lcd.write(1);
}

void shot(){
  if(buttonState == 0 && !atirando){
     atirando = true;
    xTiro = xNave+1;
    yTiro = yNave;
    som=1000;
  }
  if(atirando){
    tone(9,som);
    lcd.setCursor(xTiro,yTiro);
    som-=500;
    xTiro++;
    if(xTiro >= 13){
      xTiro=-2;
      atirando = false;
      noTone(9);
    }
    lcd.write(3);
  }
}

void Explosion9(){
  if (Explosion == true){
    explosion++;
    lcd.setCursor(xExplosion, yExplosion);
    if (explosion == 1) {
      lcd.write(4);
    } else if (explosion == 2) {
      lcd.write(5);
    } else if (explosion == 3) {
      lcd.write(6);
      Explosion = false;
      explosion=0;
    }
  }
 
  if(xTiro >= xAst && yTiro == yAst) {
    Explosion = true;
    xExplosion=xAst, yExplosion=yAst;
    noTone(9);
    tone(9,somd);
      pontuacao++;
      vida+=0.15;
      xTiro=-2;
      spawn=true;
      atirando = false;
    }
  }

void movementAsteroide (){
  if (spawn==true){
    noTone(9);
      spawn=false;
      xAst=12;
      yAst=random(0,2);
  }
    lcd.setCursor(xAst, yAst);
    if(xAst!=-1){
    xAst--;
    }
    if(xAst==-1){
    xAst=12;
    spawn=true;
    }
  lcd.write(2);
}

void life(){
  lcd.setCursor(13,0);
  lcd.print((int)vida);
}
void score(){
  lcd.setCursor(13,1);
  lcd.print(pontuacao);
}

void damage(int dano) {
  if(xAst == xNave && yAst == yNave && vida > 0){
    tone(9,somc);
    vida-=dano;
    spawn=true;
  }
  if(vida <= 0){
    game = "GAMEOVER";
  }
}

void gameOver(){
  spd=300;
  noTone(9);
  lcd.setCursor(0,0);
    lcd.print("GAME OVER");
    lcd.setCursor(0,1);
    lcd.print("Score ");
    lcd.print(pontuacao);
    if(pontuacao > high) {
      EEPROM.write(1, pontuacao);
    }
    if(buttonState==0){
      game="GAME";
      vida=100;
      pontuacao=0;
      xNave=0;
    }
    overTime++;
    if(overTime == 60){
      game="MENU";
      vida=100;
      overTime=0;
    }
}

void menu(int maxCount){
  counter++;
  if(counter >= maxCount){
    counter=0;
    if(!mens){
      mens = true;
    }else{
      mens = false;
    }
  }
  lcd.setCursor(3,0);
  lcd.print("Play Game?");
  if(mens){
    lcd.setCursor(5,1);
    lcd.print("HS: ");
    lcd.print(high);
  }
  if(buttonState==0){
    game="GAME";
    spd=300;
  }
}

void loop() {
  lcd.clear();
  buttonState=digitalRead(8);
  if(game == "GAME"){
    movement();
    movementAsteroide();
    life();
    score();
    shot();
    damage(15);
    vida-=0.10;
    if(spd > 0){
      spd-=0.50;
    }
  }else if(game == "MENU"){
    menu(1);
  }else if(game == "GAMEOVER"){
   gameOver();
  }
  Explosion9();
  delay(spd);
}
