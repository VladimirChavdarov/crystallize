#include "precomp.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "lib/stb_image.h"

using namespace Tmpl8;

// surface implementation
// ----------------------------------------------------------------------------

static char s_Font[51][5][6];
static bool fontInitialized = false;
static int s_Transl[256];

fSurface::fSurface(int w, int h) : width(w), height(h)
{
	pixels = (float3*)MALLOC64(w * h * sizeof(float3));
	ownBuffer = true; // needs to be deleted in destructor
}

fSurface::fSurface(const char* file) : pixels(0), width(0), height(0)
{
	FILE* f = fopen(file, "rb");
	if (!f) FatalError("File not found: %s", file);
	fclose(f);
	fSurface::fLoadFromFile(file);
}

fSurface::~fSurface()
{
	if (ownBuffer)
	{
		// free only if we allocated the buffer ourselves
		FREE64(pixels);
	}
}

void fSurface::fLoadFromFile(const char* file)
{
	int n;
	unsigned char* data = stbi_load(file, &width, &height, &n, 0);
	if (data)
	{
		pixels = (float3*)MALLOC64(width * height * sizeof(float3));
		ownBuffer = true; // needs to be deleted in destructor
		const int s = width * height;
		if (n == 1) // greyscale
		{
			/*for (int i = 0; i < s; i++)
			{
				const unsigned char p = data[i];
				pixels[i] = p + (p << 8) + (p << 16);
			}*/
		}
		else
		{
			for (int i = 0; i < s; i++)
			{
				
				//uint a = (data[i * n + 0] << 16) + (data[i * n + 1] << 8) + data[i * n + 2];
				pixels[i] = float3( data[i * n + 0] / 255.0f, data[i * n + 1] / 255.0f, data[i * n + 2] / 255.0f );
			}
			
		}
	}
	stbi_image_free(data);
}

void fSurface::fClear(float3 c)
{
	const int s = width * height;
	for (int i = 0; i < s; i++)
	{
		pixels[i] = c;
	}
}

void fSurface::fPlot(int x, int y, float3 c)
{
	if (x < 0 || y < 0 || x >= width || y >= height) return;
	pixels[x + y * width] = c;
}

void fSurface::fBox(int x1, int y1, int x2, int y2, float3 c)
{
	fLine((float)x1, (float)y1, (float)x2, (float)y1, c);
	fLine((float)x2, (float)y1, (float)x2, (float)y2, c);
	fLine((float)x1, (float)y2, (float)x2, (float)y2, c);
	fLine((float)x1, (float)y1, (float)x1, (float)y2, c);
}

void fSurface::fBar(int x1, int y1, int x2, int y2, float3 c)
{
	// clipping
	if (x1 < 0) x1 = 0;
	if (x2 >= width) x2 = width - 1;
	if (y1 < 0) y1 = 0;
	if (y2 >= height) y2 = width - 1;
	// draw clipped bar
	float3* a = x1 + y1 * width + pixels;
	for (int y = y1; y <= y2; y++)
	{
		for (int x = 0; x <= (x2 - x1); x++) a[x] = c;
		a += width;
	}
}

void fSurface::fPrint(const char* s, int x1, int y1, float3 c, int scale)
{
	if (!fontInitialized)
	{
		InitCharset();
		fontInitialized = true;
	}
	float3* t = pixels + x1 + y1 * width;
	for (int i = 0; i < (int)(strlen(s)); i++, t += 6 * scale)
	{
		int pos = 0;
		if ((s[i] >= 'A') && (s[i] <= 'Z')) pos = s_Transl[(unsigned short)(s[i] - ('A' - 'a'))];
		else pos = s_Transl[(unsigned short)s[i]];
		float3* a = t;
		const char* u = (const char*)s_Font[pos];
		for (int v = 0; v < 5; v++, u++, a += width * scale)
			for (int h = 0; h < 5 * scale; h += scale)
				if (*u++ == 'o') /**(a + h) = c, * (a + h + width) = 0;*/
				{
					for (int w = 0; w < scale; w++)
						for (int j = 0; j < scale; j++)
							a[w + h + j * width] = c, a[w + h + (j + 1) * width] = 0;
				}
	}
}

void fSurface::fCenterPrint(const char* a_String, int y1, float3 c, int scale)
{
	int x = (width - (int)strlen(a_String) * 6 * scale) / 2;
	fPrint(a_String, x, y1, c, scale);
}

#define OUTCODE(x,y) (((x)<xmin)?1:(((x)>xmax)?2:0))+(((y)<ymin)?4:(((y)>ymax)?8:0))

