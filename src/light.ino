const byte RED_LED = 8;
const byte YELLOW_LED = 9;
const byte GREEN_LED = 10;
const byte BUTTON_PIN = 2;


constexpr unsigned long GREEN_DURATION = 10000;

// Мінімальний час зеленого.
// Навіть після натискання кнопки зелений не вимкнеться раніше.
constexpr unsigned long MIN_GREEN_DURATION = 5000;

// Тривалість миготіння зеленого
constexpr unsigned long GREEN_BLINK_DURATION = 3000;

// Інтервал миготіння зеленого
constexpr unsigned long GREEN_BLINK_INTERVAL = 400;

// Тривалість жовтого
constexpr unsigned long YELLOW_DURATION = 2000;

// Тривалість червоного
constexpr unsigned long RED_DURATION = 5000;

// Debounce кнопки
constexpr unsigned long BUTTON_DEBOUNCE_DELAY = 50;



enum class TrafficLightState {
  GREEN,
  GREEN_BLINK,
  YELLOW,
  YELLOWRED,
  RED
};

TrafficLightState currentState = TrafficLightState::GREEN;

void initializeTrafficLight() {
    setTrafficLightState(TrafficLightState::GREEN);
}

void setTrafficLightState(TrafficLightState state) {
  switch (state) {
    case TrafficLightState::GREEN:
      digitalWrite(RED_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      break;
    case TrafficLightState::GREEN_BLINK:
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW); // Start with LED off for blinking
        break;
    case TrafficLightState::YELLOW:

        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        break;
    case TrafficLightState::YELLOWRED:
        digitalWrite(RED_LED, HIGH);
        digitalWrite(YELLOW_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        break;
    case TrafficLightState::RED:
        digitalWrite(RED_LED, HIGH);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        break;
  }
}

void blinkLight(byte ledPin, unsigned long duration, unsigned long interval) {
    unsigned long startTime = millis();
    while (millis() - startTime < duration) {
        digitalWrite(ledPin, HIGH);
        delay(interval);
        digitalWrite(ledPin, LOW);
        delay(interval);
    }
}


void setup() {
  Serial.begin(9600);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  initializeTrafficLight();
}

void loop() {
    bool buttonState = digitalRead(BUTTON_PIN);


  if (buttonState == LOW) {
    Serial.println("Button pressed");


    blinkLight(GREEN_LED, GREEN_BLINK_DURATION, GREEN_BLINK_INTERVAL);
    delay(BUTTON_DEBOUNCE_DELAY);

    setTrafficLightState(TrafficLightState::YELLOW);
    delay(YELLOW_DURATION);

    setTrafficLightState(TrafficLightState::RED);
    delay(RED_DURATION);

    setTrafficLightState(TrafficLightState::YELLOWRED);
    delay(YELLOW_DURATION);
    setTrafficLightState(TrafficLightState::GREEN);
    delay(GREEN_DURATION);




  } else {
    Serial.println("Button released");
  }



}