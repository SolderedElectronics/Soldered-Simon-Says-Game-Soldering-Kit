/**
 **************************************************
 *
 * @file        game.c
 * @brief       Game specific STM32 code for Simon Says Game
 *              Soldering Kit from Soldered.
 *
 * @note        In order to successfully run this code, make sure to use STM32 cube programmer
 * 				And set Option Bytes -> User Configuration -> NRST_MODE 2
 *
 * @authors     Robert Soric for soldered.com
 ***************************************************/
#include "game.h"

/**
 * @brief			Function which sets the four GPIO pins as inputs
 *
 * @returns			None
 */
void setPinsAsInputs() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/*Configure GPIO pin : LED_1_BLUE_Pin */
	GPIO_InitStruct.Pin = LED_1_BLUE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_1_BLUE_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED_2_YELLOW_Pin LED_3_GREEN_Pin LED_4_RED_Pin */
	GPIO_InitStruct.Pin = LED_2_YELLOW_Pin | LED_3_GREEN_Pin | LED_4_RED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
 * @brief			Function which sets the four GPIO pins as outputs
 *
 * @returns			None
 */
void setPinsAsOutputs() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/*Configure GPIO pin : LED_1_BLUE_Pin */
	GPIO_InitStruct.Pin = LED_1_BLUE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_1_BLUE_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED_2_YELLOW_Pin LED_3_GREEN_Pin LED_4_RED_Pin */
	GPIO_InitStruct.Pin = LED_2_YELLOW_Pin | LED_3_GREEN_Pin | LED_4_RED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/**
 * @brief			Function which shows the animation which plays at game start
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 *
 * @returns			None
 */
void showStartAnimation(GPIO_TypeDef * portList[], uint16_t  * pinList)
{
	int i;
	int j;

	setPinsAsOutputs();

	for (j = 0; j < 2; j++) {
		for (i = 0; i < 4; i++) {
			HAL_GPIO_WritePin(portList[i], pinList[i], GPIO_PIN_RESET);
			HAL_Delay(100);
		}
		for (i = 0; i < 4; i++) {
			HAL_GPIO_WritePin(portList[i], pinList[i], GPIO_PIN_SET);
			HAL_Delay(100);
		}
	}
	HAL_Delay(1000);

	setPinsAsInputs();
}

/**
 * @brief			Function which shows current game sequence
 *
 * @params			uint8_t _s: Array of keys
 *
 * @params			uint8_t _n: Current step in the game
 *
 * @params			uint8_t * delayTimes: Array of delay times
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 *
 * @returns			None
 */
void showSequence(uint8_t *_s, uint8_t _n, ADC_HandleTypeDef *hadc, uint8_t * delayTimes, GPIO_TypeDef * portList[], uint16_t  * pinList) {
	int i;
	_s[_n] = calculateNewRandom(hadc);
	setPinsAsOutputs();
	for (i = 0; i <= _n; i++) {
		HAL_GPIO_WritePin(portList[_s[i]], pinList[_s[i]], GPIO_PIN_RESET);
		HAL_Delay(delayTimes[(_n / 50) & 3]);
		HAL_GPIO_WritePin(portList[_s[i]], pinList[_s[i]], GPIO_PIN_SET);
		HAL_Delay(delayTimes[(_n / 50) & 3]);
	}
	setPinsAsInputs();
}

/**
 * @brief			Calculate a random number in range 0-3
 *
 * @params			ADC_HandleTypeDef *hadc: Pointer to ADC object to read noise from ADC to get the seed
 */
