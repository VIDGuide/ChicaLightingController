#include <Arduino.h>

class LED {
private:
    int LEDPin;
    bool state = false;
    bool newState = false;
    unsigned long timeToChange = 0;
    bool wantChange = false;
    long onRandomDuration = 0;
    long offRandomDuration = 0;
    int toggleDuration = 0;
    int pauseCycles = 0;
    int bright = 100;
    String name = "unset";
    int cycleCount = 0;
public:

    struct LEDConfig {
        int pin;
        String LEDName = "unset";
        bool initialState = true;
        int brightValue = 255;
        long onRandom = 0;
        long offRandom = 0;
        int cyclesLimit = 0;
        int toggle = 0;
    };

    explicit LED(LED::LEDConfig config) {
        LEDPin = config.pin;
        newState = config.initialState;
        wantChange = true;
        if (config.brightValue < 1 || config.brightValue > 254) { config.brightValue = 255; }
        bright = config.brightValue;
        pinMode(LEDPin, OUTPUT);
        onRandomDuration = config.onRandom;
        offRandomDuration = config.offRandom;
        pauseCycles = config.cyclesLimit;
        toggleDuration = config.toggle;
        name = config.LEDName;
        Refresh();
    }

    String Report() {
        String result = "LED ";
        result += name;
        result += " started. PIN: ";
        result += LEDPin;
        result += ". Group Pointer: ";
        result += cycleCount;
        return result;
    }

    void Toggle(long delayInMS) {
        if (timeToChange > millis()) {
            return;
        }
        wantChange = true;
        newState = !state;
        timeToChange = millis() + delayInMS;
    }

    void WritePin() const {
        if (bright < 255 && state) {
            analogWrite(LEDPin, bright);
        } else {
            digitalWrite(LEDPin, state);
        }
    }

    String Refresh() {
        String result = "[" + name + "] ";
        if (pauseCycles > 0 && cycleCount > pauseCycles && state) {
            result += "Pausing due to cycle count" + cycleCount;
            cycleCount = 0;
            Toggle(1000);
        } else {
            if (toggleDuration > 0) {
                Toggle(toggleDuration);
                result += "Toggle State";
            }
            if (onRandomDuration > 0 && offRandomDuration > 0) {
                Toggle(random(state ? offRandomDuration : onRandomDuration));
                result += "Random State to " + newState ? "on" : "off";
            }
        }
        if (newState != state && timeToChange >= millis() && wantChange) {
            state = newState;
            wantChange = false;
            WritePin();
            cycleCount++;
            return result;
        }
        return "";
    }
};

LED ledArray[] = {
        LED(LED::LEDConfig{
                .pin = 7,
                .LEDName = "Leg, Blue",
                initialState:false,
                .brightValue = 255,
                onRandom:3,
                offRandom:300,
                cyclesLimit:10
        }),

        LED(LED::LEDConfig{
                pin:9,
                LEDName:"Leg, White",
                initialState:false,
                .brightValue = 255,
                onRandom:5,
                offRandom:250,
                cyclesLimit:10
        }),

        LED(LED::LEDConfig{
                pin:10,
                LEDName:"Leg, Red",
                initialState:true,
                brightValue:10}),

        LED(LED::LEDConfig{
                    pin:13,
                    LEDName:"Heartbeat LED",
                    initialState:false,
                    .brightValue = 255,
                    .onRandom = 0,
                    .offRandom = 0,
                    .cyclesLimit = 0,
                    .toggle = 500
            }
        )
};

void setup() {
    Serial.begin(57600);
    Serial.print("Starting. LED Array: ");
    Serial.println(sizeof ledArray / sizeof ledArray[0]);
    for (auto &i: ledArray) { Serial.println(i.Report()); }
}


void loop() {
    for (auto &i: ledArray) {
        String a = i.Refresh();
        if (a != "") {
            Serial.println(a);
        }
    }
}
