/*
 * Draw a grid using: 
 *   immediate mode and coordinates recalculated each frame
 *   immediate mode and coordinates stored in an array
 *   immediate mode and coordinates and indices (elements) stored in an array  
 *   vertex arrays 
 *   vertex buffer objects
 *
 * $Id: grid-fw.c,v 1.4 2018/07/26 01:07:33 gl Exp gl $
 */

#define GL_GLEXT_PROTOTYPES

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define BUFFER_OFFSET(i) ((void*)(i))

typedef struct { float x, y, z; } vec3f;
typedef struct { vec3f r, n; } Vertex;

Vertex *vertices;
unsigned *indices;
unsigned n_vertices, n_indices;
unsigned vbo, ibo;
unsigned rows = 4, cols = 4;

enum { IM=0, SA, SAI, VA, VBO, nM } mode;

void enableVAs()
{
glEnableClientState(GL_VERTEX_ARRAY);
glVertexPointer(3, GL_FLOAT, sizeof(Vertex), vertices);
}

void disableVAs()
{
glEnableClientState(GL_VERTEX_ARRAY);
}

void bindVBOs()
{
}

void unbindVBOs()
{
glBindBuffer(GL_ARRAY_BUFFER,0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void buildVBOs(int i)
{
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glVertexPointer(3,GL_FLOAT,sizeof(Vertex),BUFFER_OFFSET(0));
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLE_STRIP, (rows + 1) * 2, GL_UNSIGNED_INT, BUFFER_OFFSET(i * (rows + 1) * 2 * sizeof(unsigned int)));
}

void enableVBOs()
{
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, n_vertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

}

void disableVBOs()
{
unbindVBOs();
}


void drawAxes(float length)
{

  glPushAttrib(GL_CURRENT_BIT);
  glBegin(GL_LINES);

  /* x axis */
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(length, 0.0, 0.0);

  /* y axis */
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, length, 0.0);

  /* z axis */
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, length);

  glEnd();
  glPopAttrib();
}

void drawGrid2D(int rows, int cols)
{
  glPushAttrib(GL_CURRENT_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(1.0, 1.0, 1.0);

  /* Grid */
  float dy = 2.0f / (float) rows;
  float dx = 2.0f / (float) cols;
  for (int i = 0; i < cols; i++) {
    float x = -1.0 + i * dx;
    glBegin(GL_TRIANGLE_STRIP);
    for (int j = 0; j <= rows; j++) {
      float y = -1.0 + j * dy;
      glVertex3f(x, y, 0.0);
      glVertex3f(x + dx, y, 0.0);
    }
    glEnd();
  }

  glPopAttrib();
}

void computeAndStoreGrid2D(int rows, int cols)
{
  n_vertices = (rows + 1) * (cols + 1);
  n_indices = (rows + 1) * (cols - 1) * 2 + (rows + 1) * 2;
  // or more simply: n_indices = n_vertices * 2; 
  free(vertices);
  vertices = (Vertex *)malloc(n_vertices * sizeof(Vertex));
  free(indices);
  indices = (unsigned *)malloc(n_indices * sizeof(unsigned));
  
  
  /* Grid */

  /* Vertices */
  float dy = 2.0f / (float) rows;
  float dx = 2.0f / (float) cols;
  Vertex *vtx = vertices;
  for (int i = 0; i <= cols; i++) {
    float x = -1.0 + i * dx;
    for (int j = 0; j <= rows; j++) {
      float y = -1.0 + j * dy;
      vtx->r = (vec3f) { x, y, 0.0 };
      vtx++;
    }
  }

  /* Indices */
  unsigned *idx = indices;
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j <= rows; j++) {
      *idx++ = i * (rows + 1) + j;
      *idx++ = (i + 1) * (rows + 1) + j;
    }
  }

#define DEBUG_STORE_VERTICES
#ifdef DEBUG_STORE_VERTICES
  for (int i = 0; i <= cols; i++) {
    for (int j = 0; j <= rows; j++) {
      int idx = i * (rows + 1) + j;
      printf("(%5.2f,%5.2f)", vertices[idx].r.x, vertices[idx].r.y);
    }
    printf("\n");
  }
  for (int i = 0; i < n_indices; i++) {
    printf("%d ", indices[i]);
  }
  printf("\n");
