#include <Keypad.h>

const byte ROWS = 5; //four rows
const byte COLS = 5; //four columns

char hexaKeys[ROWS][COLS] = {
  {'m', 'n', 'r', 'a', 'c'},
  {'%', '1', '2', '3', '/'},
  {'_', '4', '5', '6', '*'},
  {'s', '7', '8', '9', '-'},
  {'p', '0', '.', '=', '+'},
};

byte rowPins[ROWS] = {A4, A3, A2, 11, 12}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A0, A1, 10, 9, 8}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#define DISPLAY_CLOCK_PIN 6
#define DISPLAY_LATCH_PIN 5
#define DISPLAY_DATA_PIN 7

boolean decimal_place_on = false;
uint8_t decimal_place = 0;
char display_text[7];
boolean start_new_input = false;


double last_entry = 0;
double running_total = 0;
boolean initial_load = true;

double mrc = 0;
boolean mrc_pressed = false;

char stored_function = ' ';
char last_actual_function = ' ';

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(DISPLAY_CLOCK_PIN, OUTPUT);
  pinMode(DISPLAY_DATA_PIN, OUTPUT);
  pinMode(DISPLAY_LATCH_PIN, OUTPUT);

  digitalWrite(DISPLAY_CLOCK_PIN, LOW);
  digitalWrite(DISPLAY_DATA_PIN, LOW);
  digitalWrite(DISPLAY_LATCH_PIN, LOW);

  display_text[5] = ' ';
  display_text[4] = ' ';
  display_text[3] = ' ';
  display_text[2] = ' ';
  display_text[1] = ' ';
  display_text[0] = ' ';

  display_text[0] = 'C';
  update_LEDs();
  delay(500);

    display_text[1] = 'O';
  update_LEDs();
  delay(500);

    display_text[2] = 'T';
  update_LEDs();
  delay(500);

    display_text[3] = 'T';
  update_LEDs();
  delay(500);

    display_text[4] = 'A';
  update_LEDs();
  delay(500);

    display_text[5] = 'G';
  update_LEDs();
  delay(2000);

  clear_ac();

  customKeypad.addEventListener(keypadEvent);
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();
    if (input != '\n' && input != 0x10) {
      Serial.print("Serial input ");
      Serial.println(input);
      input_handler(input);
    }
  }

  customKeypad.getKey();

}

void keypadEvent(KeypadEvent key) {
  if (customKeypad.getState() == PRESSED) {
    Serial.print("Keypad input ");
    Serial.println(key);
    input_handler(key);
  }
}

void input_handler(char keypress) {

  if (keypress != 'r') {
    mrc_pressed = false;
  }

  switch (keypress) {
    case '0':
      display_input('0');
      break;
    case '1':
      display_input('1');
      break;
    case '2':
      display_input('2');
      break;
    case '3':
      display_input('3');
      break;
    case '4':
      display_input('4');
      break;
    case '5':
      display_input('5');
      break;
    case '6':
      display_input('6');
      break;
    case '7':
      display_input('7');
      break;
    case '8':
      display_input('8');
      break;
    case '9':
      display_input('9');
      break;
    case 's':
      immediate_function('s');
      break;
    case 'p':
      immediate_function('p');
      break;
    case '_':
      immediate_function('_');
      break;
    case '.':
      if (start_new_input) {
        clear_c();
        start_new_input = false;
      }
      if (decimal_place == 0) {
        decimal_place_on = true;
      }
      update_LEDs();
      break;

    case '-':
      std_function(keypress);
      break;
    case '+':
      std_function(keypress);
      break;
    case '*':
      std_function(keypress);
      break;
    case '/':
      std_function(keypress);
      break;

    case 'm':
      update_LEDs();
      mrc += convert_display();
      break;
    case 'n':
      update_LEDs();
      mrc -= convert_display();
      break;
    case 'r':
      if (mrc_pressed == false) {
        update_LED_Buffer(mrc);
        update_LEDs();
        last_entry = convert_display();
        mrc_pressed = true;
      } else {
        mrc = 0;
      }
      break;
    case '=':
      std_function(keypress);
      break;
    case '%':
      perc_func();
      break;
    case 'a':
      clear_ac();
      break;
    case 'c':
      clear_c();
      break;
  }


  Serial.print("Running Total: ");
  Serial.println(running_total);

  Serial.print("Last Entry: ");
  Serial.println(last_entry);

  Serial.print("Display: '");
  Serial.print(display_text);
  Serial.println("' ");

  Serial.print("Decimal Place: ");
  if (decimal_place_on) {
    Serial.print("On ");
  } else {
    Serial.print("off ");
  }
  Serial.println(decimal_place);

  Serial.print("start_new_input: ");
  Serial.println(start_new_input);
  Serial.print("func: ");
  Serial.println(stored_function);
  Serial.println("");
}

