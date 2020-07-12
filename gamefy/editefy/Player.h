class editefyPlayer {
	public:
		float x, y, z, xv, yv, zv, pitch, yaw, speed, strafeSpeed, altSpeed, gravity; 
		bool feetOnGround, movinLeft, movinRight, movinForward, movinBackward, jumpin, movinUp, movinDown;

		editefyPlayer(float X, float Y, float Z);
		void move( int ms );
		void Reset();
		void adjustVelocity( GLfloat adjustVmag, GLfloat adjustVangle, int ms);
		void adjustAltitudeVelocity( float, int);
		void moveForward(bool movin);
		void moveBackward(bool movin);
		void moveLeft(bool movin);
		void moveRight(bool movin);
		void moveUp(bool movin);
		void moveDown(bool movin);
		void jump(bool jumpin);
}; // end class editefyPlayer
