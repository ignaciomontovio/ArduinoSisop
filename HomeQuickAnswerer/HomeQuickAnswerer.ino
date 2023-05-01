#include <LiquidCrystal.h>
#include <Keypad.h>

// ------------------------------------------------
// Constantes
// ------------------------------------------------

LiquidCrystal lcd(12, 11, 14, 15, 16, 17);

const char KEY_1 = '1';
const char KEY_2 = '2';
const char KEY_3 = '3';
const char KEY_4 = '4';
const char KEY_5 = '5';
const char KEY_6 = '6';
const char KEY_NUMBER = '#';
const char KEY_ASTERISK = '*';

const char WELCOME_MESSAGE_ABOVE[16] = "    Bienvenido";
const char WELCOME_MESSAGE_BOTTOM[16] = "               ";
const char DONT_DISTURB_MESSAGE_ABOVE[16] = "      NO";
const char DONT_DISTURB_MESSAGE_BOTTOM[16] = "   MOLESTAR";
const char WAITING_ANSWER_MESSAGE_ABOVE[16] = "   ESPERANDO   ";
const char WAITING_ANSWER_MESSAGE_BOTTOM[16] = "   RESPUESTA   ";


const char MESSAGE_1_ABOVE[16] = "    No puedo";
const char MESSAGE_1_BOTTOM[16] = "atenderte ahora";
const char MESSAGE_2_ABOVE[16] = " Estoy saliendo";
const char MESSAGE_2_BOTTOM[16] = "    esperame";
const char MESSAGE_3_ABOVE[16] = "    No estoy";
const char MESSAGE_3_BOTTOM[16] = "   interesado";
const char MESSAGE_4_ABOVE[16] = "No me encuentro";
const char MESSAGE_4_BOTTOM[16] = "en este momento";
const char MESSAGE_5_ABOVE[16] = "  En 5 minutos";
const char MESSAGE_5_BOTTOM[16] = "   te atiendo";
const char MESSAGE_6_ABOVE[16] = "  Llamame al";
const char MESSAGE_6_BOTTOM[16] = "      cel";

const byte ROW = 4;
const byte COLUMN = 4;

// ------------------------------------------------
// TEMPORIZADORES
// ------------------------------------------------
#define MAX_TIME_MILLIS 5000
int flagTimer = 0;
unsigned long actualTime, previousTime;

// ------------------------------------------------
// Pines sensores
// ------------------------------------------------

#define POTENTIOMETER_PIN  A5
#define START_PUSH_BUTTON_PIN 13
#define BUZZER_PIN 10

// ------------------------------------------------
// Teclado matricial
// ------------------------------------------------
byte row_pin[] = {9, 8, 7, 6};
byte column_pin[] = {5, 4, 3, 2};

char keys[ROW][COLUMN] =
        {
                {'1', '2', '3', 'A'},
                {'4', '5', '6', 'B'},
                {'7', '8', '9', 'C'},
                {'*', '0', '#', 'D'}
        };

Keypad keypad4x4 = Keypad(makeKeymap(keys), row_pin, column_pin, ROW, COLUMN);

// ------------------------------------------------
// Estados del embebido
// ------------------------------------------------
enum state_enum {
    DONT_DISTURB_STATE,
    WAITING_FOR_CALLERS_STATE,
    CALLER_OUTSIDE_STATE,
    CALLER_NOTIFIED_STATE
};

// ------------------------------------------------
// Eventos posibles
// ------------------------------------------------
enum event_enum {
    DONT_DISTURB_KEY_EVENT,
    CHANGE_MESSAGE_EVENT,
    PUSH_BUTTON_EVENT,
    TIMEOUT_EVENT,
    RESTART_KEY_EVENT
};

// ------------------------------------------------
// Estructura de event
// ------------------------------------------------
typedef struct event_struct {
    event_enum type;
    char messageAbove[16];
    char messageBottom[16];
} event_t;

// ------------------------------------------------
// Variables globales
// ------------------------------------------------
state_enum current_state;
event_t event;
int buzzer_volume;

char lastTopMessage[16] = "";
char lastBotMessage[16] = "";

