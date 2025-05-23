#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);


const int button = 2;
bool isJumping = false;
unsigned long jumpStartTime = 0;
unsigned long lastJumpTime = 0;
int score = 0;
bool jumpUsed = false;

byte plane[8] = {
   B00000,
  B00000,
  B00000,
  B11100,
  B01111,
  B11100,
  B00000,
  B00000

};

byte obstacle[8] = {
   B00000,
  B00100,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111
};

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(button, INPUT);
  lcd.createChar(0, plane);
  lcd.createChar(1, obstacle);
  randomSeed(analogRead(A0));

 
  lcd.setCursor(0,0);
  lcd.write(byte(0));
  lcd.setCursor(2, 0);
  delay(200);
  lcd.print("Aero Escape");
  lcd.setCursor(14,0);
  delay(200);
  lcd.write(byte(0));
  lcd.createChar(1,obstacle);
  delay(250);
  lcd.setCursor(15,0);
  lcd.write(byte(1));
  
  lcd.setCursor(1, 1);
  lcd.print("Press to start");
  
  while(digitalRead(button) == LOW);
  lcd.clear();
  delay(100);
  lcd.setCursor(0,0);
  lcd.print("Rule:");
  delay(1500);
  lcd.clear();
  lcd.print("Per obstacle one");
  delay(2000);
  lcd.scrollDisplayLeft();
  lcd.clear();
  lcd.print(" jump allowed.");
  delay(2500);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("loading...");
  lcd.blink();
  delay(2000);
  lcd.noBlink();
}

void loop() {
 
  int obstaclePos = 15;
  int prevObstaclePos = 15;
  bool prevJump = false;
  isJumping = false;
  jumpUsed = false;

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.write(byte(0)); // plane at column 1

  //  Random delay before obstacle appears
  delay(random(800, 2000));

  while (obstaclePos >= 0) {
    unsigned long currentTime = millis();

    //  Allow jump only once per obstacle
    if (!jumpUsed && digitalRead(button) == HIGH && !isJumping) {
      isJumping = true;
      jumpStartTime = currentTime;
      lastJumpTime = currentTime;
      jumpUsed = true;
    }

    //  Jump ends after fixed time
    if (isJumping && currentTime - jumpStartTime > 500) {
      isJumping = false;
    }

    // Update plane only if jump changed
    if (prevJump != isJumping) {
      lcd.setCursor(0, prevJump ? 0 : 1);
      lcd.print(" ");
      lcd.setCursor(0, isJumping ? 0 : 1);
      lcd.write(byte(0));
      prevJump = isJumping;
    }

    //  Clear old obstacle
    lcd.setCursor(prevObstaclePos, 1);
    lcd.print(" ");
    //  Draw new obstacle
    lcd.setCursor(obstaclePos, 1);
    lcd.write(byte(1));
    delay(random(1,99));
    prevObstaclePos = obstaclePos;

    //  Collision check
    if (obstaclePos == 1 && !isJumping) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Game Over");
      lcd.setCursor(4, 1);
      lcd.print("Score: ");
      lcd.print(score);
      delay(3000);
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("restarting...");
      lcd.blink();
      delay(2500);
      lcd.noBlink();
      score = -1;
      return;
    }

    //  Successful dodge
    if (obstaclePos == 0 && isJumping) {
      score++;
    }

    //  Move obstacle
    obstaclePos--;

    //  Random frame speed every step!
    delay(random(120, 250));
  }

  //  Reset obstacle gap randomness
  delay(random(600, 1500));
  
}

