// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/UU - Jacco Bikker - 2006-2023

// common.h is to be included in host and device code and stores
// global settings and defines.

// default screen resolution (make sure it's a power of 2)
#define SCRWIDTH	1024
#define SCRHEIGHT	1024
//#define SCRWIDTH	1280
//#define SCRHEIGHT	720
// #define FULLSCREEN

// world size
#define WORLDWIDTH 8192
#define WORLDHEIGHT 8192

const float2 UPPER_LEFT_WORLD_POINT =  { -WORLDWIDTH / 2 , -WORLDHEIGHT / 2 };
const float2 UPPER_RIGHT_WORLD_POINT = {  WORLDWIDTH / 2 , -WORLDHEIGHT / 2 };
const float2 LOWER_LEFT_WORLD_POINT =  { -WORLDWIDTH / 2 ,  WORLDHEIGHT / 2 };
const float2 LOWER_RIGHT_WORLD_POINT = {  WORLDWIDTH / 2 ,  WORLDHEIGHT / 2 };

// constants
#define PI			3.14159265358979323846264f
#define INVPI		0.31830988618379067153777f
#define INV2PI		0.15915494309189533576888f
#define TWOPI		6.28318530717958647692528f
#define SQRT_PI_INV	0.56418958355f
#define LARGE_FLOAT	1e34f
#define DEG_TO_RAD(a) ((a) * PI / 180)
#define RAD_TO_DEG(a) ((a) * 180 / PI)

// IMPORTANT NOTE ON OPENCL COMPATIBILITY ON OLDER LAPTOPS:
// Without a GPU, a laptop needs at least a 'Broadwell' Intel CPU (5th gen, 2015):
// Intel's OpenCL implementation 'NEO' is not available on older devices.
// Same is true for Vulkan, OpenGL 4.0 and beyond, as well as DX11 and DX12.