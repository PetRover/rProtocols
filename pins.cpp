//
// Created by Bryce Cater on 8/31/15.
//

#include "pins.h"
#include <iostream>
#include <fstream>

//RVR::GpioPin::GpioPin (int pinNumber, RVR::GpioDirection direction)
//{
//	this->PIN_PATH = "/dev/gpio";
//	this->pinNumber = pinNumber;
//	this->setDirection(direction);
//}

const std::string RVR::GpioPin::PIN_BASE_PATH = "/sys/class/gpio/"; // TODO make this the correct path
const std::string RVR::AdcPin::PIN_BASE_PATH = "/sys/devices/"; // TODO make this the correct path
const std::string RVR::PwmPin::PIN_BASE_PATH = "/sys/class/pwm/"; // TODO make this the correct path

// ==============================================================
// Pin Class Member functions
// ==============================================================

std::string RVR::Pin::getPropertyFilePath(RVR::PinProperty property)
{
    switch (property)
    {
        case RVR::PinProperty::VALUE:
            return this->PIN_BASE_PATH + "gpio" + std::to_string(this->pinNumber) + "/value";
        case RVR::PinProperty::DIRECTION:
            return this->PIN_BASE_PATH + "gpio" + std::to_string(this->pinNumber) + "/direction";
        case RVR::PinProperty::ADC_VALUE:
            return this->PIN_BASE_PATH + "/ocp.2/helper.14/" + this->pinDirectory;
        case RVR::PinProperty::PWM_DUTY:
            return this->PIN_BASE_PATH + this->pinDirectory + "/duty_ns";
        case RVR::PinProperty::PWM_PERIOD:
            return this->PIN_BASE_PATH + this->pinDirectory + "/period_ns";
    }
}

int RVR::Pin::writeToFile(std::string path, std::string data)
{
    std::ofstream writeFile;
    writeFile.open(path);
    writeFile << data;
    writeFile.close();
    return 0;
}

int RVR::Pin::writeToFile(std::string path, int data)
{
    std::ofstream writeFile;
    writeFile.open(path);
    writeFile << data;
    writeFile.close();
    return 0;
}

int RVR::Pin::writeToFile(std::string path, long data)
{
    std::ofstream writeFile;
    writeFile.open(path);
    writeFile << data;
    writeFile.close();
    return 0;
}

int RVR::Pin::writeToFile(std::string path, float data)
{
    std::ofstream writeFile;
    writeFile.open(path);
    writeFile << data;
    writeFile.close();
    return 0;
}

std::string RVR::Pin::readFromFile(std::string path)
{

    std::string result;
    std::ifstream readFile;
    readFile.open(path);
    readFile >> result;
    return result;
}

int RVR::Pin::writeToProperty(RVR::PinProperty property, std::string dataString)
{
    std::string propertyPath = this->getPropertyFilePath(property);
    this->writeToFile(propertyPath, dataString);
    return 0;
}

int RVR::Pin::writeToProperty(RVR::PinProperty property, int data)
{
    std::string propertyPath = this->getPropertyFilePath(property);
    this->writeToFile(propertyPath, data);
    return 0;
}

int RVR::Pin::writeToProperty(RVR::PinProperty property, long data)
{
    std::string propertyPath = this->getPropertyFilePath(property);
    this->writeToFile(propertyPath, data);
    return 0;
}

int RVR::Pin::writeToProperty(RVR::PinProperty property, float data)
{
    std::string propertyPath = this->getPropertyFilePath(property);
    this->writeToFile(propertyPath, data);
    return 0;
}

std::string RVR::Pin::readFromProperty(RVR::PinProperty property)
{
    std::string propertyPath = this->getPropertyFilePath(property);
    std::string propertyValue = this->readFromFile(propertyPath);
    return propertyValue;
}

// ==============================================================
// GpioPin Class Member functions
// ==============================================================

RVR::GpioPin::GpioPin(int pinNumber, RVR::GpioDirection direction)
{
    this->pinNumber = pinNumber;
    this->pinDirectory = "gpio" +  std::to_string(this->pinNumber);
    this->setDirection(direction);
}

