#pragma once

namespace Tmpl8 {
	struct float3;

	// 32-bit surface container
	class fSurface
	{
		enum { OWNER = 1 };
	public:
		// constructor / destructor
		fSurface() = default;
		fSurface(int w, int h);
		fSurface(const char* file);
		~fSurface();
		// operations
		void fLoadFromFile(const char* file);
		void InitCharset();
		void SetChar(int c, const char* c1, const char* c2, const char* c3, const char* c4, const char* c5);
		//void Print(const char* t, int x1, int y1, uint c);
		void fPrint(const char* t, int x1, int y1, float3 c, int scale);
		void fCenterPrint(const char* a_String, int y1, float3 c, int scale);
		//void Clear(uint c);
		void fClear(float3 c);
		//void Line(float x1, float y1, float x2, float y2, uint c);
		void fLine(float x1, float y1, float x2, float y2, float3 c);
		//void Plot(int x, int y, uint c);
		void fPlot(int x, int y, float3 c);
		void CopyTo(Surface* dst, int x, int y);
		void fCopyTo(fSurface* d, int x, int y);
		//void Box(int x1, int y1, int x2, int y2, uint color);
		void fBox(int x1, int y1, int x2, int y2, float3 c);
		//void Bar(int x1, int y1, int x2, int y2, uint color);
		void fBar( int x1, int y1, int x2, int y2, float3 c);
		void fCircle(int x1, int y1, int r, float3 c);
		//void FloatToUintBuffer();
		// attributes
		float3* pixels = 0;
		//float3 fpixels = 0;
		int width = 0, height = 0;
		bool ownBuffer = false;
	};

}

