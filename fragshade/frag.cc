#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdio>
#include <cstring>
using namespace std;

void printInfoLog(GLhandleARB p) {
	int len, charswritten;
	glGetObjectParameterivARB(p, GL_OBJECT_INFO_LOG_LENGTH_ARB, &len);
	char *log = (char*)operator new(len);
	glGetInfoLogARB(p, len, &charswritten, log);
	printf("%s\n", log);
	delete [] log;
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutCreateWindow("G");
	GLenum err = glewInit();
	if(err != GLEW_OK) {
		printf("%d\n", glewGetErrorString(err));
		return 1;
	}
	if(GLEW_ARB_vertex_shader)
		printf("Vertex shader supported!\n");
	else
		printf("Vertex shader NOT supported!\n");
	if(GLEW_ARB_fragment_shader)
		printf("Fragment shader supported!\n");
	else
		printf("Fragment shader NOT supported!\n");

	const char *prog = 	"void main() { \
					gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
				}";
	GLhandleARB s = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	glShaderSourceARB(s, 1, &prog, NULL);
	glCompileShaderARB(s);
	GLhandleARB p = glCreateProgramObjectARB();
	glAttachObjectARB(p, s);
	glLinkProgramARB(p);
	glUseProgramObjectARB(p);

	int linked, compiled;
	glGetObjectParameterivARB(p, GL_OBJECT_LINK_STATUS_ARB, &linked);
	glGetObjectParameterivARB(p, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
	if(!(linked && compiled))
		printInfoLog(p);
	else
		printf("Shader compiled and linked correctly!\n");

	glDetachObjectARB(p, s);
	glDeleteObjectARB(s);
	return 0;
}
