#include <GL/glut.h>

void init()
{
    glClearColor(0.5,0.5,0.5,0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 150.0, 0.0, 250.0);
}

void lineSegment()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.4, 0.0);
    glBegin(GL_LINES);
        glVertex2i(150, 250);
        glVertex2i(0, 0);
    glEnd();

    glFlush();
}

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100,125);
    glutInitWindowSize(150,250);
    glutCreateWindow("Example");
    init();
    glutDisplayFunc(lineSegment);
    glutMainLoop();
    return 0;
}
