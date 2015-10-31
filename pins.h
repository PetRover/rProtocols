//
// Created by Bryce Cater on 8/31/15.
//

#ifndef RCORE_GPIO_H
#define RCORE_GPIO_H

#include <iosfwd>
#include <string>


namespace RVR
{
    // Options for the direction of a GPIO pin
    enum class GpioDirection
    {
        IN, // Input
        OUT, // Output
        ERROR // Invalid/error
    };

    // Options for the value of a GPIO pin
    enum class GpioValue
    {
        HIGH, // Output or input at voltage logic level (high) or above
        LOW, // Output or input at voltage logic level (low) or below
        ERROR // Invalid/error
    };

    // Properties that a pin can have
    enum class PinProperty
    {
        VALUE, // The logic level value of a pin
        DIRECTION, // The direction (in or out) of a pin
        ADC_VALUE,  // Analog value from the ADC
        PWM_DUTY,
        PWM_PERIOD,
        PWM_RUN
    };


    // Base class for pin objects
    class Pin
        // A pin object represents a physical pin the on the microprocessor.
        // The basic functionality of pins is reading and writing values that relate to their state or configuration.
        // This class should always be subclassed to represent a specific type of pin.
    {
    private:
        // Returns the full path of a sysfs file that represents a given property of the pin
        std::string getPropertyFilePath(PinProperty property);

        // Writes an arbitrary string to a file given by the 'path' parameter
        void writeToFile(std::string path, std::string data);

        // Returns the first line of the file given by the 'path' parameter
        std::string readFromFile(std::string path);

    protected:
        // Integer value corresponding to the pin number which this Pin instance represents
        int deviceNumber;

        // The base path to where this type of pins' sysfs files are located
        static const std::string PIN_BASE_PATH;

        virtual std::string getPinBasePath() = 0; // https://github.com/PetRover/rProtocols/issues/5

        // The directory to access the specific pin within the PIN_BASE_PATH directory
        std::string pinDirectory;

        // Send a string of data to the specified property of the pin
        void writeToProperty(PinProperty property, std::string dataString);

        // Overloads writeToProperty to allowing writing ints
        void writeToProperty(PinProperty property, int data);

        // Overloads writeToProperty to allowing writing doubles
        void writeToProperty(PinProperty property, double data);

        // Reads an int value from the specified property of the pin
        int readIntFromProperty(PinProperty property);

        // Reads a double value from the specified property of the pin
        double readDoubleFromProperty(PinProperty property);

        // Reads a string value from the specified property of the pin
        std::string readStringFromProperty(PinProperty property);

    public:
    };

    // Subclass of Pin used to represent pins that are configured as GPIOs
    class GpioPin : Pin
    {
    protected:
        // Subclassed property. See Pin class
        static const std::string PIN_BASE_PATH;

        std::string getPinBasePath(); // https://github.com/PetRover/rProtocols/issues/5

        // sets the direction of the GPIO
        int setDirection(GpioDirection direction);
    public:
        void initGpio(int deviceNumber, GpioDirection direction);

        GpioPin() { }

        // construct a GPIO pin object and set the direction
        GpioPin(int deviceNumber);

        GpioPin(int deviceNumber, GpioDirection direction);

        // Set the value of the the GPIO
        int setValue(GpioValue value);

        // Get the value of the GPIO
        GpioValue getValue();

        // Get the direction of the GPIO
        GpioDirection getDirection();

    };

    // Subclass of Pin used to represent pins that are configured as ADCs
    class AdcPin : public Pin
    {
    protected:
        static const std::string PIN_BASE_PATH;

        std::string getPinBasePath(); // https://github.com/PetRover/rProtocols/issues/5
    public:
        AdcPin() { };

        AdcPin(int deviceNumber);

        double getValue();
    };

    // Subclass of Pin used to represent pins that are configured as PWMs
    class PwmPin : public Pin
    {
    protected:
        static const std::string PIN_BASE_PATH; // https://github.com/PetRover/rProtocols/issues/5

        std::string getPinBasePath();
    public:
        PwmPin() { };

        PwmPin(int deviceNumber);

        // Returns the period in nanoseconds
        int getPeriod();

        // Returns the duty cycle in nanoseconds
        int getDutyCycleTime();

        // period is in nanoseconds
        int setPeriod(int period);

        int setDutyCycleTime(int dutyCycleTime_ns);

        int setDutyCyclePercent(double dutyCyclePercent);

        int setEnable(bool enable);
    };
};
#endif //RCORE_GPIO_H