int RVR::GpioPin::setValue(RVR::GpioValue value)
{
    switch (value)
    {
        case RVR::GpioValue::HIGH:
            this->writeToProperty(RVR::PinProperty::DIRECTION, "high"); // TODO make this the right value
            break;
        case RVR::GpioValue::LOW:
            this->writeToProperty(RVR::PinProperty::DIRECTION, "low"); // TODO make this the right value
            break;
        default:
            return -1;

    }
    return 0;
}

RVR::GpioValue RVR::GpioPin::getValue()
{
    RVR::GpioValue value;
    std::string directionString = this->readFromProperty(RVR::PinProperty::VALUE);

    if (directionString == "high")
    {
        value = RVR::GpioValue::HIGH;
    }
    else if (directionString == "low")
    {
        value = RVR::GpioValue::LOW;
    }
    else
    {
        value = RVR::GpioValue::ERROR;
    }

    return value; // TODO handle bad data read error
}

int RVR::GpioPin::setDirection(RVR::GpioDirection direction)
{
    switch (direction)
    {
        case RVR::GpioDirection::IN:
            this->writeToProperty(RVR::PinProperty::DIRECTION, "in"); // TODO make this the right value
            break;
        case RVR::GpioDirection::OUT:
            this->writeToProperty(RVR::PinProperty::DIRECTION, "out"); // TODO make this the right value
            break;
        default:
            return -1;
    }
    return 0;
}


RVR::GpioDirection RVR::GpioPin::getDirection()
{
    RVR::GpioDirection direction;
    std::string directionString = this->readFromProperty(RVR::PinProperty::DIRECTION);

    if (directionString == "in")
    {
        direction = RVR::GpioDirection::IN;
    }
    else if (directionString == "out")
    {
        direction = RVR::GpioDirection::OUT;
    }
    else
    {
        direction = RVR::GpioDirection::ERROR;
    }

    return direction; // TODO handle bad data read error
}

// ==============================================================
// AdcPin Class Member functions
// ==============================================================

RVR::AdcPin::AdcPin(int pinNumber)
{
    this->pinNumber = pinNumber;
    this->pinDirectory = "AIN" + std::to_string(getAdcPort(this->pinNumber));
}

long RVR::AdcPin::getValue()
{
    long value;
    std::string valueString = this->readFromProperty(RVR::PinProperty::ADC_VALUE);
    value = std::stol(valueString);  //string to long conversion
    return value;
}

//returns the AIN port number associated with the pin which is needed to read from pin
int RVR::AdcPin::getAdcPort(int pinNumber)
{
    switch(pinNumber) {
        case 85: // pin 9_39
            return 0;
        case 86: // pin 9_40
            return 1;
        case 83: // pin 9_37
            return 2;
        case 84: // pin 9_38
            return 3;
        case 79: // pin 9_33
            return 4;
        case 82: // pin 9_36
            return 5;
        case 81: // pin 9_35
            return 6;
    }
}

// ==============================================================
// PwmPin Class Member functions
// ==============================================================

RVR::PwmPin::PwmPin(int pinNumber)
{
    this->pinNumber = pinNumber;
    this->pinDirectory = "pwm" + std::to_string(getPwmPort(this->pinNumber));
}

//returns the PWM port number associated with the pin which is needed to export the pin
int RVR::PwmPin::getPwmPort(int pinNumber)
{
    switch(pinNumber) {
        case 68: // pin 9_22
            return 0;
        case 77: // pin 9_31
            return 0;
        case 67: // pin 9_21
            return 1;
        case 75: // pin 9_29
            return 1;
        case 88: // pin 9_42
            return 2;
        case 60: // pin 9_14
            return 3;
        case 36: // pin 8_36
            return 3;
        case 62: // pin 9_16
            return 4;
        case 34: // pin_8_34
            return 4;
        case 19: // pin 8_19
            return 5;
        case 45: // pin 8_45
            return 5;
        case 13: // pin 8_13
            return 6;
        case 46: // pin 8_46
            return 6;
        case 74: // pin 9_28
            return 7;
    }
}

int RVR::PwmPin::setPeriod(int period)
{
    this->writeToProperty(RVR::PinProperty::PWM_PERIOD, period);
    return 0;
}

int RVR::PwmPin::setDutyCycle(int dutyCycle)
{
    this->writeToProperty(RVR::PinProperty::PWM_DUTY, dutyCycle);
    return 0;
}