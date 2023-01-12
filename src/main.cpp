#include <Arduino.h>

#include "config.h"

TTGOClass *ttgo;
TFT_eSPI *tft;
TFT_eSprite *sprite;

int pageIndex = 0;

void setup()
{
	Serial.begin(115200);
	setCpuFrequencyMhz(240);

	ttgo = TTGOClass::getWatch();
	ttgo->begin();

	ttgo->power->adc1Enable(AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);

	tft = ttgo->tft;
	tft->setRotation(1);
	tft->setSwapBytes(true);
	tft->fillScreen(TFT_BLACK);

	sprite = new TFT_eSprite(tft);
	sprite->setSwapBytes(true);
	sprite->createSprite(240, 240);

	ttgo->openBL();

	setCpuFrequencyMhz(20);
}

int getHourNow()
{
	return ttgo->rtc->getDateTime().hour;
}

int getMinuteNow()
{
	return ttgo->rtc->getDateTime().minute;
}

int getSecondNow()
{
	return ttgo->rtc->getDateTime().second;
}

void powerLevel()
{
	int powerLevel = ttgo->power->getBattPercentage();

	if (powerLevel >= 50)
	{
		sprite->drawLine(0, 0, 0, powerLevel * 2.4, TFT_DARKGREEN);
	}
	else if (powerLevel >= 20)
	{
		sprite->drawLine(0, 0, 0, powerLevel * 2.4, TFT_ORANGE);
	}
	else
	{
		sprite->drawLine(0, 0, 0, powerLevel * 2.4, TFT_RED);
	}
}

void powerCharging()
{
	if (ttgo->power->isChargeing())
	{
		sprite->drawCircle(10, 10, 5, TFT_DARKGREEN);
	}
}

void hourMark()
{
	sprite->drawTriangle(239, 110, 239, 130, 220, 120, TFT_DARKCYAN);
	sprite->drawCircle(120, 120, 5, TFT_DARKCYAN);

	for (int i = 1; i <= 11; i++)
	{
		float angle = 0.523599; // 30° in radian
		sprite->drawLine(100 * cos(angle * i) + 120, 100 * sin(angle * i) + 120, 120 * cos(angle * i) + 120, 120 * sin(angle * i) + 120, TFT_DARKCYAN);
	}
}

void secondHand(int second)
{
	float angle = 0.10472; // 6° in radian
	sprite->drawLine(120, 120, 118 * cos(angle * second) + 120, 118 * sin(angle * second) + 120, TFT_DARKGREY);
}

void minuteHand(int minute)
{
	float angle = 0.10472; // 6° in radian
	sprite->drawLine(120, 120, 118 * cos(angle * minute) + 120, 118 * sin(angle * minute) + 120, TFT_CYAN);
}

void hourHand(int hour, int minute)
{
	float angleHour = 0.523599;		// 30° in radian
	float angleMinute = 0.00872665; // 0.5° in radian
	sprite->drawLine(120, 120, 70 * cos(angleHour * hour + angleMinute * minute) + 120, 70 * sin(angleHour * hour + angleMinute * minute) + 120, TFT_GOLD);
}

void menuOpener()
{
	int16_t x, y;
	ttgo->getTouch(x, y);

	if (x >= 200 and x <= 240 and y >= 80 and y <= 140)
	{
		x, y = 0;
		pageIndex = 1;
	}
}

void pageSetting()
{
	int blLevel = ttgo->bl->getLevel();

	sprite->fillScreen(TFT_BLACK);
	sprite->drawTriangle(0, 110, 0, 130, 20, 120, TFT_DARKGREY);
	sprite->drawTriangle(140, 160, 140, 80, 200, 120, TFT_WHITE);
	sprite->drawTriangle(100, 160, 100, 80, 40, 120, TFT_WHITE);
	sprite->pushSprite(0, 0);

	int16_t x, y;
	ttgo->getTouch(x, y);

	if (x >= 140 and x <= 200 and y >= 80 and y <= 160)
	{
		x, y = 0;
		if (blLevel + 10 > 255)
		{
			ttgo->bl->adjust(255);
		}
		else
		{
			ttgo->bl->adjust(blLevel + 10);
		}
	}
	if (x >= 40 and x <= 100 and y >= 80 and y <= 160)
	{
		x, y = 0;
		if (blLevel - 10 < 1)
		{
			ttgo->bl->adjust(1);
		}
		else
		{
			ttgo->bl->adjust(blLevel - 10);
		}
	}
	if (x >= 0 and x <= 40 and y >= 80 and y <= 140)
	{
		pageIndex = 0;
	}
}

void pageClock()
{
	sprite->fillScreen(TFT_BLACK);
	powerLevel();
	powerCharging();
	hourMark();
	secondHand(getSecondNow());
	minuteHand(getMinuteNow());
	hourHand(getHourNow(), getMinuteNow());
	sprite->pushSprite(0, 0);
}

void loop()
{
	menuOpener();
	if (pageIndex == 0)
	{
		pageClock();
	}
	else if (pageIndex == 1)
	{
		pageSetting();
	}
}