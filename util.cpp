#include <GL/glew.h>
#include <GL/glut.h>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <FreeImage.h>
#include "util.h"
using namespace std;

bool operator <(const vector3 &left, const vector3 &right)
{
	if (fabs(left.x - right.x) >= 1e-6) return left.x < right.x;
	if (fabs(left.y - right.y) >= 1e-6) return left.y < right.y;
	if (fabs(left.z - right.z) >= 1e-6) return left.z < right.z;
	return false;
}
bool operator ==(const vector3 &left, const vector3 &right)
{
	return (fabs(left.x - right.x) < 1e-6) &&
			(fabs(left.y - right.y) < 1e-6) &&
			(fabs(left.z - right.z) < 1e-6);
}
bool operator !=(const vector3 &left, const vector3 &right)
{
	return !(left == right);
}
bool operator <(const material &left, const material &right)
{
	if (left.diffuse != right.diffuse) return left.diffuse < right.diffuse;
	if (left.ambient != right.ambient) return left.ambient < right.ambient;
	if (left.specular != right.specular) return left.specular < right.specular;
	if (fabs(left.shininess - right.shininess) >= 1e-6) return left.shininess < right.shininess;
	return false;
}
bool operator <(const shit &left, const shit &right)
{
	if (fabs(left.vf - right.vf) >= 1e-6) return left.vf < right.vf;
	if (fabs(left.tf - right.tf) >= 1e-6) return left.tf < right.tf;
	if (fabs(left.nf - right.nf) >= 1e-6) return left.nf < right.nf;
	return false;
}
void set_material(const material &mtl)
{
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &mtl.diffuse.x);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &mtl.specular.x);
	glMaterialfv(GL_FRONT, GL_AMBIENT, &mtl.ambient.x);
	glMaterialfv(GL_FRONT, GL_EMISSION, &mtl.emission.x);
	glMaterialf(GL_FRONT, GL_SHININESS, mtl.shininess);
}
map<string, material> load_mtl(const string &filename)
{
	map<string, material> result;
	ifstream file(filename);
	string line;
	string key;
	while (getline(file, line))
	{
		stringstream parser(line);
		string sel; parser >> sel;
		if (sel == "newmtl")
			parser >> key;
		else if (sel == "Ns")
			parser >> result[key].shininess;
		else if (sel == "Ka")
			parser >> result[key].ambient.x >> result[key].ambient.y >> result[key].ambient.z;
		else if (sel == "Kd")
			parser >> result[key].diffuse.x >> result[key].diffuse.y >> result[key].diffuse.z;
		else if (sel == "Ks")
			parser >> result[key].specular.x >> result[key].specular.y >> result[key].specular.z;
		else if (sel == "Ke")
			parser >> result[key].emission.x >> result[key].emission.y >> result[key].emission.z;
	}
	file.close();
	return result;
}
void load_obj(const string &filename, vector<vec12> &norvec, vector<unsigned int> &mtvec, vector<materials> &mats, unsigned int startindex)
{
	map<shit, unsigned int> normalmap;
	ifstream file(filename);
	string line;
	vector<vector3> v, n;
	vector<vector2> t;
	map<string, material> stuff;

	while (getline(file, line))
	{
		stringstream parser(line);
		string sel; parser >> sel;
		if (sel == "mtllib")
		{
			string filename2; parser >> filename2;
			stuff = load_mtl(filename2);
		}
		else if (sel == "v")
		{
			vector3 a;
			parser >> a.x >> a.y >> a.z;
			v.push_back(a);
		}
		else if (sel == "vn")
		{
			vector3 a;
			parser >> a.x >> a.y >> a.z;
			n.push_back(a);
		}
		else if (sel == "vt")
		{
			vector2 a;
			parser >> a.u >> a.v;
			t.push_back(a);
		}
		else if (sel == "usemtl")
		{
			string curmtl;
			parser >> curmtl;
			materials newstuff = {stuff[curmtl], startindex + mtvec.size(), 0};
			mats.push_back(newstuff);
		}
		else if (sel == "f")
		{
			char sep;
			shit a;
			int zz = 3;
			while (zz--)
			{
				bool hastex = true;
				parser >> a.vf >> sep;
				if (parser.peek() == '/')
					hastex = false;
				if (hastex)
					parser >> a.tf;
				else
					a.tf = 1;
				parser >> sep >> a.nf;
				a.vf--; a.tf--; a.nf--;
				mats.back().count++;
				if (normalmap.find(a) == normalmap.end())
				{
					normalmap[a] = startindex + norvec.size();
					vec12 insta;
					insta.v = v[a.vf];
					insta.n = n[a.nf];
					if (hastex)
						insta.t = t[a.tf];
					norvec.push_back(insta);
				}
				mtvec.push_back(normalmap[a]);
			}
		}
	}
	file.close();
}