void fSurface::fLine(float x1, float y1, float x2, float y2, float3 c)
{
	// clip (Cohen-Sutherland, https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm)
	const float xmin = 0, ymin = 0, xmax = (float)width - 1, ymax = (float)height - 1;
	int c0 = OUTCODE(x1, y1), c1 = OUTCODE(x2, y2);
	bool accept = false;
	while (1)
	{
		if (!(c0 | c1)) { accept = true; break; }
		else if (c0 & c1) break; else
		{
			float x = 0.0f, y = 0.0f;
			const int co = c0 ? c0 : c1;
			if (co & 8) x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1), y = ymax;
			else if (co & 4) x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1), y = ymin;
			else if (co & 2) y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1), x = xmax;
			else if (co & 1) y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1), x = xmin;
			if (co == c0) x1 = x, y1 = y, c0 = OUTCODE(x1, y1);
			else x2 = x, y2 = y, c1 = OUTCODE(x2, y2);
		}
	}
	if (!accept) return;
	float b = x2 - x1;
	float h = y2 - y1;
	float l = fabsf(b);
	if (fabsf(h) > l) l = fabsf(h);
	int il = (int)l;
	float dx = b / (float)l;
	float dy = h / (float)l;
	for (int i = 0; i <= il; i++)
	{
		*(pixels + (int)x1 + (int)y1 * width) = c;
		x1 += dx, y1 += dy;
	}
}

//void fSurface::FloatToUintBuffer()
//{
//	for (int i = 0; i < width * height; i++)
//	{
//
//		pixels[i] = ConvertFloatToRGB(fpixels[i]);
//	}
//}

void fSurface::CopyTo(Surface* d, int x, int y)
{
	uint* dst = d->pixels;
	float3* src = pixels;
	if ((src) && (dst))
	{
		int srcwidth = width;
		int srcheight = height;
		int dstwidth = d->width;
		int dstheight = d->height;
		if ((srcwidth + x) > dstwidth) srcwidth = dstwidth - x;
		if ((srcheight + y) > dstheight) srcheight = dstheight - y;
		if (x < 0) src -= x, srcwidth += x, x = 0;
		if (y < 0) src -= y * srcwidth, srcheight += y, y = 0;
		if ((srcwidth > 0) && (srcheight > 0))
		{
			for (int y = 0; y < srcheight; y++)
			{
				for (int x = 0; x < dstwidth; x++, dst++, src++)
				{
					float3 p = *src;
					*dst = ConvertFloatToRGB(p);
				}
			}
		}
	}
}

void fSurface::fCopyTo(fSurface* d, int x, int y)
{
	float3* dst = d->pixels;
	float3* src = pixels;
	if ((src) && (dst))
	{
		int srcwidth = width;
		int srcheight = height;
		int dstwidth = d->width;
		int dstheight = d->height;
		if ((srcwidth + x) > dstwidth) srcwidth = dstwidth - x;
		if ((srcheight + y) > dstheight) srcheight = dstheight - y;
		if (x < 0) src -= x, srcwidth += x, x = 0;
		if (y < 0) src -= y * srcwidth, srcheight += y, y = 0;
		if ((srcwidth > 0) && (srcheight > 0))
		{
			dst += x + dstwidth * y;
			for (int y = 0; y < srcheight; y++)
			{
				memcpy(dst, src, srcwidth * 12);
				dst += dstwidth, src += width;
			}
		}
	}
}

void fSurface::SetChar(int c, const char* c1, const char* c2, const char* c3, const char* c4, const char* c5)
{
	strcpy(s_Font[c][0], c1);
	strcpy(s_Font[c][1], c2);
	strcpy(s_Font[c][2], c3);
	strcpy(s_Font[c][3], c4);
	strcpy(s_Font[c][4], c5);
}

