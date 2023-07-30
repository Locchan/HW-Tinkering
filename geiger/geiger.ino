#include <LiquidCrystal_I2C.h>
#include <RingBuf.h>

// Default interval. Governs menu switching (if menus switch automatically)
//  as well as data gathering/calculation for 1 minute measurment interval. For other intervals,
//  the data is gathered/calculated every minute.
#define INTERVAL_DEFAULT 5000
#define INTERVAL_1MIN 60000

// Magic number for J305 Geiger tube
#define CONVERSION_FACTOR 0.0081

// Greek letter mu for uSv/h display
const byte mu[] = {
  B00000,
  B10010,
  B10010,
  B10010,
  B10010,
  B11101,
  B10000,
  B10000
};

// Ring buffers for different time intervals.
//  The first template argument is the type of stored data, the second one is 
//   the max size of the buffer. When buffer hits its end, it re-writes data from the start
//   this way, these buffers always have data for a given period
RingBuf<unsigned short int, int(60 / (INTERVAL_DEFAULT / 1000))> data_buffer_1min;
RingBuf<unsigned short int, 5> data_buffer_5min;
RingBuf<unsigned short int, 15> data_buffer_15min;
RingBuf<unsigned short int, 60> data_buffer_hour;

LiquidCrystal_I2C lcd(0x27,16,4);

// The pin the geiger counter board is connected to.
const byte interruptPin = 2;

unsigned long current_millis = 0;
unsigned long previous_millis_default = 0;
unsigned long previous_millis_minute = 0;

// Total count of detected decays since the start
double decays_total = 0;

float last_decays_default = 0;
unsigned short int decays_since_last_default_interval = 0;

float last_decays_minute = 0;
unsigned short int decays_since_last_minute_interval = 0;

// Radioactivity in different units of measurement for different periods of time.
// Units:
//  # dc - decays detected over the given period
//  # bk - radioactivity in Bekkerels over the given period
//  # uSv - radioactivity in microSivert/h over the given period
//  # mR - radioactivity in milliRoentgen/h over the given period
float radioactivity_dc_1m = 0;
float radioactivity_dc_5m = 0;
float radioactivity_dc_15m = 0;
float radioactivity_dc_1h = 0;

float radioactivity_bk_1m = 0;
float radioactivity_bk_5m = 0;
float radioactivity_bk_15m = 0;
float radioactivity_bk_1h = 0;

float radioactivity_uSv_1m = 0;
float radioactivity_uSv_5m = 0;
float radioactivity_uSv_15m = 0;
float radioactivity_uSv_1h = 0;

float radioactivity_mR_1m = 0;
float radioactivity_mR_5m = 0;
float radioactivity_mR_15m = 0;
float radioactivity_mR_1h = 0;


// Total number of display screens. These are defined below in showScreen function
//  Should be updated according to actual number of screens available
unsigned char screens_total = 4;
unsigned char current_screen = 0;

// Converts millis() from Arduino to HH:MM:SS format
String get_readable_time_since_start(unsigned long mss){
  int mss_in_seconds = mss / 1000;
  char buffer[16];
  sprintf(buffer, "%02d:%02d:%02d", int(mss_in_seconds / 3600), int((mss_in_seconds / 60) % 60), int(mss_in_seconds % 60));
  return String(buffer);
}


// Code to be executed when an interrupt is detected at pin 2.
//  Note that this code is executed twice as the interrupt is set to CHANGE mode
//  It will execute both on LOW->HIGH and HIGH->LOW pin state transitions.
void interrupt() {
  decays_total += 0.5;
}


// Screen slide show. Will iterate over screens from 0 until reaches screens_total, then it starts from 0 for the next iteration
void screenSlideShowNext(){
  showScreen(current_screen);
  current_screen++;
  if(current_screen >= screens_total){
    current_screen = 0;
  }
}

// Shows the screen specified in screen_number variable
void showScreen(short int screen_number){
  unsigned long int current_time_since_start = millis();
  switch(screen_number){
  case 0:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("  = Last minute =");
      lcd.setCursor(0,1);
      lcd.print("bk:");
      lcd.print(radioactivity_bk_1m);
      lcd.print(" mR/h:");
      lcd.print(radioactivity_mR_1m);
      lcd.setCursor(0,2);
      lcd.write(0);
      lcd.print("Sv/h:");
      lcd.print(radioactivity_uSv_1m);
      lcd.print(" #:");
      lcd.print(int(radioactivity_dc_1m));
      lcd.setCursor(0,3);
      lcd.print("Uptime:");
      lcd.print(get_readable_time_since_start(current_time_since_start));
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" = Last 5 minutes =");
      lcd.setCursor(0,1);
      lcd.print("bk:");
      lcd.print(radioactivity_bk_5m);
      lcd.print(" mR/h:");
      lcd.print(radioactivity_mR_5m);
      lcd.setCursor(0,2);
      lcd.write(0);
      lcd.print("Sv/h:");
      lcd.print(radioactivity_uSv_5m);
      lcd.print(" #:");
      lcd.print(int(radioactivity_dc_5m));
      lcd.setCursor(0,3);
      lcd.print("Uptime:");
      lcd.print(get_readable_time_since_start(current_time_since_start));
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("= Last 15 minutes =");
      lcd.setCursor(0,1);
      lcd.print("bk:");
      lcd.print(radioactivity_bk_15m);
      lcd.print(" mR/h:");
      lcd.print(radioactivity_mR_15m);
      lcd.setCursor(0,2);
      lcd.write(0);
      lcd.print("Sv/h:");
      lcd.print(radioactivity_uSv_15m);
      lcd.print(" #:");
      lcd.print(int(radioactivity_dc_15m));
      lcd.setCursor(0,3);
      lcd.print("Uptime:");
      lcd.print(get_readable_time_since_start(current_time_since_start));
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("   = Last hour =");
      lcd.setCursor(0,1);
      lcd.print("bk:");
      lcd.print(radioactivity_bk_1h);
      lcd.print(" mR/h:");
      lcd.print(radioactivity_mR_1h);
      lcd.setCursor(0,2);
      lcd.write(0);
      lcd.print("Sv/h:");
      lcd.print(radioactivity_uSv_1h);
      lcd.print(" #:");
      lcd.print(int(radioactivity_dc_1h));
      lcd.setCursor(0,3);
      lcd.print("Uptime:");
      lcd.print(get_readable_time_since_start(current_time_since_start));
      break;
    default:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("!UNKNOWN SCREEN!");
      break;
  }
}

