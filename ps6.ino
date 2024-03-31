#include "lcd_wrapper.h"
#include "mastermind.h"
#include <LiquidCrystal_I2C.h>


void setup(){
  Serial.begin(9600);
  lcd_init();
  randomSeed(analogRead(1)); //generuje ine hodnoty

  for(int i = 6; i < 14; i++){
    pinMode(i, OUTPUT);
  }
  for(int i = 2; i < 6; i++){
    pinMode(i, INPUT);
  }

  pinMode(A0, INPUT);
  
  lcd_print_at(2, 0, "Welcome to MasterMind!");
  lcd_print_at(5, 1, "Guess my secret combination!");

  for (byte i = 0; i < 18; i++) { 
    lcd_scroll_left(); 
    delay(500); 
  }

  delay(1500);
  lcd_clear();

  lcd_print_at(0, 0, "My secret num: ");
}

void loop(){
  char* code = NULL;
  code = generate_code(false, 4);

  Serial.println(code);

  if(code != NULL){ //ked code neni null
    play_game(code);
    free(code);
  }

  else{
    lcd_clear();
    lcd_print_at(2, 0, "Wrong code generated");
    for (byte i = 0; i < 8; i++) { 
      lcd_scroll_left();
      delay(500); 
    }
  }
  
}