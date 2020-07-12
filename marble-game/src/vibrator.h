#ifndef VIBRATOR_H
#define VIBRATOR_H

#include <iostream>
#include <fstream>

class Vibrator
{
    public:
        Vibrator( const std::string &device_file );
        ~Vibrator();

        void set_level( float level );
        void pulse( float level, int length_ms );
        void step();
        void turn_off();

    private:
        std::ofstream file_;
        unsigned long pulse_end_;
};

#endif // VIBRATOR_H
