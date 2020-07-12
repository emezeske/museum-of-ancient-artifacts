class FixedWorldObject : public WorldObject {
	public:
		float x, y;
		int width, height, textureNumber;

		FixedWorldObject();
		FixedWorldObject(FixedWorldObject *);

		void draw(void);
}; // end class FixedWorldObject
