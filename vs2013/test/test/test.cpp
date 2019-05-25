#include <windows.h>
#include <stdio.h>

//#pragma pack(1) : force boundary size (tight/loose packing)
struct projectile
{
	char unsigned IsThisOnFire;
	int Damage;
	int ParticlesPerSecond;
	short HowManyCooks;

	// tightly compact => 11bytes. Optimized for CPU will be 16 bytes
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
	// it does point to the first 8 bits (1st byte) of Projectiles (the array).

	// So need to do:
	projectile *Thirty = (projectile *)BytePointer;
	Thirty->Damage = 130;
	//--


}