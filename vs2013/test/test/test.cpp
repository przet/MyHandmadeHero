#include <windows.h>
#include <stdio.h>

//#pragma pack(1) : force boundary size (tight/loose packing)
struct projectile
{
	//char unsigned IsThisOnFire;
	int Damage;
	int ParticlesPerSecond;
	short HowManyCooks;
	char unsigned IsThisOnFire;
	short Short1;
	//int Int1;
	// ^ order does matter - will place in order in memory

	// tightly compact => 11bytes. Optimized for CPU will be 16 bytes
	// note if IsThisOnFire is at the _end_, size of projectile is 12 bytes
	// Fascinating...the short is given 2 bytes..and so is the char?...TODO!
	// Casey actually address this about 7:48 on day 4 Q&A! It dependns on what follows.
	// So here with, the char at the end we have:
	// 4 + 4...okay next is 2, thats fine, I (compiler) will give it 2...next is 1,
	// but I will give it 2, cause I will be out of alignmnent...BUT tbh I don't know why
	// it couldn't just give it 1....maybe if its int/short/int...etc (alternating), then might 
	// make sense...but not sure why here.
	// Although, casey shows that if ifs short/char/short/char, the size is 8:
	// 2 + 2 + 2+ 2...
	// And then if you sneak in another char at the end, the size will not chaange - will fit in to the extra byte
	// after the preceding char
};

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
	)
{
	projectile Test;

	int SizeOfProjectile = sizeof(projectile);
	
	Test.IsThisOnFire = 1;
	Test.Damage = 1 + Test.IsThisOnFire;
	Test.ParticlesPerSecond = 3;
	Test.HowManyCooks = 4;
	Test.Short1 = 56;
	//Test.Int1 = 56;
	

	projectile *ProjectilePointer = &Test;
	short unsigned  *MrPointerMan = (short unsigned *)&Test;

	projectile Projectiles[40];
	projectile *Projectiles_ARR_Pointer = Projectiles;

	int SizeOfProjectiles = sizeof(Projectiles); //16*40
	int SizeOfProjectiles_ARR_Pointer = sizeof(Projectiles_ARR_Pointer); //4

	Projectiles[30].Damage = 60;

	//-- Adding this the ultra clarity: will be uninitialised and appear as -52(signed) or 204(unsigned), but clearer is even better
	Projectiles[30].IsThisOnFire = 1;
	
	// For when we just point to the first byte
	Projectiles[0].IsThisOnFire = 2;
	//---

	// Same as:
	(Projectiles_ARR_Pointer + 30)->Damage = 100;
	// Or:
	(*(Projectiles_ARR_Pointer + 30)).Damage = 110;

	// Note you do NOT do this: won't be what you expect: C understands +30 as 30*sizeof(...) already;
	// It will still 'work' (if the array is big enough - otherwise access violation) but will be much
	// further along!
	// Lets test this:
	// This will adjust the 30th value (2*16 -2 = 30 :
	(Projectiles_ARR_Pointer + 2 * sizeof(projectile) -2)->Damage = 111222333;

	// To use the sizeof you need to cast Projectile_Arr_Pointer to a char * (point to char; 1 byte) then add 30*sizeof(projectile)
	((projectile *)((char *)Projectiles_ARR_Pointer + 30 * sizeof(projectile)))->Damage = 120;

	//-- Broken down:
	char *BytePointer = (char *)Projectiles_ARR_Pointer;
	BytePointer = BytePointer + 30 * sizeof(projectile);

	//BUT NOW:Can't do:
	// BytePointer->Damage = 100 : BytePointer is _not_ a pointer to Projectile_ARR_Pointer, only a pointer to 8 bits (however
	// it does point to the first 8 bits (1st byte) of Projectiles (the array) : on LITTLE ENDIAN only though! see <15min day 4 Q&A and some below...

	// So need to do:
	projectile *Thirty = (projectile *)BytePointer;
	Thirty->Damage = 130;
	//--

	//-- Small vs Big Endian
	int X = 16;
	short Y = (short)X;
	char Z = (char)X;
	//all expected to have value 16 on both L and B endian

	int *XPtr = &X;
	short *YPtr = (short *)&X;
	// on L endian, both will point to same address as we are lobbing of the end, high bytes.
	// But on B endian the high buts are stored first...so XPtr and YPtr will NOT point to the same address.
	// X: 16 0 0 0. Big endian, &X will point to the last byte (the value 0). TODO: verify on a big endian emulation
	// .. or my PS3!!
	//--






}