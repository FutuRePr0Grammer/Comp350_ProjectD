//kernel.c

//Joao Alves and David Rochon

void printString(char*);
void printChar(char c);
void readString(char*);
void readSector(char* buffer, int sector);
void writeSector(char* buffer, int sector);
void readFile(char* filename, char* buffer2, int* sectorsRead);
void writeFile(char* buffer, char* filename, int numberOfSectors);
void executeProgram(char* name);
void deleteFile(char* filename);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void terminate();

//hello Joao!


//char line[100];
//char buffer[512];

void main()
{
	int startVidMem = 0xb800;
	int vidMemOffSet = 0x0;
	int white = 0x7;
	char* letters = "Hello World\0";

	char line[100];
	char buffer[512];
	//buffer specifically for readFile, max file size
	char buffer2[13312];
	int sectorsRead;

	while (*letters != 0x0)
	{
		putInMemory(startVidMem, vidMemOffSet, *letters);
		vidMemOffSet++;
		putInMemory(startVidMem, vidMemOffSet, white);
		vidMemOffSet++;
		//advance letters pointers
		letters++;
	}

	//test for step 1 - print hello world to the screen when boot up OS
	//printString("Hello Worlds");

	//test for step 1 printChar function - prints char to the screen
	//printChar('A');

	//char line[80];
/*	printString("Enter a line: ");
	readString(line);
	printString(line);

	//char buffer[512];
	readSector(buffer, 30);
	printString(buffer);

*/

	//debugging statement for writeSector
	//writeSector("hi world", 30);

	//call handleInterrupt21 to make our own interrupts!
	makeInterrupt21();

	//debugging for writeSector interrupt
	interrupt(0x21, 6, "hello David", 30, 0);

	//nothing passed, used for "21 works" message in handleInterrupt21 function below (debugging statement)
	//interrupt(0x21, 0, 0, 0, 0);

	//call printString() using interrupt 21
/*	interrupt(0x21, 0, line, 0, 0);	
	//call readString() using interrupt 21
	interrupt(0x21, 1, line, 0, 0);
	//call readSector() using interrupt 21
	interrupt(0x21, 2, buffer, 30, 0);
	//print buffer to print out the readsector message from message.txt
	interrupt(0x21, 0, buffer, 0, 0);
	//call interrupt 21 with ax = 4 to print the error message
	interrupt(0x21, 4, 0, 0, 0);
*/	//call interrupt 21 to readFile
	interrupt(0x21, 3, "messag", buffer2, &sectorsRead);
	if(sectorsRead > 0)
		interrupt(0x21, 0, buffer2, 0, 0); /*print the file*/
	else
		/*no sectors read? then print an error*/
		interrupt(0x21, 0, "messag not found\r\n", 0, 0);

	//TEST FOR EXECUTEPROGRAM
	//executeProgram("tstpr1");

	//call executeProgram
//	interrupt(0x21, 4, "tstpr1", 0, 0);

	//call tstpr2 for terminate
//	interrupt(0x21, 4, "tstpr2", 0, 0);


	//call shell
	interrupt(0x21, 4, "shell", 0, 0);


	while(1); /*hang up*/
}

void printString(char* chars)
{
	//intital value
	int index = 0;
	//an array of 0 values
	char i = chars[index];

	while (i != 0)
	{	//interrupt(0x10, 0xe * 256 + 'Q', 0, 0, 0)
				//prints the letter 'Q'
		interrupt(0x10, 0xe * 256 + i, 0, 0, 0);
		i = chars[++index]; //prints out each char inside the array
					//moves onto the next char
	}				//pre-increments the index
						//value is first incremented
}

//printchar function (step 1 recommends we add this)
void printChar(char c)
{
	interrupt(0x10, 0xe * 256 + c, 0, 0, 0);
}