// Count data for 1 minute and all other intervals
// These two functions are too fucking retarded, but found no other way with templates
// RingBuf does not want to be passed unless you give it SPECIFIC template parameters, so the functions
//  can be only specific for each RingBuf we have, not a generic one for all the buffers. Bullshit.
float count_shit_1m(){
  float result = 0;
  for (int i=0; i<(data_buffer_1min.size() - 1); i++){
    result += data_buffer_1min[i];
  }
  radioactivity_dc_1m = result;
  if (data_buffer_1min.size() < data_buffer_1min.maxSize()){
    result *= (data_buffer_1min.maxSize() / data_buffer_1min.size());
  }
  radioactivity_bk_1m = result / 60;
  radioactivity_uSv_1m = result * CONVERSION_FACTOR;
  radioactivity_mR_1m = (result * CONVERSION_FACTOR) / 10.0;
}

float count_shit_longterm(){

  // 5 minutes
  float cpm = 0;
  float result = 0;
  for (int i=0; i<(data_buffer_5min.size() - 1); i++){
    result += data_buffer_5min[i];
  }
  radioactivity_dc_5m = result;
  if (!data_buffer_5min.isFull()){
    result *= (data_buffer_5min.maxSize() / data_buffer_5min.size());
  }
  radioactivity_bk_5m = result / 300;
  cpm = result / 5;
  radioactivity_uSv_5m = cpm * CONVERSION_FACTOR;
  radioactivity_mR_5m = (cpm * CONVERSION_FACTOR) / 10.0;
  
  // 15 minutes
  result = 0;
  for (int i=0; i<(data_buffer_15min.size() - 1); i++){
    result += data_buffer_15min[i];
  }
  radioactivity_dc_15m = result;
  if (!data_buffer_15min.isFull()){
    result *= (data_buffer_15min.maxSize() / data_buffer_15min.size());
  }
  radioactivity_bk_15m = result / 900;
  cpm = result / 15;
  radioactivity_uSv_15m = cpm * CONVERSION_FACTOR;
  radioactivity_mR_15m = (cpm * CONVERSION_FACTOR) / 10.0;

  // hour
  result = 0;
  for (int i=0; i<(data_buffer_hour.size() - 1); i++){
    result += data_buffer_hour[i];
  }
  radioactivity_dc_1h = result;
  if (!data_buffer_hour.isFull()){
    result *= (data_buffer_hour.maxSize() / data_buffer_hour.size());
  }
  radioactivity_bk_1h = result / 3600;
  cpm = result / 60;
  radioactivity_uSv_1h = cpm * CONVERSION_FACTOR;
  radioactivity_mR_1h = (cpm * CONVERSION_FACTOR) / 10.0;
}

void setup() {
  Serial.begin(115200);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), interrupt, CHANGE);
  lcd.init();
  lcd.createChar(0, mu);
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Measuring...");
}

void loop() {
  current_millis = millis();
  // Code to be executed every INTERVAL_DEFAULT
  if ((current_millis - previous_millis_default) > INTERVAL_DEFAULT) {
    previous_millis_default = current_millis;
    decays_since_last_default_interval = decays_total - last_decays_default;
    last_decays_default = decays_total;
    data_buffer_1min.pushOverwrite(decays_since_last_default_interval);
    count_shit_1m();
    screenSlideShowNext();
  }

  // Code to be executed every minute
  current_millis = millis();
  if ((current_millis - previous_millis_minute) > INTERVAL_1MIN) {
    previous_millis_minute = current_millis;
    decays_since_last_minute_interval = decays_total - last_decays_minute;
    last_decays_minute = decays_total;
    data_buffer_5min.pushOverwrite(decays_since_last_minute_interval);
    data_buffer_15min.pushOverwrite(decays_since_last_minute_interval);
    data_buffer_hour.pushOverwrite(decays_since_last_minute_interval);
    count_shit_longterm();
  }
}