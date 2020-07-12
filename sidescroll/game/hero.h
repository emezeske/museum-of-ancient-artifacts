class Hero : public MobileWorldObject {
	private:
	bool jumping;
	int  currentFrame, frameTimeCounter;
	short frameDelta;

	public:
	bool movingLeft, movingRight;

	Hero(float, float, float, float);

	void jump(void);
	void draw(void);
	void updateState(int);
	void checkCollisions(float, float, int);

}; // end class Hero
