#include <GL/glut.h>
typedef GLint vertex3 [3];
vertex3 pt [8] = { {0,0,0},{0,1,0}, {1,0,0},{1,1,0},
                   {0,0,1},{0,1,1},{1,0,1},{1,1,1}};

GLubyte bitShape[20] = {
  0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00,
  0xff, 0x80, 0x7f, 0x00, 0x3e, 0x00, 0x1c, 0x00, 0x08, 0x00};


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
    int  vertex1[] = {50,50};
    int  vertex2[] = {100,100};
    glRectiv(vertex1,vertex2);

glEnableClientState(GL_VERTEX_ARRAY);
glVertexPointer(3, GL_INT, 0, pt);

GLubyte vertIndex[] = {6,2,3,7,5,1,0,4,7,3,1,5,4,0,2,6,2,0,1,3,7,5,4,6};

glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, vertIndex);

glDisableClientState(GL_VERTEX_ARRAY);

glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
glRasterPos2i(30,40);
glBitmap(9, 10, 0.0, 0.0, 20.0, 15.0, bitShape);

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