void std_function(char input_function) {

  if (input_function != '=' && last_actual_function == '=' )  {

    initial_load = true;
    last_entry = convert_display();
  }

  if (start_new_input == false) {
    last_entry = convert_display();
  }

  switch (stored_function) {
    case '+':      running_total += last_entry;      break;
    case '-':      running_total -= last_entry;      break;
    case '*':      running_total *= last_entry;      break;
    case '/':      running_total /= last_entry;      break;
  }

  if ( initial_load == true) {
    running_total = last_entry;
    initial_load = false;
  }

  update_LED_Buffer(running_total);
  update_LEDs();

  start_new_input = true;

  if (input_function != '=') {
    stored_function = input_function;
  }

  last_actual_function = input_function;
}

void perc_func() {

  if (start_new_input == false)  {

    double temp = convert_display() / 100;

    switch (stored_function) {
      case '+':        running_total += running_total * temp;        break;
      case '-':        running_total -= running_total * temp;        break;
      case '*':        running_total *= running_total * temp;        break;
      case '/':        running_total /= running_total * temp;        break;
    }

    update_LED_Buffer(running_total);
    update_LEDs();

    start_new_input = true;
    stored_function = '=';
  }

}

void update_LED_Buffer(double inputnumber) {

  if (inputnumber >= -99999 && inputnumber <= 999999) {

    //convert to string
    String stringOne = String(inputnumber, 6);

    //copy it into the display
    int i = 0;
    int j = 0;
    while (i < 6) {
      display_text[i] = stringOne[j];
      if (display_text[i] == '.') {
        decimal_place = 6 - i;
      } else {
        i++;
      }
      j++;
    }
    int tempdec = decimal_place;
    //remove trailing zeros
    while (tempdec > 0 && display_text[5] == '0') {
      display_text[5] = display_text[4];
      display_text[4] = display_text[3];
      display_text[3] = display_text[2];
      display_text[2] = display_text[1];
      display_text[1] = display_text[0];
      display_text[0] = ' ';
      decimal_place--;
      tempdec--;
    }

    if (decimal_place == 0) decimal_place_on = false;

    fix_display();

  } else {
    display_e();
  }
}

void display_input(char input) {

  if (start_new_input) {
    clear_c();
    start_new_input = false;
  }

  if ( display_text[0] == ' ' && display_text[5] != 'E') {
    //shift all over by one
    for (int i = 0; i < 5; i++) {
      display_text[i] = display_text[i + 1];
    }
    //load the char

    if (decimal_place_on) decimal_place++;

    display_text[5] = input;
  } else {
    //load an E
    display_e();
  }

  fix_display();
  update_LEDs();
}

void fix_display() {
  //prevent leading zeros
  if ( display_text[0] == ' ' && display_text[1] == ' ' && display_text[2] == ' ' && display_text[3] == ' '  && display_text[4] == '0' && decimal_place_on == false) {
    display_text[4] = ' ';
  }
  //prevent negative leading zeros
  if ( display_text[0] == ' ' && display_text[1] == ' ' && display_text[2] == ' ' &&  display_text[3] == '-' && display_text[4] == '0' && decimal_place_on == false) {
    display_text[3] = ' ';
    display_text[4] = '-';
  }
  //prevent negative zero
  if ( display_text[0] == ' ' && display_text[1] == ' ' && display_text[2] == ' ' &&  display_text[3] == '0' && display_text[4] == '-'  && display_text[5] == '0' && decimal_place_on == false) {
    display_text[4] = ' ';
  }
}

void immediate_function(char input) {
  last_entry = convert_display();
  clear_c();
  stored_function = ' ';
  switch (input) {
    case 's':
      last_entry = sqrt(last_entry);
      break;
    case 'p':
      last_entry = pow(last_entry, 2.0);
      break;
    case '_':
      last_entry = last_entry * -1;
      break;
  }
  update_LED_Buffer(last_entry);
  update_LEDs();
  initial_load = true;
}


