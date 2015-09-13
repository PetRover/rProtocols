//
// Created by Bryce Cater on 8/31/15.
//

#include "pins.h"
#include <iostream>
#include <fstream>

namespace RVR
{
    const std::string GpioPin::PIN_BASE_PATH = "/sys/class/gpio/";
    const std::string AdcPin::PIN_BASE_PATH = "/sys/devices/"; // TODO make this the correct path
    const std::string PwmPin::PIN_BASE_PATH = "/sys/class/pwm/";

// ==============================================================
// Pin Class Member functions
// ==============================================================

    std::string Pin::getPropertyFilePath(PinProperty property)
    {
        switch (property)
        {
            case PinProperty::VALUE:
                return this->getPinBasePath() + "gpio" + std::to_string(this->pinNumber) + "/value";
            case PinProperty::DIRECTION:
                return this->getPinBasePath() + "gpio" + std::to_string(this->pinNumber) + "/direction";
            case PinProperty::ADC_VALUE:
                return this->getPinBasePath() + "/ocp.2/helper.14/" + this->pinDirectory;
            case PinProperty::PWM_DUTY:
                return this->getPinBasePath() + this->pinDirectory + "/duty_ns";
            case PinProperty::PWM_PERIOD:
                return this->getPinBasePath() + this->pinDirectory + "/period_ns";
            case PinProperty::PWM_RUN:
                return this->getPinBasePath() + this->pinDirectory + "/run";
        }
    }

    void Pin::writeToFile(std::string path, std::string data)
    {
        std::ofstream writeFile;
        writeFile.exceptions(std::ofstream::badbit | std::ofstream::failbit);
        try
        {
            writeFile.open(path);
            writeFile << data;
        }
        catch (std::ios_base::failure &failure)
        {
            printf("Failure occured while trying to write '%s' to '%s'. The failure was: %s\n", data.c_str(),
                   path.c_str(), failure.what());
            throw;
        }

    }

    std::string Pin::readFromFile(std::string path)
    {
        std::string result;
        std::ifstream readFile;
        readFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        try
        {
            readFile.open(path);
            readFile >> result;
        }
        catch (std::ios_base::failure &failure)
        {
            printf("Failure occured while trying to read from '%s'. The failure was: %s\n", path.c_str(),
                   failure.what());
            throw;
        }
        readFile.close();

        return result;
    }

    void Pin::writeToProperty(PinProperty property, std::string dataString)
    {
        std::string propertyPath = this->getPropertyFilePath(property);
        this->writeToFile(propertyPath, dataString);
    }

    void Pin::writeToProperty(PinProperty property, int data)
    {
        std::string propertyPath = this->getPropertyFilePath(property);
        try
        {
            std::string dataString = std::to_string(data);
            this->writeToFile(propertyPath, dataString);
        }
        catch (std::exception &exception)
        {
            printf("Could not convert '%d' to string. The error was: %s\n", data, exception.what());
            throw;
        }
    }

    void Pin::writeToProperty(PinProperty property, double data)
    {
        std::string propertyPath = this->getPropertyFilePath(property);
        try
        {
            std::string dataString = std::to_string(data);
            this->writeToFile(propertyPath, dataString);
        }
        catch (std::exception &exception)
        {
            printf("Could not convert '%f' to string. The error was: %s\n", data, exception.what());
            throw;
        }
    }

    int Pin::readIntFromProperty(PinProperty property)
    {
        std::string propertyPath = this->getPropertyFilePath(property);
        std::string readString;
        try
        {
            readString = this->readFromFile(propertyPath);
            int propertyValue = std::stoi(readString);
            return propertyValue;
        }
        catch (std::exception &exception)
        {
            printf("Failed to convert '%s' from string to int. The error was: %s\n", readString.c_str(),
                   exception.what());
            throw;
        }
    }

    double Pin::readDoubleFromProperty(PinProperty property)
    {
        {
            std::string propertyPath = this->getPropertyFilePath(property);
            std::string readString;
            try
            {
                readString = this->readFromFile(propertyPath);
                double propertyValue = std::stoi(readString);
                return propertyValue;
            }
            catch (std::exception &exception)
            {
                printf("Failed to convert '%s' from string to double. The error was: %s\n", readString.c_str(),
                       exception.what());
                throw;
            }
        }
    }

    std::string Pin::readStringFromProperty(PinProperty property)
    {
        std::string propertyPath = this->getPropertyFilePath(property);
        std::string propertyValue = this->readFromFile(propertyPath);
        return propertyValue;
    }

// ==============================================================
// GpioPin Class Member functions
// ==============================================================

