#include <Arduino.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "mastermind.h"
#include "lcd_wrapper.h"


char* generate_code(bool repeat, int length){
  if(length < 1){
    return NULL;
  }

  if(repeat == false && length > 10){
    return NULL;
  }

  //srand(time(0));
  char* code = calloc(1, length + 1);
  for(byte i = 0; i < length; i++){
    //byte random = rand() % 10;
    int r = random(10);
    char c = r + '0';
    if(repeat == false){ //pozrie sa ci tam uz je
    	if(strchr(code, c)){
    		i--;
    		continue;
		  }
	  }
    code[i] = c;
  }

  /*lcd_print_at(10, 1, code);
  delay(2000);
  lcd_clear();*/

  return code;
}

int peg_a, peg_b;

void get_score(const char* secret, const char* guess, int* peg_a, int* peg_b){
  int a = 0, b = 0;
  for(byte i = 0; i < strlen(secret); i++){
    if(strchr(guess, secret[i])){ //ak sa nachadza secret[i] v guess
      b++;
      if(secret[i] == guess[i]){ //ak je aj na spravnej pozicii
        a++;
        b--;
      }
    }
  }

  *peg_a = a;
  *peg_b = b;
}

void render_leds(const int peg_a, const int peg_b){
  byte r[] = {7, 9, 11, 13};
  byte b[] = {6, 8, 10, 12};

  for(byte i = 0; i < peg_a; i++){
    digitalWrite(r[i], HIGH); //postupne rozsvieti
  }

  for(byte i = 0; i < peg_b; i++){
    digitalWrite(b[i], HIGH);
  }
}

void turn_off_leds(){
  byte leds[] = {6,7,8,9,10,11,12,13};
  for(byte i = 0; i < 8; i++){
    digitalWrite(leds[i], LOW);
  }
}

char p[1];    //kedze ich vyuzivam vo viacerych funkciach
char* pa = calloc(1, sizeof(char));
char* pb = calloc(1, sizeof(char));
char* history[10];
int entry_nr = 0;
char guess[] = "0000";

void render_history(char* secret, char** history, const int entry_nr){
  lcd_clear();
  get_score(secret, history[entry_nr], &peg_a, &peg_b);
  lcd_print_at(3, 0, history[entry_nr]);

  pa[0] = (char)(peg_a + '0');
  //pb[0] = (char)(peg_b + '0');
  sprintf(pb, "%d", peg_b);
  p[0] = (char)(entry_nr+1 + '0');
  /*pa[1] = '\0';
  pb[1] = '\0';
  p[1] = '\0';*/
  lcd_print_at(0, 0, p);
  lcd_print_at(1, 0, ": ");
  lcd_print_at(8, 0, pa);
  lcd_print_at(9, 0, "A");
  lcd_print_at(10, 0, pb);
  lcd_print_at(11, 0, "B");
  render_leds(peg_a, peg_b);
  lcd_print_at(0, 1, "Your guess: ");   
  lcd_print_at(12, 1, guess);
  delay(2000);
  turn_off_leds();
  lcd_clear();
}

void vypis(){
  pa[0] = (char)(peg_a + '0');
  //pb[0] = (char)(peg_b + '0');
  //sprintf(pa, "%d", peg_a);   //ked dam sprintf tak pri vycerpani pokusov mi nepise secret
  sprintf(pb, "%d", peg_b);
  //p[0] = (char)(pokusy + '0');    //pokusy su lokalna prem.
  //pa[1] = '\0';
  //pb[1] = '\0';
  //p[1] = '\0';
  lcd_print_at(0, 0, p);
  lcd_print_at(1, 0, ": ");
  lcd_print_at(8, 0, pa);
  lcd_print_at(9, 0, "A");
  lcd_print_at(10, 0, pb);
  lcd_print_at(11, 0, "B");
  //lcd_print_at(3, 0, history[pokusy-1]);    //aj tu
}

bool gameOver = false; //aby sa hra znova nespustila
int pokusy = 0;   //ked je pokusy lok. tak pri vyhre mi to vynuluje

