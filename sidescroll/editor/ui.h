class UI {
	public:
	bool showMenu  , showCollisions, showBackLayer,
	     showStatus, showFrontLayer;

	int * currentFWO;

	int selectedMenuItem, backOrFrontFWO,
	    frontCurrentFWO , backCurrentFWO;

	UI(void);

	void displayLevel(void);
	void displayMenu(void);
	void displayCollisionBlocks(void);

	void switchDispMode(void);

	void newFWO(void);
	void deleteCurFWO(void);

	void selectNextFWO(void);
	void selectPrevFWO(void);

	void mouseClick(int, int);
	void moveLeft(bool);
	void moveRight(bool);
	void moveUp(bool);
	void moveDown(bool);

}; // end class UI
