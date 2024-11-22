#pragma once
#include "polyhedron.h"
#include "GL/freeglut.h"

// Draws polyhedron with solid quads and lighting
void drawSolid(Polyhedron* poly)
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1., 2.);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	GLfloat mat_diffuse[4] = { 0.25, 0.3, 0.3, 0.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

	for (int i = 0; i < poly->nquads; i++) {
		Quad* temp_q = poly->qlist[i];
		glBegin(GL_POLYGON);
		for (int j = 0; j < 4; j++) {
			Vertex* temp_v = temp_q->verts[j];
			glNormal3d(temp_v->normal.entry[0], temp_v->normal.entry[1], temp_v->normal.entry[2]);
			glVertex3d(temp_v->x, temp_v->y, temp_v->z);
		}
		glEnd();
	}
}

// Draws polyhedron with quad outlines
void drawWirefriame(Polyhedron* poly)
{
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1.0);

	for (int i = 0; i < poly->nquads; i++) {
		Quad* temp_q = poly->qlist[i];

		glBegin(GL_POLYGON);
		for (int j = 0; j < 4; j++) {
			Vertex* temp_v = temp_q->verts[j];
			glNormal3d(temp_q->normal.entry[0], temp_q->normal.entry[1], temp_q->normal.entry[2]);
			glColor3f(0.0, 0.0, 0.0);
			glVertex3d(temp_v->x, temp_v->y, temp_v->z);
		}
		glEnd();
	}

	glDisable(GL_BLEND);
}

// Set polyhedron vertex colors to a checkerboard pattern
void initCheckerboard(Polyhedron* poly)
{
	double L = (poly->radius * 2) / 30;
	for (int i = 0; i < poly->nquads; i++) {
		Quad* temp_q = poly->qlist[i];
		for (int j = 0; j < 4; j++) {

			Vertex* temp_v = temp_q->verts[j];

			temp_v->R = int(temp_v->x / L) % 2 == 0 ? 1 : 0;
			temp_v->G = int(temp_v->y / L) % 2 == 0 ? 1 : 0;
			temp_v->B = 0.0;
		}
	}
}

// Draws polyhedron with solid quads and colors determined by vertices
void drawVertColors(Polyhedron* poly)
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1., 2.);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);

	glDisable(GL_LIGHTING);
	for (int i = 0; i < poly->nquads; i++) {
		Quad* temp_q = poly->qlist[i];
		glBegin(GL_POLYGON);
		for (int j = 0; j < 4; j++) {
			Vertex* temp_v = temp_q->verts[j];
			glColor3f(temp_v->R, temp_v->G, temp_v->B);
			glVertex3d(temp_v->x, temp_v->y, temp_v->z);
		}
		glEnd();
	}
}

// Displays polyhedron quads internally for mouse picking
void display_quads(GLenum mode, Polyhedron* this_poly)
{
	unsigned int i, j;

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);

	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	for (i = 0; i < this_poly->nquads; i++) {
		if (mode == GL_SELECT)
			glLoadName(i + 1);

		Quad* temp_q = this_poly->qlist[i];

		glBegin(GL_POLYGON);
		for (j = 0; j < 4; j++) {
			Vertex* temp_v = temp_q->verts[j];
			glVertex3d(temp_v->x, temp_v->y, temp_v->z);
		}
		glEnd();
	}
}

// Displays polyhedron vertices internally for mouse picking
void display_vertices(GLenum mode, Polyhedron* this_poly)
{
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	CHECK_GL_ERROR();

	for (int i = 0; i < this_poly->nverts; i++) {
		if (mode == GL_SELECT)
			glLoadName(i + 1);

		CHECK_GL_ERROR();

		Vertex* temp_v = this_poly->vlist[i];

		CHECK_GL_ERROR();

		{
			GLUquadric* quad = gluNewQuadric();
			CHECK_GL_ERROR();

			glPushMatrix();
			CHECK_GL_ERROR();
			glTranslatef(temp_v->x, temp_v->y, temp_v->z);
			glColor4f(0, 0, 1, 1.0);
			gluSphere(quad, this_poly->avg_edge_len * 0.2, 4, 2);
			glPopMatrix();
			CHECK_GL_ERROR();

			gluDeleteQuadric(quad);
			CHECK_GL_ERROR();

		}
		CHECK_GL_ERROR();
	}
	CHECK_GL_ERROR();
}

// Draws the selected quad for a polyhedron in solid red
void drawSelectedQuad(Polyhedron* this_poly)
{
	if (this_poly->selected_quad == -1)
		return;

	unsigned int i, j;
	GLfloat mat_diffuse[4];

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1., 1.);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);

	Quad* temp_q = this_poly->qlist[this_poly->selected_quad];

	glBegin(GL_POLYGON);
	for (j = 0; j < 4; j++) {
		Vertex* temp_v = temp_q->verts[j];
		glColor3f(1.0, 0.0, 0.0);
		glVertex3d(temp_v->x, temp_v->y, 0.001);
	}
	glEnd();
}

// Draws the selected vertex for a polyhedron as a red sphere
void drawSelectedVertex(Polyhedron* this_poly)
{
	if (this_poly->selected_vertex == -1)
		return;

	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glMatrixMode(GL_MODELVIEW);

	CHECK_GL_ERROR();

	Vertex* temp_v = this_poly->vlist[this_poly->selected_vertex];

	GLfloat mat_diffuse[4];
	mat_diffuse[0] = 1.0;
	mat_diffuse[1] = 0.0;
	mat_diffuse[2] = 0.0;
	mat_diffuse[3] = 1.0;
	
	CHECK_GL_ERROR();

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	CHECK_GL_ERROR();

	{
		GLUquadric* quad = gluNewQuadric();
		glPushMatrix();
		CHECK_GL_ERROR();
		glTranslated(temp_v->x, temp_v->y, temp_v->z);
		gluSphere(quad, this_poly->avg_edge_len * 0.2, 16, 8);
		glPopMatrix();
		CHECK_GL_ERROR();
		gluDeleteQuadric(quad);
	}

	CHECK_GL_ERROR();
}


