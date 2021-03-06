#pragma once
#include "vector.h"
#include "matrix.h"
#include "quaternion.h"
#include "GL\glew.h"
#include "GL\freeglut.h"

#define SIZE_RATIO 1

class GameObject
{
public:
	vec3* vertices;
	GLuint numVertices;
	vec3* normals;
	GLuint numNormals;
	vec2* texCoords;
	GLuint numTexCoords;
	bool normalsLoaded, texCoordsLoaded;
	MeshLoader* myMeshLoader;

	qtrn all_rotation[4];
	mat4 all_translation[4];
	mat4 all_scale[4];
	mat4 all_transformation = mat4::identityMatrix();
	qtrn self_rotation[4];
	mat4 self_translation[4];
	mat4 self_scale[4];
	mat4 self_transformation = mat4::identityMatrix();
	GLuint VaoId, VboId[4];
	mat4 **ViewMatrix, **ProjectionMatrix;

	bool canAnimate;
	float animationState = 0.0f;
	int animationDirection = 0;

	GameObject();
	~GameObject();
	mat4* GameObject::draw(GLint ModelMatrix_UId, GLint ViewMatrix_UId, GLint ProjectionMatrix_UId, mat4* parentMatrix);
	void rotateAll(qtrn rotation, int state);
	void scaleAll(mat4 scale, int state);
	void translateAll(mat4 translation, int state);
	void rotateSelf(qtrn rotation, int state);
	void scaleSelf(mat4 scale, int state);
	void translateSelf(mat4 translation, int state);
	GLsizeiptr vertexBufferSize() const;
	GLsizeiptr texCoordsBufferSize() const;
	GLsizeiptr normalBufferSize() const;
	void init(const GLuint UBO_BP);
	void runAnimation(int milliseconds);
	void animate(); //toggles animation direction

	static GameObject loadMesh(const std::string& filename);
};


//Copied from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
//Will redo later for sanity
typedef struct {
	double r;       // a fraction between 0 and 1
	double g;       // a fraction between 0 and 1
	double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
	hsv         out;
	double      min, max, delta;

	min = in.r < in.g ? in.r : in.g;
	min = min < in.b ? min : in.b;

	max = in.r > in.g ? in.r : in.g;
	max = max > in.b ? max : in.b;

	out.v = max;                                // v
	delta = max - min;
	if (delta < 0.00001)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, h is undefined
		out.s = 0.0;
		out.h = NAN;                            // its now undefined
		return out;
	}
	if (in.r >= max)                           // > is bogus, just keeps compilor happy
		out.h = (in.g - in.b) / delta;        // between yellow & magenta
	else
		if (in.g >= max)
			out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if (out.h < 0.0)
		out.h += 360.0;

	return out;
}


rgb hsv2rgb(hsv in)
{
	double      hh, p, q, t, ff;
	long        i;
	rgb         out;

	if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch (i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}

