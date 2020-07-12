
class MobileWorldObject : public WorldObject {

	protected:
	float velX, velY;

	public:
	float returnVelX(void);
	float returnVelY(void);
	virtual void updateState(int) = 0;

}; // end class MobileWorldObject
