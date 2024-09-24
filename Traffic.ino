
#define LED_PIN_GREEN 13
#define LED_PIN_RED 12
#define LED_PIN_YELLOW 11

#define SET_LIGHTS 'l'

#define SET_LED_GREEN_ON 'g'
#define SET_LED_RED_ON 'r'
#define SET_LED_YELLOW_ON 'y'

#define SET_LED_GREEN_OFF 'G'
#define SET_LED_RED_OFF 'R'
#define SET_LED_YELLOW_OFF 'Y'




#define STATE_IDLE 0
#define STATE_LED_ON 1
#define STATE_LED_OFF 2
#define STATE_LED_BLINK 3

#define BLINK_INTERVAL 500
#define BLINK_DURATION 3000
#define YELLOW_DURATION 3000
#define LIGHTS_DURATION 7000

bool is_now_lights;
int stage;
int state;

int current_led_pin;
int previous_led_pin;

long int blink_start;
long int light_start;

bool led_status = LOW;

void update_state() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        
        if (cmd == SET_LIGHTS) {
            digitalWrite(current_led_pin, LOW);
            is_now_lights = true;
            stage = 1;
            current_led_pin = LED_PIN_GREEN;
            previous_led_pin = LED_PIN_RED;
        }
        else if (cmd == SET_LED_GREEN_ON) {
            state = STATE_LED_ON;
            digitalWrite(current_led_pin, LOW);
            current_led_pin = LED_PIN_GREEN;
            is_now_lights = false;
        }
        else if (cmd == SET_LED_GREEN_OFF) {
            state = STATE_LED_OFF;
            digitalWrite(current_led_pin, LOW);
            current_led_pin = LED_PIN_GREEN;
            is_now_lights = false;
        }
        else {
            Serial.println("Unknown command");
        }
    }
}

void traffic_light_stages() {
  if (is_now_lights == true) {
      if (stage == 1) {
          current_led_pin = previous_led_pin == LED_PIN_GREEN ? LED_PIN_RED : LED_PIN_GREEN;
          state = STATE_LED_ON;
          light_start = millis();
          stage += 1;
      }
      else if (stage == 2 and millis() >= light_start + LIGHTS_DURATION) {
          state = STATE_LED_BLINK;
          stage += 1;
          blink_start = millis();
      }
      else if (stage == 3 and millis() >= light_start + LIGHTS_DURATION + BLINK_DURATION) {
          state = STATE_LED_OFF;
          stage += 1;
      }
      else if (stage == 4) {
          light_start = millis();
          current_led_pin = LED_PIN_YELLOW;
          state = STATE_LED_ON;
          stage += 1;
      }
      else if (stage == 5 and millis() >= light_start + YELLOW_DURATION) {
          light_start = millis();
          state = STATE_LED_OFF;
          previous_led_pin = previous_led_pin == LED_PIN_GREEN ? LED_PIN_RED : LED_PIN_GREEN;
          stage = 1;
      }
  }
}

void process_state() {
    if (state == STATE_LED_ON) {
        digitalWrite(current_led_pin, HIGH);
    }
    else if (state == STATE_LED_OFF) {
        digitalWrite(current_led_pin, LOW);
    }
    else if (state == STATE_LED_BLINK) {
        if (millis() <= blink_start + BLINK_INTERVAL and led_status == LOW) {
            digitalWrite(current_led_pin, HIGH);
            led_status = HIGH;
        }
        if (millis() >= blink_start + BLINK_INTERVAL and millis() <= blink_start + 2 * BLINK_INTERVAL and led_status == HIGH) {
            digitalWrite(current_led_pin, LOW);
            led_status = LOW;
        }
        if (millis() >= blink_start + 2 * BLINK_INTERVAL) {
            blink_start = millis();
        }
    }
}


void setup() {
    Serial.begin(9600);

    pinMode(LED_PIN_GREEN, OUTPUT);
    pinMode(LED_PIN_RED, OUTPUT);
    pinMode(LED_PIN_YELLOW, OUTPUT);
}

void loop() {
    update_state();
    traffic_light_stages();
    process_state();
}