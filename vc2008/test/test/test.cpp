#include <windows.h>
#include <stdio.h>

// Function definition
void foo()
{
	char* Foo = "This is the first thing we have actually printed.\n";
	OutputDebugStringA(
		"This is the first thing we have actually printed.\n");
	printf("This is a thing.\n");
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	int unsigned Test;
	Test = 500;
	Test = Test + 1;

	/*****************
		5
		0
		0

		1
		*/
	
	foo();
}