void readString(char* line)
{
	int index = 0;
	char i = 0x0;

	while(i != 0xd)
	{

		
		//backspace - tentative attempt 2!
		/*if(i == 0x08)
		{
			if(index != 0)
			{
				index--;
				//interrupt instead?
				printChar(0x08);
				printChar(0x20);
				printChar(0x08);
			}
		}*/

		// reading from user input
		i = interrupt(0x16, 0, 0, 0, 0);
		//pointer to char array/char
		*(line + index) = i;
		// printing user's input to the screen
		interrupt(0x10, 0xe * 0x100 + i, 0, 0, 0);
		index++;
	}
	

	printChar(0xa);
	line[index] = 0xa;
	printChar(0x0);
	index++;
	line[index] = 0x0;

/*	if(i == 0xd)
	{

		index++;	
		//adding line feed to end of the array and printing it
		*(line + index) = 0xa;
		interrupt(0x10, 0xe * 256 + index, 0, 0, 0);
		index++;
		//adding the end of string character to the end of the array and printing it
		*(line + index) = 0x0;
		interrupt(0x10, 0xe * 256 + index, 0, 0, 0);
	}*/
}

void readSector(char* buffer, int sector)
{
	int AH = 0x2;
	int AL = 0x1;

	//int BX = char*
	//int CH = (sector, 30);
	//int CL = (sector, 30) + 1;
	//int DH = (sector, 30);

	char* BX = buffer;
	int CH = 0x0;
	int CL = sector + 1;
	int DH = 0x0;

	int DL = 0x80;
	int AX = AH * 256 + AL;
	int CX = CH * 256 + CL;
	int DX = DH * 256 + DL;

	//interrupt(number, AX, BD, CX, DX)
	interrupt(0x13, AX, BX, CX, DX);

	//interrupt(0x13, AX, CX, DX, buffer);

}

void writeSector(char* buffer, int sector)
{
	int AH = 0x3;
	int AL = 0x1;

	//int BX = char*
	//int CH = (sector, 30);
	//int CL = (sector, 30) + 1;
	//int DH = (sector, 30);

	char* BX = buffer;
	int CH = 0x0;
	int CL = sector + 1;
	int DH = 0x0;

	int DL = 0x80;
	int AX = AH * 256 + AL;
	int CX = CH * 256 + CL;
	int DX = DH * 256 + DL;

	//interrupt(number, AX, BD, CX, DX)
	interrupt(0x13, AX, BX, CX, DX);

	//interrupt(0x13, AX, CX, DX, buffer);

}

//function to read a file and to load it, sector by sector, to buffer array
void readFile(char* filename, char* buffer2, int* sectorsRead)
{

	//will store the directory once it is loaded via readSector
	char directory[512];
	//iterator for for loop to find the file name in the directory
	int fileentry = 0;
	int loadIndex = 6;
	char s;

	//load the directory sector into the character array directory[]
	//use 2 as the sector because that is the sector holding the dir
	//1 is the map, 2 is the dir
	*sectorsRead = 0;
	readSector(directory, 2);

	//debugging statement
	//printString(directory);

	//loop to find the file name
	for(fileentry; fileentry < 512; fileentry += 32)
	{
//		printChar(directory[0]);
//		printChar(filename[0]);
//		printChar('a');
		/*if(filename[0] != directory[fileentry + 0])
			continue;*/
		if(filename[0] == directory[fileentry + 0])
		{
//			printChar(filename[0]);
//			printChar(directory[fileentry + 0]);
			if(filename[1] == directory[fileentry + 1])
			{
//			printChar(filename[1]);
//			printChar(directory[fileentry + 1]);
				if(filename[2] == directory[fileentry + 2])
				{
//					printChar(filename[2]);
//					printChar(directory[fileentry + 2]);
					if(filename[3] == directory[fileentry + 3])
					{
//						printChar(filename[3]);
//						printChar(directory[fileentry + 3]);
						if(filename[4] == directory[fileentry + 4])
						{
//							printChar(filename[4]);
//							printChar(directory[fileentry + 4]);
							if(filename[5] == directory[fileentry + 5])
							{//	printChar(filename[5]);
//								printChar(directory[fileentry + 5]);
//								printChar('!');
								//debugging statements
								//printString("Found file!");
/*								*sectorsRead = *sectorsRead + 1;*/

								//for loop to load the file
								for(loadIndex; loadIndex < 32; loadIndex++)
								{

									s = directory[fileentry + loadIndex];
									//done reading file
									if(s == 0)
										break;
									else
									{
										readSector(buffer2, s);
										buffer2 += 512;
										*sectorsRead = *sectorsRead + 1;
									}

								}
								break;
						} else
							continue;
					}
						else
							continue;
					}
					else
						continue;
				}
				else
					continue;
			}
			else
				continue;
		}
		else
			continue;
		
	}	

}


