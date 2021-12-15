/*
 * author: Kardelen Erdal
 * This is the .x file that will generate all the .c files automatically.
*/

struct numbers{
	int a;
	int b;
	char pathOfBlackbox[500];
};

program BLACKBOX_PROG{
	version BLACKBOX_VERS{
		/* Takes a numbers structure and gives the string result. */
		string blackbox(numbers)=1;
	}=1;
}=0x12345678;