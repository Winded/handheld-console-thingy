#define NUM_BUTTONS 12

// Analog sticks (analog)
const int y1_pin = 0;
const int x1_pin = 1;
const int y2_pin = 2;
const int x2_pin = 3;

// Triggers (analog)
const int tx1_pin = 4;
const int tx2_pin = 5;

// Analog stick buttons (digital)
const int stick1 = 2;
const int stick2 = 3;

// D-Pad buttons (digital)
const int pad1 = 4;
const int pad2 = 5;
const int pad3 = 6;
const int pad4 = 7;

// ABXY buttons (digital)
const int bA = 8;
const int bB = 9;
const int bX = 10;
const int bY = 11;

int buttonMapping[NUM_BUTTONS];

int stickToKeyMapping[4] = {
  215,
  216,
  217,
  218
};

class KeyMapper {
public:
	KeyMapper() {
		mButtonMapping[0] = -1;
		mButtonMapping[1] = -1;
		mButtonMapping[stick1] = 65;
		mButtonMapping[stick2] = 66;
		mButtonMapping[pad1] = 67;
		mButtonMapping[pad2] = 68;
		mButtonMapping[pad3] = 69;
		mButtonMapping[pad4] = 70;
		mButtonMapping[bA] = 71;
		mButtonMapping[bB] = 72;
		mButtonMapping[bX] = 73;
		mButtonMapping[bY] = 74; 
	}
	
	void update() {
	  for(int i = 0; i < NUM_BUTTONS; i++) {
		int key = mButtonMapping[i];
		if(key == -1) continue;
		int pinValue = digitalRead(i);
		if(pinValue == HIGH) {
		  Keyboard.press(key);
		  Serial.write(key);
		}
	  }
	}
	
private:
	int mButtonMapping[NUM_BUTTONS];
};

class AnalogStick {
public:
  AnalogStick() {
    mXPin = 0;
    mYPin = 0;
    mInvertX = false;
    mInvertY = false;
    mDeadZone = 0.16f;
  }
  
  int xPin() const {
    return mXPin;
  }
  void setXPin(int xPin) {
   	mXPin = xPin;
  }
  
  int yPin() const {
    return mYPin;
  }
  void setYPin(int yPin) {
    mYPin = yPin;
  }
  
  bool invertX() const {
  	return mInvertX;
  }
  void setInvertX(bool value) {
  	mInvertX = value;
  }
  
  bool invertY() const {
  	return mInvertY;
  }
  void setInvertY(bool value) {
  	mInvertY = value;
  }
  
  float deadZone() const {
    return mDeadZone;
  }
  void setDeadZone(float value) {
    mDeadZone = value;
  }
  
  void getNormalizedValues(float *xOutput, float *yOutput) {
  	int xAnal = analogRead(mXPin);
  	int yAnal = analogRead(mYPin);
    float x = xAnal / 1023.0f * 2 - 1;
    x = x < mDeadZone && x > -mDeadZone ? 0.0f : x;
    
    float y = yAnal / 1023.0f * 2 - 1;
    y = y < mDeadZone && y > -mDeadZone ? 0.0f : y;
    
    if(mInvertX) 
    	x = -x;
    if(mInvertY)
    	y = -y;
    
    *xOutput = x;
    *yOutput = y;
  }
  
private:
  int mXPin;
  int mYPin;
  
  bool mInvertX;
  bool mInvertY;
  
  float mDeadZone;
};

class MouseStick : public AnalogStick {
public:
  MouseStick() : AnalogStick() {
    mXSpeed = 1.0f;
    mYSpeed = 1.0f;
  }
  
  float xSpeed() const {
    return mXSpeed;
  }
  void setXSpeed(float xSpeed) {
    mXSpeed = xSpeed;
  }
  
  float ySpeed() const {
    return mYSpeed;
  }
  void setYSpeed(float ySpeed) {
    mYSpeed = ySpeed;
  }
  
  void update() {
    float x, y;
    getNormalizedValues(&x, &y);
    x *= mXSpeed;
    y *= mYSpeed;
    if(x != 0 || y != 0) {
      Mouse.move(x, y, 0);
      Serial.print("X: ");
      Serial.print(x);
      Serial.print(", Y: ");
      Serial.print(y);
    }
  }
  
private:
  float mXSpeed;
  float mYSpeed;
  
};

class KeysStick : public AnalogStick {
public:
  KeysStick() : AnalogStick() {
    mRightKey = 215;
    mLeftKey = 216;
    mDownKey = 217;
    mUpKey = 218;
  }
  
  int rightKey() const {
    return mRightKey;
  }
  int leftKey() const {
    return mLeftKey;
  }
  int downKey() const {
    return mDownKey;
  }
  int upKey() const {
    return mUpKey;
  }
  
  void setKeys(int right, int left, int down, int up) {
    mRightKey = right;
    mLeftKey = left;
    mDownKey = down;
    mUpKey = up;
  }
  
  void update() {
    float x, y;
    getNormalizedValues(&x, &y);
    if(x > 0) {
      Serial.write(mRightKey);
      Keyboard.press(mRightKey);
    }
    else if(x < 0) {
      Serial.write(mLeftKey);
      Keyboard.press(mLeftKey);
    }
    if(y > 0) {
      Serial.write(mUpKey);
      Keyboard.press(mUpKey);
    }
    else if(y < 0) {
      Serial.write(mDownKey);
      Keyboard.press(mDownKey);
    }
  }
  
private:
  int mRightKey;
  int mLeftKey;
  int mDownKey;
  int mUpKey;
  
};

void setupPin(const int pinNum) {
	pinMode(pinNum, INPUT);
	digitalWrite(pinNum, HIGH);
}

KeyMapper *keyMapper;
MouseStick *mouseStick;
KeysStick *keysStick;

void setup()
{
  keyMapper = new KeyMapper();
  
  mouseStick = new MouseStick();
  mouseStick->setXPin(x1_pin);
  mouseStick->setYPin(y1_pin);
  mouseStick->setXSpeed(10.0f);
  mouseStick->setYSpeed(10.0f);
  
  keysStick = new KeysStick();
  keysStick->setXPin(x2_pin);
  keysStick->setYPin(y2_pin);
  keysStick->setDeadZone(0.25f);
  
  Serial.begin(9600);
  Keyboard.begin();
  Mouse.begin();
  
  setupPin(stick1);
  setupPin(stick2);
  
  setupPin(pad1);
  setupPin(pad2);
  setupPin(pad3);
  setupPin(pad4);
  
  setupPin(bA);
  setupPin(bB);
  setupPin(bX);
  setupPin(bY);
}

void loop()
{ 
  keyMapper->update();
  mouseStick->update();
  keysStick->update();
  
  Serial.print("\n");
  Keyboard.releaseAll();
  delay (10);
}