#endif
  
}

void drawGrid2DStoredVertices(int rows, int cols)
{
  glPushAttrib(GL_CURRENT_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(1.0, 1.0, 1.0);

  /* Grid */
  for (int i = 0; i < cols; i++) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int j = 0; j <= rows; j++) {
      int idx = i * (rows + 1) + j;
//#ifdef DEBUG_DRAW_GRID_ARRAY
      printf("%d %d %d %f %f\n", i, j, idx, vertices[idx].r, vertices[idx].n);
//#endif
      glVertex3fv(&vertices[idx].r.x);
      idx += rows + 1;
//#ifdef DEBUG_DRAW_GRID_ARRAY
      printf("%d %d %d %f %f\n", i, j, idx, vertices[idx].r, vertices[idx].n);
//#endif
      glVertex3fv(&vertices[idx].r.x);
    }
    glEnd();
  }

  glPopAttrib();
}

void drawGrid2DStoredVerticesAndIndices(int rows, int cols)
{
  glPushAttrib(GL_CURRENT_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(1.0, 1.0, 1.0);

  /* Grid */
  int idx = 0;
  for (int i = 0; i < cols; i++) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int j = 0; j <= rows; j++) {
      glVertex3fv(&vertices[indices[idx]].r.x);
      	idx++;
      glVertex3fv(&vertices[indices[idx]].r.x);
      	idx++;
	printf("%d\n",idx);
	printf("%f, %f\n",vertices[indices[idx]].r.x,vertices[indices[idx]].r.y);
	

    }
    glEnd();
  }

  glPopAttrib();
}

void drawGrid2DVAs(int rows, int cols)
{
  glPushAttrib(GL_CURRENT_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(1.0, 1.0, 1.0);

  /* Grid */
  for (int i = 0; i < cols; i++) {
    glDrawElements(GL_TRIANGLE_STRIP, (rows + 1) * 2, GL_UNSIGNED_INT, &indices[i * (rows + 1) * 2]);
  }

  glPopAttrib();
}

void drawGrid2DVBOs(int rows, int cols)
{
  glPushAttrib(GL_CURRENT_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(1.0, 1.0, 1.0);

  /* Grid */
  for (int i = 0; i < cols; i++) {
    buildVBOs(i);
  }

  glPopAttrib();
}


void init()
{
  computeAndStoreGrid2D(rows, cols);

}

void display(void)
{
  int err; 

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  glLoadIdentity();
  drawAxes(5.0);

  // Draw grid
  printf("mode %d\n", mode);
  switch (mode) {
  case IM:
    drawGrid2D(rows, cols);
    break;
  case SA:
    drawGrid2DStoredVertices(rows, cols);
    break;
  case SAI:
    drawGrid2DStoredVerticesAndIndices(rows, cols);
    break;
  case VA:
    enableVAs();
	//disableVBOs();
    drawGrid2DVAs(rows, cols);
    break;
  case VBO:
    enableVBOs();
	//disableVAs();
    drawGrid2DVBOs(rows, cols);
    break;
  case nM:
    break;
  }

  // Draw axes

  glPopMatrix();

  glutSwapBuffers();

  if ((err = glGetError()) != GL_NO_ERROR)
    printf("display: %s\n", gluErrorString(err));
}

void key(unsigned char k, int x, int y)
{
  printf("key = %u\n", k);

  switch (k) {
  case 27:  /* Escape */
    exit(0);
    break;
  case 'm':
    if (mode == VA)
      disableVAs();
    if (mode == VBO)
      disableVBOs();
    mode++;
    if (mode >= nM)
      mode = IM;
    if (mode == VA)
      enableVAs();
    if (mode == VBO)
      enableVBOs();
    break;
  default:
    return;
  }
  glutPostRedisplay();
}

void update()
{
}


void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-2.0, 2.0, -2.0, 2.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
}

/*
 * Main Loop Open window with initial window size, title bar, RGBA display
 * mode, and handle input events.
 */
int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(50, 50);
  glutInitWindowSize(500, 500);
  glutCreateWindow(argv[0]);
  init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(key);
  glutIdleFunc(update);
  glutMainLoop();

  return 0;  
}
