//
// Created by Bryce Cater on 8/31/15.
//

#include "pins.h"
#include <iostream>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedImportStatement"
#include <fstream>
#include <string>
#include <stdexcept>

#define CONFIRM_SYSFS_WRITES

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
                return this->getPinBasePath() + this->pinDirectory + "/value";
            case PinProperty::DIRECTION:
                return this->getPinBasePath() + this->pinDirectory + "/direction";
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
#ifdef CONFIRM_SYSFS_WRITES
            std::string storedString = readFromFile(path);
            if (storedString != data)
            {
                printf("[ERROR] Failed to write '%s' to the file '%s'\n", data.c_str(), path.c_str());
            }
#endif
        }
        catch (std::ios_base::failure &failure)
        {
            printf("Failure occured while trying to write '%s' to '%s'. The failure was: %s\n", data.c_str(), path.c_str(), failure.what());
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
                printf("Failed to convert '%s' from string to double. The error was: %s\n", readString.c_str(), exception.what());
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

    GpioPin::GpioPin(int deviceNumber)
    {
        this->deviceNumber = deviceNumber;
        this->pinDirectory = "gpio" + std::to_string(this->deviceNumber);

        // Always initialize output pins with a low value
        if (this->getDirection() == GpioDirection::OUT)
        {
            this->setValue(GpioValue::LOW);
        }

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

    AdcPin::AdcPin(int deviceNumber)
    {
        this->deviceNumber = deviceNumber;
        this->pinDirectory = "AIN" + std::to_string(deviceNumber);
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

// ==============================================================
// PwmPin Class Member functions
// ==============================================================

    PwmPin::PwmPin(int deviceNumber)
    {
        this->deviceNumber = deviceNumber;
        this->pinDirectory = "pwm" + std::to_string(this->deviceNumber);
    }

    std::string PwmPin::getPinBasePath()
    {
        return PwmPin::PIN_BASE_PATH;
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
        int dutyCycleTime_ns = (int) ((period_ns * dutyCyclePercent) / 100);
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

#pragma clang diagnostic pop
