//Class used to Rapid-I Font.. Different font types..

#include <string>
#include <map>
#include <list>
#include "..\RapidGraphics.h"

using namespace std;
class RapidGraphics_API RGraphicsFont
{
private:
	int RFont_Size; //Holds the size of the font.
	std::string RFontName;
	bool RFont_bold, RFont_Italic, RFont_Underline;

	GLuint* RFont_textures;	//Holds the texture id's 
	GLuint RFont_listbase;	//Holds the first display list id
	
	int next_powerof2(int val);
	void GenerateTextureFromImage(BYTE* bpdata, int bpwidth, int bpheight, int listcount, GLuint list_base, GLuint* tex_base);

	void pushScreenCoordinateMatrix();
	void pop_projection_matrix();
public:
	RGraphicsFont(); //Constructor..
	~RGraphicsFont(); //Destructor..

	//Error Handling
	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
	void (*RGraphicsFontError) (char* Graphicserrorcode, char* GraphicsFileName, char* GraphicsFunName);

	//Create / recreate Font texture list
	void Initialise_RFontlist(list<BYTE*>* rfontlist_bytearray, list<int>* rfont_bytewidth, list<int>* rfont_byteheight, char* rfont_name, int rfont_size, bool rfont_bold, bool rfont_italic, bool rfont_underline);
	//Clear cureent font list.
	void DeleteCurrent_RFontlist();
	//draw current text..
	void drawcurrent_Rfont(double PosX, double PosY, double PosZ, double r, double g, double b, char *string); 
};


//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!