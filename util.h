#ifndef _UTIL_H_INCLUDED
#define _UTIL_H_INCLUDED

#include <string>
#include <vector>
#include <map>
using namespace std;
struct vector3
{
	float x, y, z;
};
bool operator <(const vector3 &left, const vector3 &right);
bool operator ==(const vector3 &left, const vector3 &right);
bool operator !=(const vector3 &left, const vector3 &right);
struct vector2
{
	float u, v;
};
struct vec12
{
	vector3 v, n;
	vector2 t;
};
struct shit
{
	unsigned int vf, tf, nf;
};
struct material
{
	vector3 diffuse, ambient, specular, emission;
	float shininess;
};
bool operator <(const material &left, const material &right);
struct materials
{
	material mtl;
	unsigned int startindex;
	unsigned int count;
};
bool operator <(const shit &left, const shit &right);
void set_material(const material &mtl);
map<string, material> load_mtl(const string &filename);
void load_obj(const string &filename, vector<vec12> &norvec, vector<unsigned int> &mtvec, vector<materials> &mats, unsigned int startindex = 0);

// Method to load an image into a texture using the freeimageplus library. Returns the texture ID or dies trying.
GLuint loadTexture(string filenameString, GLenum minificationFilter = GL_LINEAR, GLenum magnificationFilter = GL_LINEAR);
#endif
