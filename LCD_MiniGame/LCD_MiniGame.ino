#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


const int joy_SW_pin = 2; // digital pin connected to switch output
const int joy_X_pin = 0; // analog pin connected to X output
const int joy_Y_pin = 1; // analog pin connected to Y output

byte game_choice = 0;
byte game_select_cursor = 1;
int joy_SW = 0; // joystick button value
int joy_X = 0; // joystick X value
int joy_Y = 0; // joystick Y value
int mat[2][16] = {
  {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
  {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8}
  }; // for drawing, 8 means blank
  
int score = 0;
int dino_object_count = 0;
int dino_cactus_appeared = 0;
bool dino_dead = false;
int dino_bird_appeared = 0;
const int dino_object_max = 4;
const int dino_jump_duration = 4; // amount of frame that dino stays in the air
int dino_jump_time = 0; // when is 0, dino is on the ground
byte animation_tick = 0; //temporary value for animation

void setup() {
  pinMode(joy_SW_pin, INPUT);
  digitalWrite(joy_SW_pin, HIGH);
  lcd.begin(16, 2);
//  while(true){
//    debug_joystick();
//  }
  while(game_choice==0){
    main_menu();
  }
  if(game_choice == 1){
    set_dino_char();
  }else if(game_choice == 2){
    set_snake_char();
  }
}

void loop() {
  int dino_difficulty = 200; // Lower the harder. It gets harder over time
  int max_dino_difficulty = 30;
  if(game_choice == 1){
    dino_game();
    score += 1;
    if(dino_dead){
      lcd.clear();
      lcd.setCursor(1,1);
      lcd.write(1);
      delay(100);
      lcd.clear();
      delay(100);    
      lcd.setCursor(1,1);
      lcd.write(1);
      delay(100);
      lcd.clear();
      delay(100);     
      lcd.setCursor(1,1);
      lcd.write(1);
      delay(100);
      lcd.clear();
      lcd.print("Score: "+String(score));
      while(true){
        delay(1000);
      }
    }
    delay(dino_difficulty);
    if(dino_difficulty > max_dino_difficulty){
      dino_difficulty -= 2;
    }
  }else if(game_choice == 2){
    snake_game();
  }else{
    lcd.setCursor(0,0);
    lcd.print("no such game");
  }
}

void clear_custom_char() 
{
  // ensures all custom character slots are clear before new custom
  // characters can be defined. 
  byte blank[8] =
  {
    B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000
  };
  for(int i = 0; i < 8; i++)
  {
    lcd.createChar(i, blank);
  }
}

void main_menu() 
{
  lcd.clear();
  // entrance menu
  read_joystick();
  if(game_select_cursor==1){
    lcd.setCursor(0,0);
    lcd.print("1.Dinosaur <-");
    lcd.setCursor(0,1);
    lcd.print("2.Snake");
  }
  if(game_select_cursor==2){
    lcd.setCursor(0,0);
    lcd.print("1.Dinosaur");
    lcd.setCursor(0,1);
    lcd.print("2.Snake <-");
  }
  if(joy_X<300){
      game_select_cursor=2;
  }
  if(joy_X>700){
      game_select_cursor=1;
  }
  if(joy_SW==0){
    if(game_select_cursor==1){
      game_choice=1;
    }
    if(game_select_cursor==2){
      game_choice=2;
    }
  }
  delay(300);
}
void dino_game() 
{
  read_joystick();
  bool btnDown = (joy_X<300);
  bool btnUp = (joy_X>700);
  for(int i = 0; i<2;i++){
    for(int j = 0; j<15; j++){
      mat[i][j] = mat[i][j+1];
    }
  }
  if(mat[1][0]==5 || mat[1][0]==7){
    dino_object_count -= 1;
  }
  mat[0][15] = 8;
  mat[1][15] = 8;
  dino_generate_obstacles();
  draw_dino_background();


  
  if(btnDown == false && dino_jump_time==0){
    lcd.setCursor(1,1);
    if(mat[1][1]==7 || mat[1][1]==5){
      dino_dead = true;
    }
    lcd.write(byte(1+animation_tick));  
  }
  if(btnDown == false && dino_jump_time!=0){
    lcd.setCursor(1,0);
    if(mat[1][0]==6){
      dino_dead = true;
    }
    lcd.write(byte(2));
  }
  if(btnUp == true && dino_jump_time ==0){
    dino_jump_time=dino_jump_duration;
  }
  if(btnDown == true){
    lcd.setCursor(1,1);
    if(mat[1][1]==5){
      dino_dead = true;
    }
    lcd.write(byte(3+animation_tick));
    dino_jump_time = 0;
  }
  lcd.setCursor(15,0);
  lcd.write(byte(0));
//  lcd.setCursor(0,0);
//  lcd.write(dino_object_count+'0');
  dino_jump_time = (dino_jump_time>0)?dino_jump_time-1:0;  
  animation_tick = (animation_tick==0)?1:0; // alternate between 0 and 1
}

void cleanmat(){
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 16; j++){
      mat[i][j] = 8;
    }
  }
}
void draw_dino_background(){ 
  for(int i = 0; i<2;i++){
    for(int j = 0; j<16; j++){
      lcd.setCursor(j,i);
      if(mat[i][j]==8){
        lcd.print(" ");
      }else{
        lcd.write(mat[i][j]);        
      }
    }
  }
}
void dino_generate_obstacles(){
  bool cactus_spawn = false;
  bool bird_spawn = false;  
  if(dino_cactus_appeared>=3 && dino_bird_appeared>=3){
    cactus_spawn = true;
  }
  if(dino_cactus_appeared>=3 && dino_bird_appeared>=3){
    bird_spawn = true;
  }
  
  int randint = random(1,10); 
  if(randint<4 && dino_object_count<dino_object_max){
    if(bird_spawn&&cactus_spawn){
      int randint = random(1,10);
      if(randint<5){
        mat[0][15] = 6;
        mat[1][15] = 7;
        dino_bird_appeared = -1;
      }else {
        mat[1][15] = 5;
        dino_cactus_appeared = -1;
      }
      dino_object_count += 1;
    }
    if(bird_spawn && !cactus_spawn){
      mat[0][15] = 6;
      mat[1][15] = 7;
      dino_bird_appeared = -1;
      dino_object_count += 1;
    }
    if(!bird_spawn && cactus_spawn){
      mat[1][15] = 5;
      dino_cactus_appeared = -1;
      dino_object_count += 1;
    }
  }
  dino_bird_appeared += 1;
  dino_cactus_appeared += 1;
}
void set_dino_char() 
{
  // array of custom charactor on LCD
  byte sun[8] = {
    B00000,
    B00000,
    B01110,
    B11111,
    B11111,
    B11111,
    B01110,
    B00000
  };
  byte dino_run1[] = {
    B00111,
    B00101,
    B00111,
    B00100,
    B10111,
    B11110,
    B01110,
    B11011
  };
  byte dino_run2[] = {
    B00111,
    B00101,
    B00111,
    B00100,
    B10111,
    B11110,
    B01111,
    B01100
  };
  byte cactus[] = {
    B00100,
    B00100,
    B00101,
    B10101,
    B10111,
    B11100,
    B00100,
    B00100
  };
  byte dino_duck1[] = {
    B00000,
    B00000,
    B00011,
    B00101,
    B10111,
    B11110,
    B01111,
    B01000
  };
  byte dino_duck2[] = {
    B00000,
    B00000,
    B00011,
    B00101,
    B10111,
    B11110,
    B01110,
    B10010
  };
  byte bird_top[] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00111,
    B01110,
    B11100
  };
  byte bird_bottom[] = {
    B11100,
    B01110,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000
  };
  lcd.createChar(0, sun);
  lcd.createChar(1, dino_run1);
  lcd.createChar(2, dino_run2);
  lcd.createChar(3, dino_duck1);
  lcd.createChar(4, dino_duck2);
  lcd.createChar(5, cactus);
  lcd.createChar(6, bird_top);
  lcd.createChar(7, bird_bottom);
}

void snake_game() 
{
  lcd.print("Coming Soon...");
  delay(200);
}

void set_snake_char() 
{

//  lcd.createChar(0, smiley);
//  lcd.createChar(1, dino_run1);
//  lcd.createChar(2, dino_run2);
//  lcd.createChar(3, dino_duck1);
//  lcd.createChar(4, dino_duck2);
//  lcd.createChar(5, cactus);
//  lcd.createChar(6, bird_top);
//  lcd.createChar(7, bird_bottom);
}

void debug_joystick() 
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(analogRead(joy_X_pin));
  lcd.setCursor(6, 0);
  lcd.print(analogRead(joy_Y_pin));
  lcd.setCursor(0, 1);
  lcd.print(digitalRead(joy_SW_pin));
  delay(30); 
}
void read_joystick() 
{
  joy_SW = digitalRead(joy_SW_pin); // digital pin connected to switch output
  joy_X = analogRead(joy_X_pin); // analog pin connected to X output
  joy_Y = analogRead(joy_Y_pin); // analog pin connected to Y output
}
