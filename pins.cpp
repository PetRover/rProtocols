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

const std::string RVR::GpioPin::PIN_BASE_PATH = "/dev/gpio/"; // TODO make this the correct path
const std::string RVR::AdcPin::PIN_BASE_PATH = "/dev/adc/"; // TODO make this the correct path
const std::string RVR::PwmPin::PIN_BASE_PATH = "/dev/pwm/"; // TODO make this the correct path

// ==============================================================
// Pin Class Member functions
// ==============================================================

std::string RVR::Pin::getPropertyFilePath(RVR::PinProperty property)
{
    switch (property)
    {
        case RVR::PinProperty::VALUE:
            // NOTE %d should get replaced with this->pinNumber somehow
            return this->PIN_BASE_PATH + "%d/value.file?"; // TODO make this the correct path
        case RVR::PinProperty::DIRECTION:
            return this->PIN_BASE_PATH + "%d/direction.file?"; // TODO make this the correct path
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

std::string RVR::Pin::readFromFile(std::string path)
{

    std::string result;
    std::ifstream readFile;
    readFile.open(path);
    readFile >> result;
    return result;
}

int RVR::Pin::exportPin()
{
    this->writeToFile(this->PIN_BASE_PATH + "export",
                      "placeholder"); // TODO replace placeholder with this->pinNumber cast to string

    return 0;
}

int RVR::Pin::unexportPin()
{
    this->writeToFile(this->PIN_BASE_PATH + "unexport",
                      "placeholder"); // TODO replace placeholder with this->pinNumber cast to string

    return 0;
}


int RVR::Pin::writeToProperty(RVR::PinProperty property, std::string dataString)
{
    std::string propertyPath = this->getPropertyFilePath(property);
    this->writeToFile(propertyPath, dataString);
    return 0;
}

std::string RVR::Pin::readFromProperty(RVR::PinProperty property)
{
    std::string propertyPath = this->getPropertyFilePath(property);
    std::string propertyValue = this->readFromFile(propertyPath);
    return propertyValue;
}

RVR::Pin::~Pin()
{
    this->unexportPin();
}

// ==============================================================
// GpioPin Class Member functions
// ==============================================================

RVR::GpioPin::GpioPin(int pinNumber, RVR::GpioDirection direction)
{
    this->pinNumber = pinNumber;
    this->exportPin();
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
}

// ==============================================================
// PwmPin Class Member functions
// ==============================================================

RVR::PwmPin::PwmPin(int pinNumber)
{
    this->pinNumber = pinNumber;
}
