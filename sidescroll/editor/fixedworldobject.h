class FixedWorldObject : public WorldObject {
	public:

		FixedWorldObject();
		FixedWorldObject(FixedWorldObject *);
		FixedWorldObject(FixedWorldObject *, FixedWorldObject *);

		void draw(bool);
		void drawMenuTile(int, int, bool);
}; // end class FixedWorldObject
