#include "led.h"



led::led(unsigned int led_pin, unsigned int btn_pin, unsigned int& score)
	: LED_PIN(led_pin)
	, BTN_PIN(btn_pin)
	, pressed(false)
	, on(false)
	, score(score)
{
	reset_life_time();
	reset_wait_time();

	pinMode(LED_PIN, OUTPUT);
	pinMode(BTN_PIN, INPUT);
}

void led::update()
{
	if (wait_time.check() == 1 && on == false)
	{
		on = true;
		reset_life_time();
	}

	if (life_time.check() == 1 && on == true)
	{
		on = false;
		reset_wait_time();
	}

	if (check_hit() == true) score++;
	
	if (on == true) digitalWrite(LED_PIN, HIGH);
	else digitalWrite(LED_PIN, LOW);
}

void led::check_input()
{
	if (digitalRead(BTN_PIN)) pressed = true;
	else pressed = false;
}

bool led::check_hit()
{
	if (on  == true && pressed == true)
	{
		on = false;
		pressed = false;
		reset_wait_time();
		return true;
	}

	return false;
}

void led::reset_life_time()
{
	life_time = MIN_LIFE + (random() % (MAX_LIFE - MIN_LIFE));
}

void led::reset_wait_time()
{
	wait_time = MIN_WAIT + (random() % (MAX_WAIT - MIN_WAIT));
}