void writeFile(char* buffer, char* filename, int numberOfSectors)
{
/*
	char map[512];
	char dir[512];
	int sectorNumber = 0;
	
	while(sectorNumber = 3; sectorNumber < 512; sectorNumber++)
	{
		if(map[sectorNumber] == 0)
		{
			
		}	
	}
*/
}



//function to execute a program by first reading the file name of the program, loading it into memory, and then executing it
void executeProgram(char * name)
{
	//buffer to hold the file name
	char bufferForFile[13312];
	int index;
	int sectorRead;
	//read file into the buffer
//	printChar('#');
	readFile(name, bufferForFile, &sectorRead);

	if( sectorRead == 0)
		return;
//	printChar('%');

	//load the file into memory using putInMemory from kernel.asm
	//;void putInMemory (int segment, int address, char character)
	for(index = 0; index < 13312; index++)
	{
		putInMemory(0x2000, index, bufferForFile[index]);
	}	

//	printChar('*');
	launchProgram(0x2000);
}

void deleteFile(char* filename)
{
	int fileentry;
	int sectorsRead;
	
	//arrays to hold the directory and the map (which says which sectors are full or not for file placement)
	char dir[512];
	char map[512];

	char buffer[13312];

	//debugging to make sure deleteFile is accessed
	printString("deleteFile in kernel.c was accessed");

	//read the directory (sector 2) into the dir array, map (sector 1) into the map array using readSector
	readSector(2, map, 1);
	readSector(2, dir, 2);

	//search for the filename to be deleted
	//resets sectorsRead to 0 in case it has been used previously
	sectorsRead = 0;
	readFile(0x21, 3, filename, buffer, &sectorsRead);
	if(sectorsRead > 0)
	{
		printString("Deleting file found the file");
		printString(buffer); /*print the file*/
	}
	else
		/*no sectors read? then print an error*/
		printString("file does not exist to be deleted\r\n");
}


//makes interrupt 21 based on function in kernel.asm. Stores our code in the interrupt vector table at the base of memory
//when interrupt 21 happens, goes to the table in memory, executes our code
void handleInterrupt21(int ax, int bx, int cx, int dx)
{
	//debugging statement
	//printString("Interrupt 21 Works!");

	//if ax equals zero, then call printString()
	if(ax == 0)
	{
		printString(bx);
	}

	//else readString() is called
	else if(ax == 1)
	{
		readString(bx);
	}

	//else read sector
	else if(ax == 2)
	{
		readSector(bx, cx);
	}
	else if(ax == 3)
	{
		readFile(bx, cx, dx);
	}
	else if(ax == 4)
	{
		executeProgram(bx);
	}
	else if(ax == 5)
	{
		terminate();
	}
	else if(ax == 6)
	{
		writeSector(bx, cx);
	}
	else if(ax == 7)
	{
		deleteFile(bx);
	}
	else if(ax >= 8)
	{
		printString("Invalid value for AX. No function available! Please try again.");
	}

}


void terminate()
{
	char shellname[6];
	shellname[0] = 's';
	shellname[1] = 'h';
	shellname[2] = 'e';
	shellname[3] = 'l';
	shellname[4] = 'l';
	shellname[5] = '\0';

	//executeProgram(shellname, 0x2000);
	interrupt(0x21, 4, shellname, 0x2000, 0);

	while(1);

}