void fSurface::InitCharset()
{
	SetChar(0, ":ooo:", "o:::o", "ooooo", "o:::o", "o:::o");
	SetChar(1, "oooo:", "o:::o", "oooo:", "o:::o", "oooo:");
	SetChar(2, ":oooo", "o::::", "o::::", "o::::", ":oooo");
	SetChar(3, "oooo:", "o:::o", "o:::o", "o:::o", "oooo:");
	SetChar(4, "ooooo", "o::::", "oooo:", "o::::", "ooooo");
	SetChar(5, "ooooo", "o::::", "ooo::", "o::::", "o::::");
	SetChar(6, ":oooo", "o::::", "o:ooo", "o:::o", ":ooo:");
	SetChar(7, "o:::o", "o:::o", "ooooo", "o:::o", "o:::o");
	SetChar(8, "::o::", "::o::", "::o::", "::o::", "::o::");
	SetChar(9, ":::o:", ":::o:", ":::o:", ":::o:", "ooo::");
	SetChar(10, "o::o:", "o:o::", "oo:::", "o:o::", "o::o:");
	SetChar(11, "o::::", "o::::", "o::::", "o::::", "ooooo");
	SetChar(12, "oo:o:", "o:o:o", "o:o:o", "o:::o", "o:::o");
	SetChar(13, "o:::o", "oo::o", "o:o:o", "o::oo", "o:::o");
	SetChar(14, ":ooo:", "o:::o", "o:::o", "o:::o", ":ooo:");
	SetChar(15, "oooo:", "o:::o", "oooo:", "o::::", "o::::");
	SetChar(16, ":ooo:", "o:::o", "o:::o", "o::oo", ":oooo");
	SetChar(17, "oooo:", "o:::o", "oooo:", "o:o::", "o::o:");
	SetChar(18, ":oooo", "o::::", ":ooo:", "::::o", "oooo:");
	SetChar(19, "ooooo", "::o::", "::o::", "::o::", "::o::");
	SetChar(20, "o:::o", "o:::o", "o:::o", "o:::o", ":oooo");
	SetChar(21, "o:::o", "o:::o", ":o:o:", ":o:o:", "::o::");
	SetChar(22, "o:::o", "o:::o", "o:o:o", "o:o:o", ":o:o:");
	SetChar(23, "o:::o", ":o:o:", "::o::", ":o:o:", "o:::o");
	SetChar(24, "o:::o", "o:::o", ":oooo", "::::o", ":ooo:");
	SetChar(25, "ooooo", ":::o:", "::o::", ":o:::", "ooooo");
	SetChar(26, ":ooo:", "o::oo", "o:o:o", "oo::o", ":ooo:");
	SetChar(27, "::o::", ":oo::", "::o::", "::o::", ":ooo:");
	SetChar(28, ":ooo:", "o:::o", "::oo:", ":o:::", "ooooo");
	SetChar(29, "oooo:", "::::o", "::oo:", "::::o", "oooo:");
	SetChar(30, "o::::", "o::o:", "ooooo", ":::o:", ":::o:");
	SetChar(31, "ooooo", "o::::", "oooo:", "::::o", "oooo:");
	SetChar(32, ":oooo", "o::::", "oooo:", "o:::o", ":ooo:");
	SetChar(33, "ooooo", "::::o", ":::o:", "::o::", "::o::");
	SetChar(34, ":ooo:", "o:::o", ":ooo:", "o:::o", ":ooo:");
	SetChar(35, ":ooo:", "o:::o", ":oooo", "::::o", ":ooo:");
	SetChar(36, "::o::", "::o::", "::o::", ":::::", "::o::");
	SetChar(37, ":ooo:", "::::o", ":::o:", ":::::", "::o::");
	SetChar(38, ":::::", ":::::", "::o::", ":::::", "::o::");
	SetChar(39, ":::::", ":::::", ":ooo:", ":::::", ":ooo:");
	SetChar(40, ":::::", ":::::", ":::::", ":::o:", "::o::");
	SetChar(41, ":::::", ":::::", ":::::", ":::::", "::o::");
	SetChar(42, ":::::", ":::::", ":ooo:", ":::::", ":::::");
	SetChar(43, ":::o:", "::o::", "::o::", "::o::", ":::o:");
	SetChar(44, "::o::", ":::o:", ":::o:", ":::o:", "::o::");
	SetChar(45, ":::::", ":::::", ":::::", ":::::", ":::::");
	SetChar(46, "ooooo", "ooooo", "ooooo", "ooooo", "ooooo");
	SetChar(47, "::o::", "::o::", ":::::", ":::::", ":::::"); // Tnx Ferry
	SetChar(48, "o:o:o", ":ooo:", "ooooo", ":ooo:", "o:o:o");
	SetChar(49, "::::o", ":::o:", "::o::", ":o:::", "o::::");
	char c[] = "abcdefghijklmnopqrstuvwxyz0123456789!?:=,.-() #'*/";
	int i;
	for (i = 0; i < 256; i++) s_Transl[i] = 45;
	for (i = 0; i < 50; i++) s_Transl[(unsigned char)c[i]] = i;
}