void play_game(char* secret){ 
  for(int i = 0; i < 10; i++){
    history[i] = calloc(10, sizeof(char));
  }

  //char guess[] = "0000"; //ako glob.
  byte g0 = 0, g1 = 0, g2 = 0, g3 = 0;
  

  while(peg_a != 4 && pokusy < 9 && gameOver == false){
    if(pokusy > 0){
      vypis();
      p[0] = (char)(pokusy + '0'); 
      lcd_print_at(3, 0, history[pokusy-1]);    
    }
    lcd_print_at(0, 1, "Your guess: ");   
    lcd_print_at(12, 1, guess);
    get_score(secret, history[pokusy-1], &peg_a, &peg_b);

    if(digitalRead(BTN_1_PIN) == HIGH && digitalRead(BTN_2_PIN) == LOW && digitalRead(BTN_3_PIN) == LOW){
      g3++;
      if(g3 > 9){
        g3 = 0;
      }
      guess[3] = (char)(g3 + '0'); //zapisovanie do guess
      delay(300);
    }
    if(digitalRead(BTN_2_PIN) == HIGH && digitalRead(BTN_1_PIN) == HIGH){
      if(entry_nr > 0){
        if(pokusy == entry_nr){ //aby nezobrazovalo aktualne vyhodnoteny zaznam
          entry_nr--;
        }
        render_history(secret, history, entry_nr - 1);
        entry_nr--;
      }     
    }
    if(digitalRead(BTN_3_PIN) == HIGH && digitalRead(BTN_1_PIN) == HIGH){
      if(entry_nr < pokusy - 1){
        render_history(secret, history, entry_nr + 1);
        entry_nr++;
      }    
    }
    if(digitalRead(BTN_2_PIN) == HIGH && digitalRead(BTN_1_PIN) == LOW){
      g2++;
      if(g2 > 9){
        g2 = 0;
      }
      guess[2] = (char)(g2 + '0');
      delay(300);
    }
    if(digitalRead(BTN_3_PIN) == HIGH && digitalRead(BTN_1_PIN) == LOW){
      g1++;
      if(g1 > 9){
        g1 = 0;
      }
      guess[1] = (char)(g1 + '0');
      delay(300);
    }
    if(digitalRead(BTN_4_PIN) == HIGH){
      g0++;
      if(g0 > 9){
        g0 = 0;
      }
      guess[0] = (char)(g0 + '0');
      delay(300);
    }

    if(digitalRead(BTN_ENTER_PIN) == HIGH){     
      strcpy(history[pokusy], guess); //zapis guess do history
      pokusy++;
      entry_nr++;
      //pok[0] += (char)(pokusy + '0');
      get_score(secret, guess, &peg_a, &peg_b);
      render_leds(peg_a, peg_b);
      lcd_clear();
      if(peg_a != 4){
        lcd_print_at(0, 0, "LEDKY...");
        g0 = 0, g1 = 0, g2 = 0, g3 = 0; //vynulovanie po vyhodnoteni
        strcpy(guess, "0000");
        delay(3500);
        lcd_clear();
        turn_off_leds();
        lcd_print_at(12, 1, guess);
      }           
    }
  }

  if(pokusy == 9){
    lcd_clear();
    p[0] = (char)(pokusy + '0'); 
    vypis();
    lcd_print_at(3, 0, history[pokusy-1]); 
    
    lcd_print_at(0, 1, "You lost...");
    delay(1500);
    lcd_clear();
    lcd_print_at(0, 1, secret);
    lcd_print_at(4, 1, "was code");
    //delay(1500);
    gameOver = true;

    for(int i = 0; i < 10; i++){
      free(history[i]);
    }
    free(pa);
    free(pb);
  }

  if(peg_a == 4){
    lcd_clear();
    p[0] = (char)(pokusy + '0'); 
    vypis();
    lcd_print_at(3, 0, guess); 

    lcd_print_at(0, 1, "You won!"); 

    gameOver = true;

    for(int i = 0; i < 10; i++){
      free(history[i]);
    }
    free(pa);
    free(pb);
  }
}