    GpioPin::GpioPin(int pinNumber, GpioDirection direction)
    {
        this->pinNumber = pinNumber;
        this->pinDirectory = "gpio" + std::to_string(this->pinNumber);

        // Always initialize output pins with a low value
        if (direction == GpioDirection::OUT)
        {
            this->setValue(GpioValue::LOW);
        }

        this->setDirection(direction);

    }

    std::string GpioPin::getPinBasePath()
    {
        return GpioPin::PIN_BASE_PATH;
    }

    int GpioPin::setValue(GpioValue value)
    {
        switch (value)
        {
            case GpioValue::HIGH:
                this->writeToProperty(PinProperty::VALUE, 1);
                break;
            case GpioValue::LOW:
                this->writeToProperty(PinProperty::VALUE, 0);
                break;
            default:
                return -1;

        }
        return 0;
    }

    GpioValue GpioPin::getValue()
    {
        GpioValue value;
        std::string directionString = this->readStringFromProperty(PinProperty::VALUE);

        if (directionString == "high")
        {
            value = GpioValue::HIGH;
        }
        else if (directionString == "low")
        {
            value = GpioValue::LOW;
        }
        else
        {
            value = GpioValue::ERROR;
        }

        return value; // TODO handle bad data read error
    }

    int GpioPin::setDirection(GpioDirection direction)
    {
        switch (direction)
        {
            case GpioDirection::IN:
                this->writeToProperty(PinProperty::DIRECTION, "in");
                break;
            case GpioDirection::OUT:
                this->writeToProperty(PinProperty::DIRECTION, "out");
                break;
            default:
                return -1;
        }
        return 0;
    }


    GpioDirection GpioPin::getDirection()
    {
        GpioDirection direction;
        std::string directionString = this->readStringFromProperty(PinProperty::DIRECTION);

        if (directionString == "in")
        {
            direction = GpioDirection::IN;
        }
        else if (directionString == "out")
        {
            direction = GpioDirection::OUT;
        }
        else
        {
            direction = GpioDirection::ERROR;
        }

        return direction; // TODO handle bad data read error
    }

// ==============================================================
// AdcPin Class Member functions
// ==============================================================

    AdcPin::AdcPin(int pinNumber)
    {
        this->pinNumber = pinNumber;
        this->pinDirectory = "AIN" + std::to_string(getAdcPort(this->pinNumber));
    }

    std::string AdcPin::getPinBasePath()
    {
        return AdcPin::PIN_BASE_PATH;
    }

    double AdcPin::getValue()
    {
        double value = this->readDoubleFromProperty(PinProperty::ADC_VALUE);
        return value;
    }

//returns the AIN port number associated with the pin which is needed to read from pin
    int AdcPin::getAdcPort(int pinNumber)
    {
        switch (pinNumber)
        {
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
            default:
                return -1;
        }
    }

// ==============================================================
// PwmPin Class Member functions
// ==============================================================

    PwmPin::PwmPin(int pinNumber)
    {
        this->pinNumber = pinNumber;
        this->pinDirectory = "pwm" + std::to_string(getPwmPort(this->pinNumber));
    }

    std::string PwmPin::getPinBasePath()
    {
        return PwmPin::PIN_BASE_PATH;
    }

//returns the PWM port number associated with the pin which is needed to export the pin
    int PwmPin::getPwmPort(int pinNumber)
    {
        switch (pinNumber)
        {
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
            default:
                printf("An invalid pin (%d) was requested for PWM.", pinNumber);
                throw std::invalid_argument(std::to_string(pinNumber) + " is not a valid PWM pin");
        }
    }

    int PwmPin::getPeriod()
    {
        int period_ns = this->readIntFromProperty(PinProperty::PWM_PERIOD);
        return period_ns;
    }

    int PwmPin::getDutyCycleTime()
    {
        int dutyCycle_ns = this->readIntFromProperty(PinProperty::PWM_DUTY);
        return dutyCycle_ns;
    }

    int PwmPin::setPeriod(int period)
    {
        this->writeToProperty(PinProperty::PWM_PERIOD, period);
        return 0;
    }

    int PwmPin::setDutyCycleTime(int dutyCycle_ns)
    {
        this->writeToProperty(PinProperty::PWM_DUTY, dutyCycle_ns);
        return 0;
    }

    int PwmPin::setDutyCyclePercent(double dutyCyclePercent)
    {
        int period_ns = this->getPeriod();
        int dutyCycleTime_ns = (int) ((period_ns * 100) / dutyCyclePercent);
        this->setDutyCycleTime(dutyCycleTime_ns);
        return 0;
    }

    int PwmPin::setEnable(bool enable)
    {
        if (enable)
        {
            this->writeToProperty(PinProperty::PWM_RUN, 1);
        }
        else
        {
            this->writeToProperty(PinProperty::PWM_RUN, 0);
        }
        return 0;
    }
}
