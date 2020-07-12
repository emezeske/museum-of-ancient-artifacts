#ifndef MANDLEBROT_SHADER_H
#define MANDLEBROT_SHADER_H

#include "fractal_generator.h"
#include "shader.h"
#include "utilities.h"

struct MandelbrotShader : public FractalGenerator
{
    static cstring GENERATOR_NAME;

    MandelbrotShader();
    virtual ~MandelbrotShader();

    virtual void doOneStep( double step_time );
    virtual void draw( const Vector2Di& screen_size, const Vector2Df& viewport_position, const Vector2Df& viewport_size );

private:
    TextureMap palette_;

    float palette_offset_;

    int max_iterations_;

    Shader shader_;

    void initialize_gui();
    void destroy_gui();

    void set_uniform_variables();
};

#endif // MANDLEBROT_SHADER_H
