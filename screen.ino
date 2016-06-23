double convert_display() {
  double tempnum = atol(display_text);
  int i = 0;
  while (i < decimal_place) {
    tempnum = tempnum / 10;
    i++;
  }
  return tempnum;
}

void clear_ac() {
  display_text[5] = ' ';
  clear_c();
  running_total = 0.0;
  last_entry = 0.0;
  stored_function = ' ';
  last_actual_function = ' ';
  mrc = 0;
  initial_load = true;
}

void clear_c() {
  if ( display_text[5] != 'E') {

    for (int i = 0; i < 7; i++) display_text[i] = ' ';
    display_text[5] = '0';
    display_text[6] = '\0';
    start_new_input = false;
    decimal_place = 0;
    decimal_place_on = false;
    update_LEDs();
  }
}

void display_e() {
  for (int i = 0; i < 5; i++) {
    display_text[i] = ' ';
  }
  display_text[5] = 'E';
  start_new_input = false;
  update_LEDs();
}

void update_LEDs() {
  //simulate the screen flicker of old calculators
  for (int i = 0; i < 6; i++) postNumber(' ', false);
  delay(30);

  //actually output the data
  for (int i = 0; i < 6; i++) {
    //supress decimal when off and in the first place
    postNumber(display_text[5 - i], i == decimal_place && (decimal_place_on || decimal_place > 0));
  }
}


//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

  byte segments;

  switch (number)
  {
    case '1': segments = b | c; break;
    case '2': segments = a | b | d | e | g; break;
    case '3': segments = a | b | c | d | g; break;
    case '4': segments = f | g | b | c; break;
    case '5': segments = a | f | g | c | d; break;
    case '6': segments = a | f | g | e | c | d; break;
    case '7': segments = a | b | c; break;
    case '8': segments = a | b | c | d | e | f | g; break;
    case '9': segments = a | b | c | d | f | g; break;
    case '0': segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'C': segments = a | f | e | d; break;
    case 'E': segments = a | f | e | d | g; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
    case '_': segments = d; break;
    case 'T': segments = f | e | a; break;
    case 'O': segments = a | b | c | d | e | f; break;
    case 'G': segments = a | f | d | e | c; break;
    case 'A': segments = a | f | b | g | e | c; break;
  }

  if (decimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(DISPLAY_CLOCK_PIN, LOW);
    digitalWrite(DISPLAY_DATA_PIN, segments & 1 << (7 - x));
    digitalWrite(DISPLAY_CLOCK_PIN, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
  digitalWrite(DISPLAY_LATCH_PIN, LOW);
  digitalWrite(DISPLAY_LATCH_PIN, HIGH); //Register moves storage register on the rising edge of RCK
}

