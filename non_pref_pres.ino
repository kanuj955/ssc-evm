#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SPI.h>
#include <EEPROM.h>
#include <SD.h>

File myFile;
// Constants
const byte NUM_PRESIDENTIAL_CANDIDATES = 4;
const byte LCD_RS = 2;
const byte LCD_E = 3;
const byte LCD_D4 = 4;
const byte LCD_D5 = 5;
const byte LCD_D6 = 6;
const byte LCD_D7 = 7;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
const byte SD_CS = 10;
const byte enable_pin = 1;
const byte pin_addr = 0;
const byte pin_length = 0;
const byte counter_addr = pin_addr + pin_length;


class Candidate      // Base class for candidates
{
  private:
    char Code;
    String Name;
  public:
    Candidate() {
      Code = 0;
      Name = " ";
    }
    Candidate(char candCode, String candName) {
      Code = candCode;
      Name = candName;
    }
    char getCode() {
      return Code;
    }
    String getName() {
      return Name;
    }
};

// Class for Presidential Candidate
class Presidential : public Candidate {
  private:
    word votes;
  public:
    Presidential() {
      votes = 0;
    }
    Presidential(char candCode, String candName) : Candidate(candCode, candName) {
      // Lets go with empty votes every time the machine starts
      votes = 0;
    }
    void vote() {
      votes += 1;
    }
    word getVotes() {
      return votes;
    }

};


//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {8, 9, 14, 15}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {16, 17, 18, 19}; //connect to the column pinouts of the keypad

//Candidate classes

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
char key;
char message_president;
Presidential president[] = {  Presidential('A', "Candidate A"),
                              Presidential('B', "Candidate B"),
                              Presidential('C', "Candidate C"),
                              Presidential('D', "Candidate D")
                           };
void setup() {
  lcd.begin(16, 4); 
  while (!SD.begin(SD_CS)) {
      lcd.clear();
      lcd.print("Check SD card");
  }
  word votes[NUM_PRESIDENTIAL_CANDIDATES];
  word blanks[NUM_PRESIDENTIAL_CANDIDATES] = {0};
  word sum_votes = 0;
  for (int i = 0; i < NUM_PRESIDENTIAL_CANDIDATES; i++){
      EEPROM.get(counter_addr+i*sizeof(word),votes[i]);
      sum_votes += votes[i];
  }
  if(sum_votes != 0){
    myFile = SD.open("counter.txt",FILE_WRITE);
    if(myFile){
      for(int i = 0; i < NUM_PRESIDENTIAL_CANDIDATES; i++){
          myFile.print(votes[i]);
          myFile.print(';');
      }
      myFile.println(' ');
      myFile.close();
      lcd.clear();
      lcd.print("Written counter");
      delay(500);
      EEPROM.put(counter_addr,blanks);
    }
    else{
    lcd.clear();
    lcd.print("Can't write counter");
    delay(500);
    }
  }
  else{
    lcd.clear();
    lcd.print("Welcome");
    delay(500);
  }
}
void loop(){
  while (digitalRead(enable_pin)!=HIGH){
    lcd.clear();
    lcd.print ("Wait for enable");
    delay(500);
  }
  lcd.clear();
  lcd.print("Enabled");
  delay(500);
  lcd.clear();
  // Vote for president
  lcd.print("President");
  delay(500);
  while(1){
    lcd.clear();
    lcd.print("Cast Vote:");
    lcd.setCursor(0, 3);
    lcd.print("*-cancel");
    lcd.setCursor(0, 2);
    while( key == NO_KEY ){
      key = customKeypad.getKey();
    }
    if (key){
      lcd.setCursor(0, 2);
      // Then assign the vote to the candidate or action
      switch (key){
        case 'A':
        case 'B':
        case 'C':
        case 'D':
          for (int i = 0; i < NUM_PRESIDENTIAL_CANDIDATES; i++) {
            if (president[i].getCode() == key) {
              president[i].vote();
              lcd.print(president[i].getName().c_str());
              delay(500);
              for (int i = 0; i < NUM_PRESIDENTIAL_CANDIDATES; i++){
                 EEPROM.update(counter_addr+i*sizeof(word),president[i].getVotes());   
                 
              }
            }
          }
          key=NO_KEY;
          break;
        default:
                
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Invalid choice");
          lcd.setCursor(0, 1);
          lcd.print("Please choose");
          lcd.setCursor(0, 2);
          lcd.print("again.");
          key = NO_KEY;
          delay(500);
           continue;
      }
        delay(500);
        lcd.clear();
        lcd.print("Thank you!");
        delay(500);  
        break;   
    }
    
  }

}
