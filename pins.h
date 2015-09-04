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
        PWM_PERIOD
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
        int writeToFile(std::string path, std::string data);

        // Returns the first line of the file given by the 'path' parameter
        std::string readFromFile(std::string path);

    protected:
        // Integer value corresponding to the pin number which this Pin instance represents
        int pinNumber;
        // The base path to where this type of pins' sysfs files are located
        static const std::string PIN_BASE_PATH; // TODO make sure this works and we do not need to implement a "getPinBasePath" virtual function see (http://stackoverflow.com/questions/10915568/is-it-possible-to-declare-a-virtual-static-constant-value-in-a-c-class)

        // Send a string of data to the specified property of the pin
        int writeToProperty(PinProperty property,
                            std::string dataString); // TODO make this function more robust by changing the type of dataString to a custom Enum type that only allows sending valid commands
        // Reads the value of the specified property of the pin
        std::string readFromProperty(PinProperty property);

    public:
    };

    // Subclass of Pin used to represent pins that are configured as GPIOs
    class GpioPin : Pin
    {
    protected:
        // Subclassed property. See Pin class
        static const std::string PIN_BASE_PATH;
    public:


        GpioPin() { }

        // construct a GPIO pin object and set the direction
        GpioPin(int pinNumber, GpioDirection direction);

        // Set the value of the the GPIO
        int setValue(GpioValue value);

        // Get the value of the GPIO
        GpioValue getValue();

        // Set the direction of the GPIO
        int setDirection(GpioDirection direction);

        // Get the direction of the GPIO
        GpioDirection getDirection();


    };

    // Subclass of Pin used to represent pins that are configured as ADCs
    class AdcPin : public Pin
    {
    protected:
        static const std::string PIN_BASE_PATH;
    public:
        AdcPin() { };

        AdcPin(int pinNumber);

        long getValue();
    };

    // Subclass of Pin used to represent pins that are configured as PWMs
    class PwmPin : public Pin
    {
    protected:
        static const std::string PIN_BASE_PATH;
    public:
        PwmPin() { };

        PwmPin(int pinNumber);

        int setPeriod(int period);
        int setDutyCycle(int dutyCycle);
    };
};
#endif //RCORE_GPIO_H
