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

	int x = 5;
	int y = 10;
	int z = 12;

	x = 0xA;
	/*
		0xA == 1010

		in memory(or really more generally on the machine: virtual mem, hard disk, via os etc)
		we have for an int 4 bytes = 4*8bits groups.

		00000000 00000000 00000000 00001010

		Lets break it up into nibbles to help with hex (4bits => 1 hex digit)

		0000 0000     0000 0000    0000 0000    0000 1010

		x << 1 =>

		0000 0000     0000 0000    0000 0000    0001 0100
	0x     00             00           00          14
	dec                       20


		x << 1 =>

		0000 0000     0000 0000    0000 0000    0010 1000
	0x     00             00           00          28
	dec                       40


		x << 1 =>

		0000 0000     0000 0000    0000 0000    0101 0000
	0x     00             00           00         50 
	dec                      80 
		

		x << 1 =>

		0000 0000     0000 0000    0000 0000    1010 0000
	0x     00             00           00         a0 
	dec                      160 

	*/	
	x = x << 1;
	x = x << 1;
	x = x << 1;
	x = x << 1;
	x = x << 1;
	x = x << 1;
	x = x << 1;
	x = x << 1;
	x = x << 1;
	x = x << 1;

	x = 0;
	y = 1 << 4;

	// x does not have any bits set; y does, 1 bit set for down from the first bit (5th position):
	// so

	x = x | y;
	
	// will result in x == 0x10

	// so x now has its fifth bit set:

	// therefore:

	x = x | (1 << 8);   // note we could have assigned z = 1 << 8, but can do it directly: 1 << 8 == 000...0001 0000 0000.

	// will result in x having its fifth bit set AND its 9th bit. It already had its fifth bit set. Note the lack of 
	// short-circuiting : it doens't just stay the same - it sets the bit as described in the second operand

	// so we expect x to now be 0x110

	x = x | (1 << 8);

	// Now consider what happens with the bitwise &: x has bits set. 1 << 4 has bits set
	// x == 0x110
	// 1<<4 == 0x010
	
	x = x & (1 << 4);

	// I expect x then to be 0x010 : the only bit pattern both x and 1 << 4 have in common

	// So | is you take on both (operand) set bits; & is you only get the mutual set bits of the operands

	// & can be used for masking:
	// consider x == 0x11011101
	// and y ==      0x11111111
	// & will result in x:
	x = 0x11011101;
	y = 0x11111111;

	x = x & y;

	// | will result in y:

	x = x | y;

	// you could also zero a number:

	// reassigning x the same value as before, for clarity on opposite bits
	x = 0x11011101;
	y = 0x00100010;

	x = x & y; //x will == 0;

	// Not

	x = 0;
	x = ~x;

	//-- Exclusive OR

	x = (1 << 4) | (1 << 8); // x =  0x00000110
	y = (1 << 8) | (1 << 16); // y = 0x00010100

	z = x | y; // z = 0x00010110;

	int q = x & y; // q 0x00000100

	int r = x ^ y; // r = 0x00010010

	// cool property - inversion:

	/*
	Consider 01010,b

	then ^'ed with itself, is 00000 = zero

	consider x,b = 01010
			 y,b = 01001

	x^y = 00011

	and this result ^'ed with y:
	01010; i.e x again!

	i.e (a^b)^b = a.

	Drawing it out on a venn diagram (even in your head) will show this to be true.

	In words: a^b results in all of a and all of b, but for the common part of a and b.

	Then, taking this result and again applying ^b results in in all of a^b and all of b,
	but for any part in common: and the only part in common is the part of b without a.

	The above assumes an intersection between a and b, however it work if there is none:
	for then a^b is simply all of a and all of b, and then ^ with b again, only a remains.

	The special case, where a=b is clear using venn diagrams too - nothing is left (empty) as everything is in common (a with
	itself)
	*/

	//--

	//-- Logical operators

	x = 0; //False
	y = 1;// True
	z = 19423042;//True
	r = -349; // N.B have not yet done 2-complement yet, note!

	// Things are either zero or they are not (everything non-zero is true)

	// C does not have type bool!
	int less = (x < y); 
	int greater = (x > y);
	int equal = (x == y);
	int notequal = (x != y);

	less = (z < r); 
	greater = (z > r);
	equal = (z == r);
	notequal = (z != r);

	//! Compiler is under no obligation (in general) to produce a 1 for true - but check the spec
	// Again, C does not have type bool!

	// Loops

	// Would not really do this, but drives home the point of break
	while (1)
	{
		x = x << 4;

		if (x == 0)
		{
			break;
		}
	}

	for (x = 0xa;  // Once at beginning
		(x != 0);  // Condition
		x = x << 4) //The thing to do every iteration
	{
		// More stuff
		//OutputDebugStringA("We are in the loop!\n");
	}

	// this can be transformed into:

	x = 0xa; // Do once
	while (x != 0) // Condition check
	{
		OutputDebugStringA("We are in the loop!\n");
		x = x << 4; // do this every iteration
	}

	// Switch - when tired of if! Good when comaparing to constants

	x = 3;

	// instead of
	if (x == 0)
	{
		OutputDebugStringA("Case 0\n");
	}
	else if (x == 1)
	{
		OutputDebugStringA("Case 1\n");
	}
	else if (x == 2)
	{
		OutputDebugStringA("Case 2\n");
	}
	else if (x == 3)
	{
		OutputDebugStringA("Case 3\n");
	}
	else if (x == 4)
	{
		OutputDebugStringA("Case 4\n");
	}
	else if (x == 5)
	{
		OutputDebugStringA("Case 5\n");
	}
	else if (x == 6)
	{
		OutputDebugStringA("Case 6\n");
	}
	else
	{
		OutputDebugStringA("Case is unknown! \n");
	}

	// etc...

	// Can do

	switch (x)
	{
	case 0:
		OutputDebugStringA("Case 0\n");
	case 1:
		OutputDebugStringA("Case 1\n");
	case 2:
		OutputDebugStringA("Case 2\n");
	case 3:
		OutputDebugStringA("Case 3\n");
	case 4:
		OutputDebugStringA("Case 4\n");
	case 5:
		OutputDebugStringA("Case 5\n");
	case 6:
		OutputDebugStringA("Case 6\n");
	default:   // same as the else above, if nothing else is true
		OutputDebugStringA("Case is unknown! \n");

	}

	// NOTE the difference (look at output window) - switch only does the jumping for you - it will immediately jump
	// to the case that matches x - but then CONTINUE printing out the other cases!

	// this is where *break* comes in...
	switch (x)
	{
	case 0:
		OutputDebugStringA("Case 0\n");
		break;
	case 1:
		OutputDebugStringA("Case 1\n");
		break;
	case 2:
		OutputDebugStringA("Case 2\n");
		break;
	case 3:
		OutputDebugStringA("Case 3\n");
		break;
	case 4:
		OutputDebugStringA("Case 4\n");
		break;
	case 5:
		OutputDebugStringA("Case 5\n");
		break;
	case 6:
		OutputDebugStringA("Case 6\n");
		break;
	default:   // same as the else above, if nothing else is true
		OutputDebugStringA("Case is unknown! \n");
		break; //Probably don't need this!

	}

	//-- Blocks

	// Cool stuff coming up with pointers!

	int a = 5;
	int *ptr = 0;

	if (a == 5)
	{
		int b = 6;
		int a = 4;

		ptr = &b;
	}

	// Now b is out of scope
	// b = 4 //will not compile, for example

	// BUT. can do: (read: should not though!) - almost never any reason to do this - except maybe when hacking something??

	*ptr = 5;

	// Note however compilers are usaully under no obligation to keep that memory available - it _could_ use it
	// for something else, but in this case of nothing else happening in the program, it just keeps it there because
	// there is no reason yet to clear it, and it has no use for it yet - although there are some scenarios I could
	// think of where it could clear it, perhaps in multithreading scenarios?

	//-- Ptr to ptr

	int **ptrptr = 0;
	if (a == 5)
	{
		int b = 6;
		int a = 4;

		ptr = &b;
		ptrptr = &ptr;
	}

	int *tempPtr = *ptrptr; // expect the address of b
	a = **ptrptr;  //expect 6 (b was set to 6)


	// Check this out: shadowing and while loop
	// Will this loop end?

	while (a <= 5)
	{
		int *ptr_to_outside_a = &a;
		int a = 6;
		int *ptr_to_inside_a = &a;
		++a;
	}

	// No - the ++a refers to the a inside the block






}