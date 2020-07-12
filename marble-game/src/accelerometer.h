#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <vector>
#include <string>
#include <sys/time.h>
#include <time.h>

enum AxisType
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
    NUM_AXES
};

struct AccelerometerReading
{
    AccelerometerReading();

    bool is_set() const;
    void reset();
    void print() const;

    int acceleration_[NUM_AXES];
    struct timeval measured_at_[NUM_AXES];
};

typedef std::vector<AccelerometerReading> AccelerometerReadingVector;
void average_accelerometer_readings( const AccelerometerReadingVector &readings, AccelerometerReading &average );

class AccelerometerReader
{
    public:
        AccelerometerReader( const std::string &device_file );
        ~AccelerometerReader();

        bool read( AccelerometerReading &reading );

    private:
        int fd_;
        const std::string device_file_;
};

#endif // ACCELEROMETER_H