// ------------------------------------------------
// Logica de temporizadores
// ------------------------------------------------

boolean verifyTimeout() {
    if (flagTimer == 0) {
        flagTimer = 1;
        actualTime = millis();
        previousTime = actualTime;
    }
    if (actualTime - previousTime > MAX_TIME_MILLIS) {
        event.type = TIMEOUT_EVENT;
        strcpy(event.messageAbove, WELCOME_MESSAGE_ABOVE);
        strcpy(event.messageBottom, WELCOME_MESSAGE_BOTTOM);
        return true;
    }
    actualTime = millis();
    return false;
}
// ------------------------------------------------
// Logica de sensores
// ------------------------------------------------

boolean check_push_button() {
    return digitalRead(START_PUSH_BUTTON_PIN) == HIGH;
}

boolean check_potentiometer_variation() {
    int buzzer_volume_new = analogRead(POTENTIOMETER_PIN);
    return buzzer_volume_new != buzzer_volume;
}

boolean get_key(char key) {
    if (key != NULL) {
        switch (key) {
            case KEY_1:
                event.type = CHANGE_MESSAGE_EVENT;
                strcpy(event.messageAbove, MESSAGE_1_ABOVE);
                strcpy(event.messageBottom, MESSAGE_1_BOTTOM);
                break;
            case KEY_2:
                event.type = CHANGE_MESSAGE_EVENT;
                strcpy(event.messageAbove, MESSAGE_2_ABOVE);
                strcpy(event.messageBottom, MESSAGE_2_BOTTOM);
                break;
            case KEY_3:
                event.type = CHANGE_MESSAGE_EVENT;
                strcpy(event.messageAbove, MESSAGE_3_ABOVE);
                strcpy(event.messageBottom, MESSAGE_3_BOTTOM);
                break;
            case KEY_4:
                event.type = CHANGE_MESSAGE_EVENT;
                strcpy(event.messageAbove, MESSAGE_4_ABOVE);
                strcpy(event.messageBottom, MESSAGE_4_BOTTOM);
                break;
            case KEY_5:
                event.type = CHANGE_MESSAGE_EVENT;
                strcpy(event.messageAbove, MESSAGE_5_ABOVE);
                strcpy(event.messageBottom, MESSAGE_5_BOTTOM);
                break;
            case KEY_6:
                event.type = CHANGE_MESSAGE_EVENT;
                strcpy(event.messageAbove, MESSAGE_6_ABOVE);
                strcpy(event.messageBottom, MESSAGE_6_BOTTOM);
                break;
            default:
                Serial.println("Key not valid");
        }
    }
}

// ------------------------------------------------
// Logica de actuadores
// ------------------------------------------------

void change_buzzer_volume() {
    buzzer_volume = analogRead(POTENTIOMETER_PIN);
}

void to_ring_buzzer() {
    tone(BUZZER_PIN, buzzer_volume, 1000);
}

void change_message() {
    if (strcmp(event.messageAbove, lastTopMessage) != 0 && strcmp(event.messageBottom, lastBotMessage) != 0) {
        strcpy(lastTopMessage, event.messageAbove);
        strcpy(lastBotMessage, event.messageBottom);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(event.messageAbove);
        lcd.setCursor(0, 1);
        lcd.print(event.messageBottom);
    }
}

// ------------------------------------------------
// Captura de events
// ------------------------------------------------

