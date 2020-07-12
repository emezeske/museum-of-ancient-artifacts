class Fracterrain
{
	public:
		static const unsigned TER_W = 129;

		Fracterrain( unsigned seed = 1, float jitter = 1.0, float roughness = 0.5 );

		void draw( void );

	private:
		inline float get_random_height( float jitter );

		void generate( unsigned seed, float jitter, float roughness );

		void do_one_square(	float jitter,
							unsigned lX, unsigned lY,
							unsigned uX, unsigned uY );

		float hmap[TER_W][TER_W];
}; // end class Fracterrain