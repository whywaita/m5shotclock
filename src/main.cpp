#include <M5Unified.h>

constexpr int DEFAULT_TIME = 60;
constexpr int FONT_SIZE = 3;
constexpr int TIME_X = 160;
constexpr int TIME_Y = 100;
constexpr int BUTTON_HEIGHT = 50;
constexpr int BUTTON_WIDTH = 120;
constexpr int BUTTON_Y = 180;
constexpr int FLASH_INTERVAL = 500;  // milliseconds

struct Button {
    int x;
    int y;
    int w;
    int h;
    const char* label;
    uint16_t color;
};

struct ShotClock {
    int remaining = DEFAULT_TIME;
    bool running = false;
    bool timeUp = false;
    bool flashState = false;
    bool alarmPlayed = false;
    unsigned long lastSecondTick = 0;
    unsigned long lastFlashToggle = 0;

    void reset() {
        remaining = DEFAULT_TIME;
        running = false;
        timeUp = false;
        flashState = false;
        alarmPlayed = false;
        const unsigned long now = millis();
        lastSecondTick = now;
        lastFlashToggle = now;
    }

    void start() {
        if (timeUp) {
            return;
        }
        if (remaining <= 0) {
            remaining = DEFAULT_TIME;
        }
        running = true;
        lastSecondTick = millis();
    }

    void stop() {
        running = false;
    }

    bool update() {
        const unsigned long now = millis();
        bool changed = false;

        if (running && !timeUp) {
            const unsigned long elapsed = now - lastSecondTick;
            if (elapsed >= 1000) {
                const int secondsPassed = elapsed / 1000;
                lastSecondTick += secondsPassed * 1000;

                if (secondsPassed > 0) {
                    remaining = (remaining > secondsPassed) ? remaining - secondsPassed : 0;
                    changed = true;
                }

                if (remaining == 0) {
                    setTimeUp(now);
                    changed = true;
                }
            }
        }

        if (timeUp && (now - lastFlashToggle) >= FLASH_INTERVAL) {
            flashState = !flashState;
            lastFlashToggle = now;
            changed = true;
        }

        return changed;
    }

  private:
    void setTimeUp(unsigned long now) {
        running = false;
        timeUp = true;
        flashState = true;
        lastFlashToggle = now;
        if (!alarmPlayed) {
            M5.Speaker.tone(1000, 1000);
            alarmPlayed = true;
        }
    }
};

Button startButton = {20, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, "START", GREEN};
Button stopButton = {180, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, "STOP", RED};
Button resetButton = {180, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, "RESET", BLUE};

ShotClock shotClock;

void updateDisplay();
void drawButtons();
void drawButton(const Button& btn, const char* label);
void handleTouch(const m5::touch_detail_t& touch);
bool isInButton(const m5::touch_detail_t& touch, const Button& btn);

void setup() {
    auto cfg = M5.config();
    cfg.internal_imu = false;
    cfg.internal_rtc = false;
    cfg.internal_mic = false;
    cfg.internal_spk = true;
    M5.begin(cfg);

    M5.Display.setRotation(1);
    M5.Display.setTextSize(FONT_SIZE);
    M5.Display.setTextDatum(MC_DATUM);

    shotClock.reset();
    updateDisplay();
}

void loop() {
    M5.update();

    auto touch = M5.Touch.getDetail();
    if (touch.isPressed()) {
        handleTouch(touch);
    }

    if (shotClock.update()) {
        updateDisplay();
    }

    delay(10);
}

void handleTouch(const m5::touch_detail_t& touch) {
    if (isInButton(touch, startButton) && !shotClock.running) {
        if (shotClock.timeUp) {
            shotClock.reset();
        }
        shotClock.start();
        updateDisplay();
        return;
    }

    if (shotClock.timeUp) {
        if (isInButton(touch, resetButton)) {
            shotClock.reset();
            updateDisplay();
        }
        return;
    }

    if (isInButton(touch, stopButton) && shotClock.running) {
        shotClock.stop();
        updateDisplay();
    }
}

bool isInButton(const m5::touch_detail_t& touch, const Button& btn) {
    return (touch.x >= btn.x && touch.x <= btn.x + btn.w &&
            touch.y >= btn.y && touch.y <= btn.y + btn.h);
}

void updateDisplay() {
    const bool showFlash = shotClock.timeUp && shotClock.flashState;
    const uint16_t background = showFlash ? RED : BLACK;

    M5.Display.fillScreen(background);
    M5.Display.setTextColor(WHITE, background);
    M5.Display.setTextSize(FONT_SIZE);

    char timeStr[10];
    snprintf(timeStr, sizeof(timeStr), "%02d", shotClock.remaining);
    M5.Display.drawString(timeStr, TIME_X, TIME_Y);

    if (shotClock.timeUp) {
        M5.Display.setTextColor(WHITE, background);
        M5.Display.drawString("TIME UP!", TIME_X, TIME_Y - 30);
    }

    drawButtons();
}

void drawButtons() {
    drawButton(startButton, startButton.label);

    const Button& activeButton = shotClock.timeUp ? resetButton : stopButton;
    const char* label = shotClock.timeUp ? resetButton.label : stopButton.label;
    drawButton(activeButton, label);
}

void drawButton(const Button& btn, const char* label) {
    M5.Display.fillRoundRect(btn.x, btn.y, btn.w, btn.h, 8, btn.color);
    M5.Display.setTextColor(WHITE, btn.color);
    M5.Display.drawString(label, btn.x + btn.w / 2, btn.y + btn.h / 2);
}
