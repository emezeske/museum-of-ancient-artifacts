#include <GL/glut.h>
#include <string>

using namespace std;
enum fontSetOption { FONT_SIZE } ;

string numberToString(int);

void showStatus(void);
void drawAllText(void);
void drawTextBox(string, float, float, float);
void drawBorderBox(float, float, float, float);
void drawAChar(float, float, char);
void fontSet(fontSetOption, float);
void initTextDisplayer(void);
