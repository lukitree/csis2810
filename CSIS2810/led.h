#pragma once
#include <Metro.h>
#include <Arduino.h>

class led
{
public:
						led(unsigned int led_pin, unsigned int btn_pin, unsigned int& score);

public:
	void				update();
	void				check_input();

private:
	void				reset_life_time();
	void				reset_wait_time();

	bool				check_hit();

private:
	Metro				life_time;
	Metro				wait_time;
	bool				on;
	bool				pressed;
	unsigned int&		score;

private:
	static const unsigned int	MIN_LIFE = 2000;
	static const unsigned int	MAX_LIFE = 3000;
	static const unsigned int	MIN_WAIT = 1000;
	static const unsigned int	MAX_WAIT = 2000;

private:
	const unsigned int	LED_PIN;
	const unsigned int	BTN_PIN;
};

