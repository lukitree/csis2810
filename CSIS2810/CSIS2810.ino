#include <Metro.h>

// Shift Register Pins
const int dataPin = 10;
const int latchPin = 11;
const int clockPin = 12;

// Score Board
const int SCOREBOARD_PIN_COUNT = 6;
const int scoreBoardPin[SCOREBOARD_PIN_COUNT] = { 14, 15 ,16 ,17 ,18 ,19 };

// Game Variables
const int PLAY_TIME = 30000;
const int LED_COUNT = 7;
int score = 0;
Metro gameTimer = Metro(PLAY_TIME);
Metro nextPopUp = Metro(1000);

struct LedBtn {
	int btnPin;
	bool wasPressed = false;;
	bool pressed = false;
	bool on = false;
	Metro timer = Metro(1000);
};

struct LedBtn ledBtns[LED_COUNT];

// FSM
enum class State {
	INTRO,
	PLAYING,
	END,
};

State gameState = State::END;

void setup()
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		ledBtns[i].btnPin = (i + 2);
		pinMode(ledBtns[i].btnPin, INPUT_PULLUP);
	}

	for (int i = 0; i < SCOREBOARD_PIN_COUNT; ++i)
	{
		pinMode(scoreBoardPin[i], OUTPUT);
		digitalWrite(scoreBoardPin[i], LOW);
	}

	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
}

void loop()
{
	getInput();
	switch (gameState)
	{
		case State::INTRO:
		{
			intro();
		} break;
		case State::PLAYING:
		{
			update();
		} break;
		case State::END:
		{
			end();
		} break;
	}
	display();
}

void intro()
{
	const int ORDER[6] = { 6, 5, 2, 0, 1, 4, };
	const int DELAY = 50;

	allOn();

	for (int x = 0; x < 2; ++x)
	{
		for (int i = 0; i < 6; ++i)
		{
			ledBtns[ORDER[i]].on = true;
			display();
			delay(DELAY);
			ledBtns[ORDER[i]].on = false;
			display();
		}

		ledBtns[3].on = true;
		display();

		for (int i = 0; i < 6; ++i)
		{
			ledBtns[ORDER[i]].on = true;
			display();
			delay(DELAY);
			ledBtns[ORDER[i]].on = false;
			display();
		}

		ledBtns[3].on = false;
		display();
	}

	// Init Game
	gameState = State::PLAYING;
	gameTimer.reset();
	nextPopUp.reset();
	score = 0;
	allOff();
}

void end()
{
	static Metro blinkTimer = Metro(500);
	static bool toggle = true;

	int t = blinkTimer.check();

	for (int i = 0; i < LED_COUNT; ++i)
	{
		if (t == 1) ledBtns[i].on = toggle;
		if (ledBtns[i].pressed == true) gameState = State::INTRO;
	}

	if (t == 1)
	{
		toggle = !toggle;
		display();
		blinkTimer.reset();
	}
}

void getInput()
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		int pressed = !digitalRead(ledBtns[i].btnPin);
		int wasPressed = ledBtns[i].wasPressed;

		if (pressed == HIGH)
		{
			ledBtns[i].pressed = (wasPressed == false) ? true : false;
			ledBtns[i].wasPressed = true;
		}
		else
		{
			ledBtns[i].wasPressed = false;
			ledBtns[i].pressed = false;
		}
	}
}

void update()
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		if (ledBtns[i].on)
		{
			if (ledBtns[i].timer.check() == 1)
			{
				ledBtns[i].on = false;
			}
			else if (ledBtns[i].pressed)
			{
				++score;
				ledBtns[i].on = false;

				bool ledStates[LED_COUNT];
				for (int i = 0; i < LED_COUNT; ++i)
				{
					ledStates[i] = ledBtns[i].on;
					ledBtns[i].on = true;
				}
				display();
				delay(10);
				for (int i = 0; i < LED_COUNT; ++i)
				{
					ledBtns[i].on = ledStates[i];
				}
				display();
			}
		}
	}

	if (nextPopUp.check() == 1)
	{
		int select = random() % LED_COUNT;
		ledBtns[select].on = true;
		ledBtns[select].timer.interval(200 + (random() % 500));
		ledBtns[select].timer.reset();
		nextPopUp.interval(500 + (random() % 1000));
		nextPopUp.reset();
	}

	if (gameTimer.check() == 1)
	{
		gameState = State::END;
		allOn();
		delay(2000);
		allOff();
	}
}

void display()
{
	// Display Scoreboard
	for (int i = 0; i < SCOREBOARD_PIN_COUNT; ++i)
	{
		if (score & (1 << i))
		{
			digitalWrite(scoreBoardPin[i], HIGH);
		}
		else
		{
			digitalWrite(scoreBoardPin[i], LOW);
		}
	}

	// Display Buttons
	digitalWrite(latchPin, LOW);
	digitalWrite(clockPin, LOW);

	int pinState = 0;
	byte data = 0;

	for (int i = 0; i < LED_COUNT; ++i)
	{
		if (ledBtns[i].on == true)
		{
			data |= 1 << (i + 1);
		}
	}

	for (int i = 7; i >= 0; --i)
	{
		digitalWrite(clockPin, 0);

		if (data & (1 << i))
		{
			pinState = HIGH;
		}
		else
		{
			pinState = LOW;
		}

		digitalWrite(dataPin, pinState);
		digitalWrite(clockPin, HIGH);
		digitalWrite(dataPin, LOW);
	}

	digitalWrite(clockPin, LOW);
	digitalWrite(latchPin, HIGH);

	static int prevScore = score;

	if (score != prevScore)
	{
		Serial.println(score);
		prevScore = score;
	}
}

void allOn()
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		ledBtns[i].on = true;
	}
	display();
}

void allOff()
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		ledBtns[i].on = false;
	}
	display();
}