uint8_t calculateNewRandom(ADC_HandleTypeDef *hadc) {
	setPinsAsInputs();
	uint16_t seed;
	uint8_t temp;
	int i;
	//Get the data
	HAL_Delay(1);

	// Make 25 readings of noise on ADC pin and shuffle their bits
	for (i = 0; i < 25; i++) {
		// Randomly delay last couple of measurements by 3 ms max
		if(i > 22) HAL_Delay(temp & 0b00000011);
		seed ^= HAL_ADC_GetValue(hadc);
		//Shuffle the seed
		seed = 2053 * seed + 13849;
		//XOR two bytes
		temp = seed ^ (seed >> 8);
		//XOR two nibbles
		temp ^= (temp >> 4);
	}

	//XOR two pairs of bits and return remainder after division by 4
	return (temp ^ (temp >> 2)) & 0b00000011;
}

/**
 * @brief			Read the inputted keys from the buttons
 *
 * @params			uint8_t keys[]: The randomly generated steps of the game
 *
 * @params			uint8_t _n: Current step in the game
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 */
uint8_t getKeys(uint8_t keys[], uint8_t steps, GPIO_TypeDef * portList[], uint16_t  * pinList) {
	uint8_t n = 0;
	uint8_t _b;
	int i;
// We have to read the keys
	while (n <= steps) {
		do {
			_b = getButtons(portList, pinList);
		} while (_b == 0);

		for (i = 0; i < 4; i++) {
			if (_b & (1 << i))
				break;
		}
		HAL_Delay(25);
		if (HAL_GPIO_ReadPin(portList[i], pinList[i]) == GPIO_PIN_RESET) {
			while (getButtons(portList, pinList))
				;
			if (keys[n] != i)
				return 0;
			n++;
		}
	}
	return 1;
}

/**
 * @brief			Read the button states
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 */
uint8_t getButtons(GPIO_TypeDef * portList[], uint16_t  * pinList) {
	uint8_t _buttons = 0;

	for (int i = 0; i < 4; i++) {
		int pinState = HAL_GPIO_ReadPin(portList[i], pinList[i]);
		_buttons |= !pinState << i;
	}
	_buttons &= 0x0f;

	return (_buttons);
}

/**
 * @brief			Show the game over animation
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 */
void showFailAnimation(GPIO_TypeDef * portList[], uint16_t  * pinList) {
	int i;
	int j;
	setPinsAsOutputs();
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 4; i++) {
			HAL_GPIO_WritePin(portList[i], pinList[i], GPIO_PIN_RESET);
		}
		HAL_Delay(250);
		for (i = 0; i < 4; i++) {
			HAL_GPIO_WritePin(portList[i], pinList[i], GPIO_PIN_SET);
		}
		HAL_Delay(250);
	}
	setPinsAsInputs();
}

/**
 * @brief			Show the game result
 *
 * @params			uint8_t _r: Current result of the game
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 */
void showResult(uint8_t _r, GPIO_TypeDef * portList[], uint16_t  * pinList) {
	uint8_t _blinks;
	uint8_t i;
	setPinsAsOutputs();
	_blinks = _r / 100;
	for (i = 0; i < _blinks; i++) {
		HAL_GPIO_WritePin(portList[2], pinList[2], GPIO_PIN_RESET);
		HAL_Delay(SCORE_BLINK_ON);
		HAL_GPIO_WritePin(portList[2], pinList[2], GPIO_PIN_SET);
		HAL_Delay(SCORE_BLINK_OFF);
	}

	_blinks = _r / 10 % 10;
	for (i = 0; i < _blinks; i++) {
		HAL_GPIO_WritePin(portList[1], pinList[1], GPIO_PIN_RESET);
		HAL_Delay(SCORE_BLINK_ON);
		HAL_GPIO_WritePin(portList[1], pinList[1], GPIO_PIN_SET);
		HAL_Delay(SCORE_BLINK_OFF);
	}

	_blinks = _r % 10;
	for (i = 0; i < _blinks; i++) {
		HAL_GPIO_WritePin(portList[0], pinList[0], GPIO_PIN_RESET);
		HAL_Delay(SCORE_BLINK_ON);
		HAL_GPIO_WritePin(portList[0], pinList[0], GPIO_PIN_SET);
		HAL_Delay(SCORE_BLINK_OFF);
	}
	setPinsAsInputs();
}
