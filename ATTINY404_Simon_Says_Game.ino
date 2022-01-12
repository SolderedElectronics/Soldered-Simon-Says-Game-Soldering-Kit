/**
 **************************************************
 *
 * @file        ATTINY404_Simon_Says_Game.ino
 * @brief       Arduino code for Simon Says Game
 *              Soldering Kit from Soldered.
 *
 * @note        In order to use this code, install megaTinyCore from https://github.com/SpenceKonde/megaTinyCore
 *              Core version: 2.4.2
 *              Chip: ATtiny404
 *              Clock: 1MHz Internal
 *              millis()/micros Timer: Enabled (default timer)
 *
 * @authors     Borna Biro for soldered.com
 ***************************************************/

#define LED1_RED    PIN_PA4
#define LED2_GREEN  PIN_PA5
#define LED3_BLUE   PIN_PA6
#define LED4_YELLOW PIN_PA7

#define DELAY_MS        250
#define MAX_STEPS       100
#define SCORE_BLINK_ON  200
#define SCORE_BLINK_OFF 200

const int pinList[] = {LED1_RED, LED2_GREEN, LED3_BLUE, LED4_YELLOW};
uint8_t keys[MAX_STEPS];
uint8_t delayTimes[4] = {255, 127, 63, 31};
uint8_t steps = 0;

void setup()
{
    pinsAsOutputs();
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(pinList[i], HIGH);
    }

    showStartAnimation();
}

void loop()
{
    while (1)
    {
        if (steps < MAX_STEPS)
        {
            showSequence(keys, steps);
            if (getKeys())
            {
                delay(500);
                steps++;
            }
            else
            {
                delay(500);
                showFailAnimation();
                delay(500);
                showResult(steps);
                delay(500);
                steps = 0;
                showStartAnimation();
            }
        }
        else
        {
            while (1)
            {
                steps = 0;
                showStartAnimation();
            }
        }
    };
}


void pinsAsOutputs()
{
    for (int i = 0; i < 4; i++)
    {
        pinMode(pinList[i], OUTPUT);
    }
}

void pinsAsInputs()
{
    for (int i = 0; i < 4; i++)
    {
        pinMode(pinList[i], INPUT_PULLUP);
    }
}

uint8_t getKeys(void)
{
    uint8_t n = 0;
    uint8_t _b;
    int i;
    // We have to read the keys
    pinsAsInputs();
    while (n <= steps)
    {
        do
        {
            _b = getButtons();
        } while (_b == 0);

        for (i = 0; i < 4; i++)
        {
            if (_b & (1 << i))
                break;
        }

        delay(25);
        if (!digitalRead(pinList[i]))
        {
            while (getButtons())
                ;
            if (keys[n] != i)
                return 0;
            n++;
        }
    }
    return 1;
}

void showStartAnimation()
{
    int i;
    int j;

    pinsAsOutputs();

    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 4; i++)
        {
            digitalWrite(pinList[i], LOW);
            delay(100);
        }
        for (i = 0; i < 4; i++)
        {
            digitalWrite(pinList[i], HIGH);
            delay(100);
        }
    }

    delay(1000);
}

void showSequence(uint8_t *_s, uint8_t _n)
{
    int i;
    _s[_n] = calculateNewRandom();
    pinsAsOutputs();
    for (i = 0; i <= _n; i++)
    {
        digitalWrite(pinList[_s[i]], LOW);
        delay(delayTimes[(steps / 50) & 3]);
        digitalWrite(pinList[_s[i]], HIGH);
        delay(delayTimes[(steps / 50) & 3]);
    }
}

uint8_t calculateNewRandom()
{
    // Randomize it!
    randomSeed(analogRead(6) ^ micros());
    return ((uint8_t)random(0, 255) >> 3) & 0x03;
}

uint8_t getButtons()
{
    uint8_t _buttons = 0;
    pinsAsInputs();

    for (int i = 0; i < 4; i++)
    {
        _buttons |= (~digitalRead(pinList[i]) & 1) << i;
    }
    _buttons &= 0x0f;

    return (_buttons);
}

void showFailAnimation(void)
{
    int i;
    int j;
    pinsAsOutputs();
    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 4; i++)
        {
            digitalWrite(pinList[i], LOW);
        }
        delay(250);
        for (i = 0; i < 4; i++)
        {
            digitalWrite(pinList[i], HIGH);
        }
        delay(250);
    }
}

void showResult(uint8_t _r)
{
    uint8_t _blinks;
    uint8_t i;
    pinsAsOutputs();
    _blinks = _r / 100;
    for (i = 0; i < _blinks; i++)
    {
        digitalWrite(pinList[2], LOW);
        delay(SCORE_BLINK_ON);
        digitalWrite(pinList[2], HIGH);
        delay(SCORE_BLINK_OFF);
    }

    _blinks = _r / 10 % 10;
    for (i = 0; i < _blinks; i++)
    {
        digitalWrite(pinList[1], LOW);
        delay(SCORE_BLINK_ON);
        digitalWrite(pinList[1], HIGH);
        delay(SCORE_BLINK_OFF);
    }

    _blinks = _r % 10;
    for (i = 0; i < _blinks; i++)
    {
        digitalWrite(pinList[0], LOW);
        delay(SCORE_BLINK_ON);
        digitalWrite(pinList[0], HIGH);
        delay(SCORE_BLINK_OFF);
    }
}
