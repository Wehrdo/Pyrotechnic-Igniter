//Author: David A Wehr; July 7, 2012; davidwehr.wordpress.com
//Licensed as (CC BY-NC-SA 3.0)
//http://creativecommons.org/licenses/by-nc-sa/3.0/
//-----------------------------------------------------------------------------\\

//Custom igniter class. Handles lighting each igniter
class Igniter {
  private:
    //Used or set outside the class through the constructor and setters/getters
    byte pin; //Which pin this igniter is connected to
    int threshold; //At what threshold to launch at. Pulse length in microseconds
    int length; //How long to activate pin for. In milliseconds
    
    //Used only within this class
    unsigned long start_time; //The chip's running time for this program. Used
                              //for checking when to turn pin LOW
    boolean launched; //Whether this pin has been launched yet
    boolean check; //Whether to check the clock. Only true when lighting igniter
  
  public:
    Igniter(byte p, int l, int t) {
      pin = p;
      length = l;
      threshold = t;
      launched = false;
      check = false;
      pinMode(pin, OUTPUT);
    };
    ~Igniter() {};
    
    //Use these to change or get attributes of the object. They have all been
    //set values in the constructor
    void setPin(byte p) {pin = p;}
    void setThreshold(int thresh) {threshold = thresh;};
    void setTime(int mils) {length = mils;};
    int getThreshold() {return threshold;};
    
    //Starts the launching process
    void launch() {
      //If it's alreday been launched, exit function
      if (launched == true) {
        return;
      }
      launched = true;
      check = true;
      start_time = millis();
      digitalWrite(pin, HIGH);
    }
    
    void update() {
      //If we don't need to check the time, exit function
      if (check == false) {
        return;
      }
      //Otherwise, turn off the pin when the allocated length has been met
      if (millis() > (start_time + length)) {
        digitalWrite(pin, LOW);
      }
    }
  };


//Main program

const byte SIG_PIN = 2; //Pin the signal is to be read on

const int LIGHT_TIME = 2000; //How long to set the launching pin to HIGH, in ms
const int PIN_COUNT = 4; //Number of items in launchers[]

 //An array of each igniter object. You could set each igniter to stay on for
 //a different amount of time, if desired(for e.g. different types of igniters)
 //The third argument is the minimum pulse length to launch at. Reading from the
 //receiver, the values are usually between 1000-2000.
Igniter launchers[] = {
  Igniter(2, LIGHT_TIME, 1250), 
  Igniter(3, LIGHT_TIME, 1500), 
  Igniter(4, LIGHT_TIME, 1750), 
  Igniter(5, LIGHT_TIME, 1990)
};

void setup() {
  pinMode(SIG_PIN, INPUT);
}

void loop() {
  int pulse_len = pulseIn(SIG_PIN, HIGH);
  //If the pulse is greater than 3 ms, most likely the radio was off
  if (pulse_len < 3000) {
    for (int i  = 0; i < PIN_COUNT; i++) {
      launchers[i].update();
      //Check to see if the pulse length is long enough for an igniter to launch
      if (pulse_len > launchers[i].getThreshold()) {
        launchers[i].launch();
      }
    }
  }
}
