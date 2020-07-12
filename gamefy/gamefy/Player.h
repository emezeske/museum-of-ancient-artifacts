class Player {
	public:
		float x, y, z, xv, yv, zv, pitch, yaw, speed, strafeSpeed, gravity; 
		bool feetOnGround, movinLeft, movinRight, movinForward, movinBackward, jumpin;

		Player(float X, float Y, float Z);
		void move( int ms );
		void adjustVelocity( GLfloat adjustVmag, GLfloat adjustVangle, int ms);
		void moveForward(bool movin);
		void moveBackward(bool movin);
		void moveLeft(bool movin);
		void moveRight(bool movin);
		void jump(bool jumpin);
}; // end class Player
