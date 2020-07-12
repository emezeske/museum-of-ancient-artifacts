class WorldObject {

	public:
	float x, y;	
	int width, height;
	int textureNumber;

	virtual void draw(bool) = 0;

}; // end class WorldObject
