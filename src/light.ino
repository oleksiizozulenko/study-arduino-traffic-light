// --- ПІНИ ---
// Світлофор для авто
const byte CAR_RED_PIN    = 8;
const byte CAR_YELLOW_PIN = 9;
const byte CAR_GREEN_PIN  = 10;

// Світлофор для пішоходів
const byte PED_RED_PIN    = 6;
const byte PED_GREEN_PIN  = 7;

// Кнопка та Зумер
const byte BUTTON_PIN     = 2;
const byte BUZZER_PIN     = 5;

// --- ТАЙМІНГИ (у мілісекундах) ---
const unsigned long CAR_GREEN_TIME  = 5000; // Мінімальний зелений для авто
const unsigned long CAR_YELLOW_TIME = 2000; // Жовтий для авто
const unsigned long PED_GREEN_TIME  = 5000; // Зелений для пішоходів
const unsigned long DEBOUNCE_DELAY  = 50;   // Затримка антидребезгу кнопки

// --- СТАНІ СВІТЛОФОРА (State Machine) ---
enum TrafficState {
  STATE_CAR_GREEN,
  STATE_CAR_YELLOW,
  STATE_PED_GREEN,
  STATE_CAR_RED_YELLOW
};

TrafficState currentState = STATE_CAR_GREEN;

// --- ЗМІННІ ДЛЯ ЗБЕРЕЖЕННЯ ЧАСУ ТА СТАНУ ---
unsigned long stateStartTime = 0;

// Змінні кнопки та антидребезгу
bool buttonPressedRequest = false;
bool lastRawButtonState   = HIGH;
bool buttonState          = HIGH;
unsigned long lastDebounceTime = 0;

void setup() {
  Serial.begin(9600);

  // Налаштування пінів авто-світлофора
  pinMode(CAR_RED_PIN, OUTPUT);
  pinMode(CAR_YELLOW_PIN, OUTPUT);
  pinMode(CAR_GREEN_PIN, OUTPUT);

  // Налаштування пінів пішохідного світлофора
  pinMode(PED_RED_PIN, OUTPUT);
  pinMode(PED_GREEN_PIN, OUTPUT);

  // Налаштування зумера та кнопки
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Початковий стан світлодіодів
  setTrafficLights(LOW, LOW, HIGH, HIGH, LOW); // Зелений авто, червоний пішоходам
  stateStartTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. ЧИТАННЯ КНОПКИ З DEBOUNCE
  handleButtonDebounce(currentMillis);

  // 2. STATE MACHINE (Скінченний автомат)
  switch (currentState) {

    case STATE_CAR_GREEN:
      // Переходимо далі, тільки якщо минув час GREEN і була натиснута кнопка
      if (currentMillis - stateStartTime >= CAR_GREEN_TIME) {
        if (buttonPressedRequest) {
          buttonPressedRequest = false; // Скидаємо прапорець запиту
          currentState = STATE_CAR_YELLOW;
          stateStartTime = currentMillis;
          setTrafficLights(LOW, HIGH, LOW, HIGH, LOW); // Жовтий авто, червоний пішоходам
          Serial.println("State change: CAR_YELLOW");
        }
      }
      break;

    case STATE_CAR_YELLOW:
      if (currentMillis - stateStartTime >= CAR_YELLOW_TIME) {
        currentState = STATE_PED_GREEN;
        stateStartTime = currentMillis;
        setTrafficLights(HIGH, LOW, LOW, LOW, HIGH); // Червоний авто, зелений пішоходам
        Serial.println("State change: PED_GREEN");
      }
      break;

    case STATE_PED_GREEN:
      // Звуковий сигнал під час руху пішоходів (переривчастий писк)
      if ((currentMillis / 300) % 2 == 0) {
        tone(BUZZER_PIN, 1000); // 1000 Гц
      } else {
        noTone(BUZZER_PIN);
      }

      if (currentMillis - stateStartTime >= PED_GREEN_TIME) {
        noTone(BUZZER_PIN); // Вимикаємо звук
        currentState = STATE_CAR_RED_YELLOW;
        stateStartTime = currentMillis;
        setTrafficLights(HIGH, HIGH, LOW, HIGH, LOW); // Червоний+Жовтий авто, червоний пішоходам
        Serial.println("State change: CAR_RED_YELLOW");
      }
      break;

    case STATE_CAR_RED_YELLOW:
      if (currentMillis - stateStartTime >= CAR_YELLOW_TIME) {
        currentState = STATE_CAR_GREEN;
        stateStartTime = currentMillis;
        setTrafficLights(LOW, LOW, HIGH, HIGH, LOW); // Повертаємо зелений для авто
        Serial.println("State change: CAR_GREEN");
      }
      break;
  }
}

// --- ДОПОМІЖНІ ФУНКЦІЇ ---

// Обробка антидребезгу та крайніх випадків (edge cases)
void handleButtonDebounce(unsigned long currentMillis) {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastRawButtonState) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;

      // Фіксуємо натискання (LOW через INPUT_PULLUP)
      if (buttonState == LOW) {
        // Крайовий випадок: реєструємо запит, навіть якщо авто ще тільки почали їхати
        buttonPressedRequest = true;
        Serial.println("Pedestrian button pressed (Request saved)");
      }
    }
  }

  lastRawButtonState = reading;
}

// Зручна функція управління всіма світлодіодами
void setTrafficLights(bool carR, bool carY, bool carG, bool pedR, bool pedG) {
  digitalWrite(CAR_RED_PIN, carR);
  digitalWrite(CAR_YELLOW_PIN, carY);
  digitalWrite(CAR_GREEN_PIN, carG);

  digitalWrite(PED_RED_PIN, pedR);
  digitalWrite(PED_GREEN_PIN, pedG);
}