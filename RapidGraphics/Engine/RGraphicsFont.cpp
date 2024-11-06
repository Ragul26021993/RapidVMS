//Include the header files...//
#include "stdafx.h"
#include "RGraphicsFont.h"

#define FONTLIST 255

RGraphicsFont::RGraphicsFont()
{
	RGraphicsFontError = NULL;
}

RGraphicsFont::~RGraphicsFont()
{
	try
	{
		//glDeleteLists(RFont_listbase, 128);
		//glDeleteTextures(128, RFont_textures);
		//delete[] RFont_textures;
	}
	catch(...){ SetAndRaiseErrorMessage("RFONTDLL0002", __FILE__, __FUNCSIG__); }
}

void RGraphicsFont::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	if(RGraphicsFontError != NULL)
		RGraphicsFontError((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
}

int RGraphicsFont::next_powerof2(int val)
{
	try
	{
		int rval = 1;
		while(rval < val) rval <<= 1;
		return rval;
	}
	catch(...){ SetAndRaiseErrorMessage("RFONTDLL0003", __FILE__, __FUNCSIG__); return 2;}
}

void RGraphicsFont::GenerateTextureFromImage(BYTE* bpdata, int bpwidth, int bpheight, int listcount, GLuint list_base, GLuint* tex_base)
{
	try
	{
		int width = next_powerof2(bpwidth);
		int height = next_powerof2(bpheight);
		int Step = 4;
		GLubyte* expanded_data = new GLubyte[Step * width * height];
		
		for(int j = 0; j < height; j++) 
		{
			for(int i = 0; i < width; i++)
			{
				if(i >= bpwidth || j >= bpheight)
				{
					for(int k = 0; k < Step; k++)
						expanded_data[j * width * Step + i * Step + k] = 0;
				}
				else
				{
					for(int k = 0; k < Step; k++)
						expanded_data[j * width * Step + i * Step + k] = bpdata[j * bpwidth * Step + i * Step + k];
				}
			}
		}
		float x = (float)bpwidth / (float)width,
			y = (float)bpheight / (float)height;
		
		glNewList(list_base + listcount, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, tex_base[listcount]); // Set texture handle as current
		glTexImage2D(GL_TEXTURE_2D, 0, Step, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, expanded_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glPushMatrix();
		glScaled(x, y, 1);
		glEnable(GL_TEXTURE_2D);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glEnable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, tex_base[listcount]);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex2f(0, bpheight);
		glTexCoord2d(0, y); glVertex2f(0, 0);
		glTexCoord2d(x, y); glVertex2f(bpwidth, 0);
		glTexCoord2d(x, 0); glVertex2f(bpwidth, bpheight);
		glEnd();
		//glEnable(GL_DEPTH_TEST);
		//glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		glEndList();
		delete[] expanded_data;
	}
	catch(...){ SetAndRaiseErrorMessage("RFONTDLL0004", __FILE__, __FUNCSIG__); }
}

//Create / recreate Font texture list
void RGraphicsFont::Initialise_RFontlist(list<BYTE*>* rfontlist_bytearray, list<int>* rfont_bytewidth, list<int>* rfont_byteheight, char* rfont_name, int rfont_size, bool rfont_bold, bool rfont_italic, bool rfont_underline)
{
	try
	{
		RFontName = rfont_name;
		RFont_Size = rfont_size;
		RFont_bold = rfont_bold;
		RFont_Italic = rfont_italic;
		RFont_Underline = rfont_underline;
		
		int listnumbers = rfontlist_bytearray->size();

		RFont_textures = new GLuint[listnumbers];
		RFont_listbase = glGenLists(listnumbers);
		glGenTextures(listnumbers, RFont_textures);
		list<BYTE*>::iterator fontlist_itr = rfontlist_bytearray->begin();
		list<int>::iterator width_itr = rfont_bytewidth->begin();
		list<int>::iterator height_itr = rfont_byteheight->begin();
		for(int i = 0; i < listnumbers; i++)
		{
			BYTE* current_byteArray = (*fontlist_itr);
			int rwidth = (*width_itr);
			int rheight = (*height_itr);
			fontlist_itr++;
			width_itr++;
			height_itr++;
			GenerateTextureFromImage(current_byteArray, rwidth, rheight, i, RFont_listbase, RFont_textures);
		}
		while(rfontlist_bytearray->size() != 0)
		{
			fontlist_itr = rfontlist_bytearray->begin();
			BYTE* CbyteArray = *(fontlist_itr);
			rfontlist_bytearray->remove(CbyteArray);
			delete[] CbyteArray;
		}

	}
	catch(...){ SetAndRaiseErrorMessage("RFONTDLL0005", __FILE__, __FUNCSIG__); }
}

//Clear cureent font list.
void RGraphicsFont::DeleteCurrent_RFontlist()
{
	try
	{
		/*glDeleteLists(RFont_listbase, 128);
		glDeleteTextures(128, RFont_textures);
		delete[] RFont_textures;*/
	}
	catch(...){ SetAndRaiseErrorMessage("RFONTDLL0006", __FILE__, __FUNCSIG__); }
}

//draw current text..
void RGraphicsFont::drawcurrent_Rfont(double PosX, double PosY, double PosZ, double r, double g, double b, char *string)
{
	try
	{
		//return;
		//pushScreenCoordinateMatrix();
		
		std::string lines = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz";
	
	
		//glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);	
		//glMatrixMode(GL_MODELVIEW);
		//glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		/*glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		*/
		//glColor3d(1, 1, 0);
		glListBase(RFont_listbase);

		glPushMatrix();
		glTranslatef(PosX, PosY, PosZ);
		glScaled(1/100, 1/100, 1);
		glCallList(RFont_listbase);

		//glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, lines.c_str());
		glPopMatrix();

		/*glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);*/
		glDisable(GL_TEXTURE_2D);
		//glPopAttrib();		

		//pop_projection_matrix();

		/*glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();*/
	}
	catch(...){ SetAndRaiseErrorMessage("RFONTDLL0007", __FILE__, __FUNCSIG__); }
}


//A fairly straight forward function that pushes a projection matrix that will make object world coordinates identical to window coordinates.
void RGraphicsFont::pushScreenCoordinateMatrix() 
{
	try
	{
		glPushAttrib(GL_TRANSFORM_BIT);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
		glPopAttrib();
	}
	catch(...){ SetAndRaiseErrorMessage("RFONTDLL0008", __FILE__, __FUNCSIG__); }
}

//Pops the projection matrix without changing the current MatrixMode.
void RGraphicsFont::pop_projection_matrix() 
{
	try
	{
		glPushAttrib(GL_TRANSFORM_BIT);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();
	}
	catch(...){ SetAndRaiseErrorMessage("RFONTDLL0009", __FILE__, __FUNCSIG__); }
}