void get_event() {
    char key = keypad4x4.getKey();

    switch (current_state) {
        case DONT_DISTURB_STATE:
            flagTimer = 0;
            if (key == KEY_NUMBER) {
                event.type = RESTART_KEY_EVENT;
                strcpy(event.messageAbove, WELCOME_MESSAGE_ABOVE);
                strcpy(event.messageBottom, WELCOME_MESSAGE_BOTTOM);
            }
            break;
        case WAITING_FOR_CALLERS_STATE:
            flagTimer = 0;
            if (check_potentiometer_variation()) {
                change_buzzer_volume();
            }
            if (key == KEY_ASTERISK) {
                event.type = DONT_DISTURB_KEY_EVENT;
                strcpy(event.messageAbove, DONT_DISTURB_MESSAGE_ABOVE);
                strcpy(event.messageBottom, DONT_DISTURB_MESSAGE_BOTTOM);
                break;
            }
            if (check_push_button()) {
                event.type = PUSH_BUTTON_EVENT;
                strcpy(event.messageAbove, WAITING_ANSWER_MESSAGE_ABOVE);
                strcpy(event.messageBottom, WAITING_ANSWER_MESSAGE_BOTTOM);
                break;
            }
            get_key(key);
            break;
        case CALLER_OUTSIDE_STATE:
            if (verifyTimeout() == true) {
                break;
            }
            if (key == KEY_ASTERISK) {
                event.type = DONT_DISTURB_KEY_EVENT;
                strcpy(event.messageAbove, DONT_DISTURB_MESSAGE_ABOVE);
                strcpy(event.messageBottom, DONT_DISTURB_MESSAGE_BOTTOM);
                break;
            }
            get_key(key);
            break;
        case CALLER_NOTIFIED_STATE:
            if (verifyTimeout() == true) {
                break;
            }
            if (key == KEY_NUMBER) {
                event.type = RESTART_KEY_EVENT;
                strcpy(event.messageAbove, WELCOME_MESSAGE_ABOVE);
                strcpy(event.messageBottom, WELCOME_MESSAGE_BOTTOM);
                break;
            }
            get_key(key);
            break;
    }
}

// ------------------------------------------------
// Inicialización
// ------------------------------------------------
void start() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(START_PUSH_BUTTON_PIN, INPUT);
    pinMode(POTENTIOMETER_PIN, INPUT);
    buzzer_volume = analogRead(POTENTIOMETER_PIN);
    current_state = WAITING_FOR_CALLERS_STATE;
    event.type = CHANGE_MESSAGE_EVENT;
    strcpy(event.messageAbove, WELCOME_MESSAGE_ABOVE);
    strcpy(event.messageBottom, WELCOME_MESSAGE_BOTTOM);
}

// ------------------------------------------------
// Implementación maquina de estados
// ------------------------------------------------
void fsm() {
    get_event();
    switch (current_state) {
        case DONT_DISTURB_STATE:
            switch (event.type) {
                case RESTART_KEY_EVENT:
                    change_message();
                    current_state = WAITING_FOR_CALLERS_STATE;
                    break;
                default:
                    break;
            }
            break;
        case WAITING_FOR_CALLERS_STATE:
            switch (event.type) {
                case DONT_DISTURB_KEY_EVENT:
                    change_message();
                    current_state = DONT_DISTURB_STATE;
                    break;
                case CHANGE_MESSAGE_EVENT:
                    change_message();
                    current_state = CALLER_NOTIFIED_STATE;
                    break;
                case PUSH_BUTTON_EVENT:
                    change_message();
                    to_ring_buzzer();
                    current_state = CALLER_OUTSIDE_STATE;
                    break;
                default:
                    break;
            }
            break;
        case CALLER_OUTSIDE_STATE:
            switch (event.type) {
                case DONT_DISTURB_KEY_EVENT:
                    change_message();
                    current_state = DONT_DISTURB_STATE;
                    break;
                case CHANGE_MESSAGE_EVENT:
                    change_message();
                    current_state = CALLER_NOTIFIED_STATE;
                    break;
                case TIMEOUT_EVENT:
                    change_message();
                    current_state = WAITING_FOR_CALLERS_STATE;
                    break;
                default:
                    break;
            }
            break;
        case CALLER_NOTIFIED_STATE:
            switch (event.type) {
                case CHANGE_MESSAGE_EVENT:
                    change_message();
                    current_state = CALLER_NOTIFIED_STATE;
                    break;
                case RESTART_KEY_EVENT:
                    change_message();
                    current_state = WAITING_FOR_CALLERS_STATE;
                    break;
                case TIMEOUT_EVENT:
                    change_message();
                    current_state = WAITING_FOR_CALLERS_STATE;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

// ------------------------------------------------
// Arduino setup
// ------------------------------------------------
void setup() {
    start();
}

// ------------------------------------------------
// Arduino loop
// ------------------------------------------------
void loop() {
    fsm();
}
