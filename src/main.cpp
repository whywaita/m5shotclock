#include <M5Unified.h>
#include <Preferences.h>

// Global variables
Preferences preferences;
const int DEFAULT_TIME = 60;  // Constant default time
int defaultTime = DEFAULT_TIME;
int currentTime = defaultTime;
bool isRunning = false;
bool isTimeUp = false;
bool flashState = false;  // For background flashing effect
unsigned long lastFlashTime = 0;  // For controlling flash timing

// Screen layout constants
const int FONT_SIZE = 3;
const int TIME_X = 160;  // Center X for landscape mode (320/2)
const int TIME_Y = 100;  // Slightly above center for better layout with buttons
const int BUTTON_HEIGHT = 50;
const int BUTTON_WIDTH = 120;  // Increased width since we only have 2 buttons now
const int BUTTON_Y = 180;     // Moved up slightly for better layout
const int FLASH_INTERVAL = 500;  // Flash interval in milliseconds

// Button position definitions
struct Button {
    int x;
    int y;
    int w;
    int h;
    const char* label;
    uint16_t color;
};

// Adjusted button positions for landscape mode with proper spacing
Button startButton = {20, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, "START", GREEN};
Button stopButton = {180, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, "STOP", RED};
Button resetButton = {180, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, "RESET", BLUE};  // Same position as STOP button

// Forward declarations
void updateDisplay();
void drawButtons();
void handleTouch(const m5::touch_detail_t& touch);
bool isInButton(const m5::touch_detail_t& touch, const Button& btn);
void resetTimer();

void setup() {
    // Initialize M5Stack
    auto cfg = M5.config();
    cfg.internal_imu = false;  // Disable IMU as it's not needed
    cfg.internal_rtc = false;  // Disable RTC as it's not needed
    cfg.internal_mic = false;  // Disable microphone as it's not needed
    cfg.internal_spk = true;   // Enable speaker for buzzer
    M5.begin(cfg);

    // Initialize display
    M5.Display.setRotation(1);  // Set rotation so buttons are at the bottom
    M5.Display.setTextSize(FONT_SIZE);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setTextDatum(MC_DATUM);
    
    // Clear and initialize preferences
    preferences.begin("shotclock", false);
    preferences.clear();  // Clear any stored preferences
    defaultTime = DEFAULT_TIME;
    currentTime = defaultTime;
    
    // Initialize display
    updateDisplay();
    drawButtons();
}

void loop() {
    M5.update();
    
    auto t = M5.Touch.getDetail();
    if (t.isPressed()) {
        handleTouch(t);
    }
    
    if (isRunning && !isTimeUp) {
        currentTime--;
        if (currentTime <= 0) {
            currentTime = 0;
            isTimeUp = true;
            isRunning = false;
            M5.Speaker.tone(1000, 1000);  // Time up sound
            lastFlashTime = millis();  // Initialize flash timing
            flashState = true;  // Start with red background
        }
        updateDisplay();
    }
    
    // Handle background flashing when time is up
    if (isTimeUp) {
        unsigned long currentTime = millis();
        if (currentTime - lastFlashTime >= FLASH_INTERVAL) {
            flashState = !flashState;  // Toggle flash state
            lastFlashTime = currentTime;
            updateDisplay();
        }
    }
    
    delay(100);  // Screen update interval
}

void resetTimer() {
    currentTime = defaultTime;
    isTimeUp = false;
    isRunning = false;
    flashState = false;
    updateDisplay();
}

void handleTouch(const m5::touch_detail_t& touch) {
    // Start button
    if (isInButton(touch, startButton) && !isRunning) {
        isRunning = true;
        isTimeUp = false;
        if (currentTime == 0) {
            currentTime = defaultTime;  // Reset time if it was at 0
        }
        updateDisplay();  // Immediately update display to clear flashing
    }
    
    // Stop/Reset button
    if (isTimeUp) {
        // Act as Reset button when time is up
        if (isInButton(touch, resetButton)) {
            resetTimer();
        }
    } else {
        // Act as Stop button during normal operation
        if (isInButton(touch, stopButton) && isRunning) {
            isRunning = false;
        }
    }
}

bool isInButton(const m5::touch_detail_t& touch, const Button& btn) {
    return (touch.x >= btn.x && touch.x <= btn.x + btn.w &&
            touch.y >= btn.y && touch.y <= btn.y + btn.h);
}

void updateDisplay() {
    // Set background color based on time up state and flash state
    if (isTimeUp && flashState) {
        M5.Display.fillScreen(RED);
        M5.Display.setTextColor(WHITE, RED);
    } else {
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(WHITE, BLACK);
    }
    
    M5.Display.setTextSize(FONT_SIZE);
    
    // Time display
    char timeStr[10];
    sprintf(timeStr, "%02d", currentTime);
    M5.Display.drawString(timeStr, TIME_X, TIME_Y);
    
    // Time up display
    if (isTimeUp) {
        M5.Display.setTextColor(WHITE, flashState ? RED : BLACK);
        M5.Display.drawString("TIME UP!", TIME_X, TIME_Y - 30);
    }
    
    drawButtons();
}

void drawButtons() {
    // Start button
    M5.Display.fillRoundRect(startButton.x, startButton.y, 
                        startButton.w, startButton.h, 8, startButton.color);
    M5.Display.setTextColor(WHITE, startButton.color);
    M5.Display.drawString(startButton.label, 
                      startButton.x + startButton.w/2,
                      startButton.y + startButton.h/2);
    
    // Stop/Reset button (changes based on state)
    if (isTimeUp) {
        // Draw as Reset button
        M5.Display.fillRoundRect(resetButton.x, resetButton.y,
                            resetButton.w, resetButton.h, 8, resetButton.color);
        M5.Display.setTextColor(WHITE, resetButton.color);
        M5.Display.drawString(resetButton.label,
                          resetButton.x + resetButton.w/2,
                          resetButton.y + resetButton.h/2);
    } else {
        // Draw as Stop button
        M5.Display.fillRoundRect(stopButton.x, stopButton.y,
                            stopButton.w, stopButton.h, 8, stopButton.color);
        M5.Display.setTextColor(WHITE, stopButton.color);
        M5.Display.drawString(stopButton.label,
                          stopButton.x + stopButton.w/2,
                          stopButton.y + stopButton.h/2);
    }
}