// Method to load an image into a texture using the freeimageplus library. Returns the texture ID or dies trying.
GLuint loadTexture(string filenameString, GLenum minificationFilter, GLenum magnificationFilter)
{
    // Get the filename as a pointer to a const char array to play nice with FreeImage
    const char* filename = filenameString.c_str();

    // Determine the format of the image.
    // Note: The second paramter ('size') is currently unused, and we should use 0 for it.
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename , 0);

    // Image not found? Abort! Without this section we get a 0 by 0 image with 0 bits-per-pixel but we don't abort, which
    // you might find preferable to dumping the user back to the desktop.
    if (format == -1)
    {
        cout << "Could not find image: " << filenameString << " - Aborting." << endl;
        exit(-1);
    }

    // Found image, but couldn't determine the file format? Try again...
    if (format == FIF_UNKNOWN)
    {
        cout << "Couldn't determine file format - attempting to get from file extension..." << endl;

        // ...by getting the filetype from the filename extension (i.e. .PNG, .GIF etc.)
        // Note: This is slower and more error-prone that getting it from the file itself,
        // also, we can't use the 'U' (unicode) variant of this method as that's Windows only.
        format = FreeImage_GetFIFFromFilename(filename);

        // Check that the plugin has reading capabilities for this format (if it's FIF_UNKNOWN,
        // for example, then it won't have) - if we can't read the file, then we bail out =(
        if ( !FreeImage_FIFSupportsReading(format) )
        {
            cout << "Detected image format cannot be read!" << endl;
            exit(-1);
        }
    }

    // If we're here we have a known image format, so load the image into a bitap
    FIBITMAP* bitmap = FreeImage_Load(format, filename);

    int bitsPerPixel =  FreeImage_GetBPP(bitmap);

    FIBITMAP* bitmap32;
    if (bitsPerPixel == 32)
    {
        cout << "Source image has " << bitsPerPixel << " bits per pixel. Skipping conversion." << endl;
        bitmap32 = bitmap;
    }
    else
    {
        cout << "Source image has " << bitsPerPixel << " bits per pixel. Converting to 32-bit colour." << endl;
        bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
    }

    // Some basic image info - strip it out if you don't care
    int imageWidth  = FreeImage_GetWidth(bitmap32);
    int imageHeight = FreeImage_GetHeight(bitmap32);
    cout << "Image: " << filenameString << " is size: " << imageWidth << "x" << imageHeight << "." << endl;

    // Get a pointer to the texture data as an array of unsigned bytes.
    // Note: At this point bitmap32 ALWAYS holds a 32-bit colour version of our image - so we get our data from that.
    // Also, we don't need to delete or delete[] this textureData because it's not on the heap (so attempting to do
    // so will cause a crash) - just let it go out of scope and the memory will be returned to the stack.
    GLubyte* textureData = FreeImage_GetBits(bitmap32);

    // Generate a texture ID and bind to it
    GLuint tempTextureID;
    glGenTextures(1, &tempTextureID);
    glBindTexture(GL_TEXTURE_2D, tempTextureID);

    // Construct the texture.
    // Note: The 'Data format' is the format of the image data as provided by the image library. FreeImage decodes images into
    // BGR/BGRA format, but we want to work with it in the more common RGBA format, so we specify the 'Internal format' as such.
    glTexImage2D(GL_TEXTURE_2D,    // Type of texture
                 0,                // Mipmap level (0 being the top level i.e. full size)
                 GL_RGBA,          // Internal format
                 imageWidth,       // Width of the texture
                 imageHeight,      // Height of the texture,
                 0,                // Border in pixels
                 GL_BGRA,          // Data format
                 GL_UNSIGNED_BYTE, // Type of texture data
                 textureData);     // The image data to use for this texture

    // Specify our minification and magnification filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minificationFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnificationFilter);

    // If we're using MipMaps, then we'll generate them here.
    // Note: The glGenerateMipmap call requires OpenGL 3.0 as a minimum.
    if (minificationFilter == GL_LINEAR_MIPMAP_LINEAR   ||
        minificationFilter == GL_LINEAR_MIPMAP_NEAREST  ||
        minificationFilter == GL_NEAREST_MIPMAP_LINEAR  ||
        minificationFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Check for OpenGL texture creation errors
    GLenum glError = glGetError();
    if(glError)
    {
        cout << "There was an error loading the texture: "<< filenameString << endl;

        switch (glError)
        {
            case GL_INVALID_ENUM:
                cout << "Invalid enum." << endl;
                break;

            case GL_INVALID_VALUE:
                cout << "Invalid value." << endl;
                break;

            case GL_INVALID_OPERATION:
                cout << "Invalid operation." << endl;

            default:
                cout << "Unrecognised GLenum." << endl;
                break;
        }

        cout << "See https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml for further details." << endl;
    }

    // Unload the 32-bit colour bitmap
    FreeImage_Unload(bitmap32);

    // If we had to do a conversion to 32-bit colour, then unload the original
    // non-32-bit-colour version of the image data too. Otherwise, bitmap32 and
    // bitmap point at the same data, and that data's already been free'd, so
    // don't attempt to free it again! (or we'll crash).
    if (bitsPerPixel != 32)
    {
        FreeImage_Unload(bitmap);
    }

    // Finally, return the texture ID
    return tempTextureID;
}
