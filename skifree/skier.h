class Skier {
	public:
		float x, y, yVel;
		bool movingLeft, movingRight,
		     slowingDown;
	
		Skier(void);
		void draw(void);
		void update(int);

}; // end class
