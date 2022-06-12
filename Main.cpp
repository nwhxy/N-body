#include "Display.h"
#include<iostream>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include "nbody.cuh"
#include "Timer.h"
using namespace std;
#define WIDTH 1280
#define HEIGHT 720
bool fullscreen = false;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")

int main()
{
	int num;
	int scale;
	cout << "请输入规模大小：";
	cin >> scale;
	num = scale*1024;
	if (scale==0)num = 3;
	Display display = Display(WIDTH, HEIGHT, fullscreen);
	display.initShaders();
	
	size_t size = num * sizeof(GLfloat) * 3;

	GLfloat* points = (GLfloat*) malloc(size);
	GLfloat* vels = (GLfloat*) malloc(size);
	GLfloat* accel = (GLfloat*) malloc(size);
	GLfloat* masses = (GLfloat*)malloc(num * sizeof(GLfloat));

	srand(time(0));

	for(int i = 0; i < num; i++)
	{
		int x = i*3;
		int y = i*3 +1;
		int z = i*3 +2;

		points[x] = (float)(rand() % 10000) / 1000.0f -5.0f;
		points[y] = (float)(rand() % 10000) / 10000.0f -5.0f;
		points[z] = (float)(rand() % 10000) / 1000.0f -5.0f;

		vels[i*3] = 0;
		vels[i*3+1] = 0;
		vels[i*3+2] = 0;

		vels[x]/= 100.0f;
		vels[z]/=100.0f;

		masses[i] = 100000.0f + (float)(rand() % 1000000);

	}


	display.setVertexData(points, vels, accel, masses, num);

	float3* p = 0;
	float3* v = 0;
	float3* a = 0;
	float* m = 0;

	display.registerCUDA();

	Timer timer = Timer(); 

	while(display.running()) 
	{
		timer.start();

		if(!display.paused())
		{
			display.getDevicePointers(&p, &v, &a, &m);
			if (num<1024*6)runPhysics(p, v, a, m, display.getNumPoints(), false, 1.0f);
			else runPhysics(p, v, a, m, display.getNumPoints(), true, 1.0f);
			display.unmapCUDAResources();
		}
		display.render();
		timer.stop();
	}

	display.unregisterCUDA();

	return 0;
}