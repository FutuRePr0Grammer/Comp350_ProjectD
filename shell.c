// Joao Alves & David Rochon
int fileType(char* string);
int executeFile(char* string);
int listDir(char* string);
int deleteFile(char* string);
int cpFile(char* string);
int createFile(char* string);
int main2();

//void type(char* filename);
int main(){
	main2();
 }


char type [] = "type";
char execute [] = "exec";
char dir [] = "dir";
char del [] = "del";
char cp [] = "cp";
char create [] = "create";

char lines[100];
char badCommand [] = "File Not Found, Try again \r \n";
char buffer[13312];
char directory[512];


int main2()
{

	while(1)
	{
		char getfilename;
		int filename;
		int sectorsRead;
		int entry;
//		int result;

		syscall( 0, "SHELL> ", 0, 0); //promt for shell
                syscall( 1, lines, 0 , 0); //read user's string to the screen

		//debugging for listDir
/*		syscall(0, "The result of listDir is:", 0, 0);
		result = listDir(lines);
		syscall(0, result, 0, 0);
*/

/*
	// Different approach for getting 'type' instead of my function fileType
		// loades the file
		if(lines[0] == 't' && lines[1] == 'y' && lines[2] == 'p' && lines[3] == 'e')
		{
                        syscall( 3, lines + 5, buffer, 0);
                        syscall(0, buffer, 0, 0);
		}
		else if(lines[0] == 'e' && lines[1] == 'x' && lines[2] == 'e' && lines[3] == 'c')
		{
			type(lines, filename, 5);
		}
		else
		{
                        syscall(0, badCommand, 0, 0);
                        // prints error message if no file matches the user input
                }
*/


		// read the file
		if(fileType(lines) == 1)
		{
			syscall( 3, lines + 5, buffer, &filename);

			if(filename > 0)
				syscall( 0, buffer, 0, 0);
			else
				syscall(0,"file not found", 0 , 0);
		}

		// executes the file
		else if(executeFile(lines) == 1)
		{
			syscall(0, "exec program", 0 , 0);
			syscall(0, lines +5 ,0 ,0);
			syscall( 4, lines + 5, buffer);
			syscall(0, "file not found!", 0 ,0);
			//syscall( 0, buffer, 0, 0);
		}
		else if(listDir(lines) == 1)
		{
//			syscall(0, "listDir works", 0, 0);
			//calls readsector (interrupt 2) using the directory array as a buffer and sector 2 (where dir is located)
			syscall(2, directory, 2, 0);
		
			//for loop to print each filename in the directory
			entry = 0;
			for(entry = 0; entry < 512; entry += 32)
			{
				directory[entry + 6] = 0;
				//call printString (interrupt 0x0) to print the directory
				syscall(0, directory + entry, 0, 0);
				syscall(0, "\r\n", 0, 0);
			}
		}
		else if(deleteFile(lines) == 1)
		{
			//debugging statements
			//syscall(0, "del command read successfully", 0, 0);
			//syscall(0, "del command read successfully! File name being pased is: ", 0, 0);
			//syscall(0, lines + 4, 0, 0);

			syscall(7, lines + 4, 0, 0);
		}
		else if(cpFile(lines) == 1)
		{
			//debugging statement
/*			//syscall(0, "cpFile works!", 0, 0);

			//starting from lines[3] (cp space = first 3 letters], get filename 1 until a space, 
			//then filename2 begins and should be stored until end of line is reached (enter)
			//once get the filenames, copy the bytes from filename 1 into filename2
			int currentCharIndex = 3;
			int indexFile1 = 0;
			int indexFile2 = 0;
			char currentChar = lines[3];
			char filename1[];
			char filename2[];

			//get filename 1
			while(currentChar != 0x20)
			{	
				//debugging
				syscall(0, "currentChar is", 0, 0);
				syscall(9, currentChar, 0, 0);

				//if character is enter, not enough filenames entered
				if(currentChar == 0x0A)
				{
					syscall(0, "Not enough arguments for copy file, input two file names");
					break;
				}
				currentChar = lines[currentCharIndex];
				filename1[indexFile1] = lines[currentCharIndex];
				indexFile1++;
				currentCharIndex++;
			}
			
			//increment currentCharIndex to get past the space			
			currentCharIndex++;	

			//debugging
			syscall(0, "filename1", 0, 0);
			syscall(0, filename1, 0, 0);

			//get filename2
			while(currentChar != 0x0A)
			{
				currentChar = lines[currentCharIndex + indexFile2];
				filename2[indexFile2] = lines[currentCharIndex + indexFile2];
				indexFile2++;
				currentCharIndex++;
			}

			//debugging
			syscall(0, "filename2", 0, 0);
			syscall(0, filename2, 0, 0);
*/

			int file1Index;
			int file2Index;
			char filename1[6];
			char filename2[6];
			int bufferIndex;
			int sectorsRead;

			//syscall(0, "In copyFile\n", 0, 0);

			for(bufferIndex = 0; bufferIndex < 13312; bufferIndex++)
			{
				buffer[bufferIndex] = '\0';
			}

			for(file1Index = 0; file1Index < 6; file1Index++)
			{
				filename1[file1Index] = lines[file1Index + 3];
			}

			syscall(0, filename1, 0, 0);

			for(file2Index = 0; file2Index < 6; file2Index++)
			{
				filename2[file2Index] = lines[file2Index + 10];
			}

			syscall(0, filename2, 0, 0);

			syscall(3, filename1, buffer, &sectorsRead);

			if(sectorsRead > 0)
			{
				syscall(8, buffer, filename2, sectorsRead);
			}
		}

		else if(createFile(lines) == 1)
		{
			//debugging
			//syscall(0, "Create file recognized\n", 0, 0);

			char fileName[6];
			int filenameIndex;
			char newInput[100];

			//get the filename to be created
			for(filenameIndex = 0; filenameIndex < 6; filenameIndex++)
			{
				fileName[filenameIndex] = lines[filenameIndex + 7];
			}

			//debugging
			//syscall(0, lines, 0, 0);
			syscall(0, fileName, 0, 0);

			//test to write the file properly
			syscall(8, "Test", fileName, 0);
		}

		else
		{
			syscall(0, badCommand, 0, 0);
			// prints error message if no file matches the user input
		}
	}
}

/*
// function to read file
void type(char* filename)
{
	int sectorsRead;
	while(1);
	syscall(3,buffer, filename, &sectorsRead);
	if(sectorsRead==0)
		syscall(0, "File Not Found, Try Again! \r\n");
	else
		syscall(0, buffer);
}
*/



//function to recognize 'type'
int fileType(char* string)
{

	int i = 0;
	int index = 1;

	while(i < 4) //searching with 'type'
	{
		if (string[i] != type[i])  //comparing the first letter with user input
		{
			index = 0; //if it is not equal, start over
			break;
		}

		i++; //move onto the next letter

	}
	return index;
}

// function to recognize 'exec'
int executeFile(char* string)
{
        int i = 0;
        int index = 1;

        while(i < 4) //searching with 'exec'
        {
                if (string[i] != execute[i])
                {
                        index = 0;
                        break;
                }

                i++;

        }
        return index;
}

// function to recognize 'dir' command
int listDir(char* string)
{
	int i = 0;
	int index = 1;

	while(i < 3) //searching with 'dir'
	{
//		syscall(0, string[i], 0, 0);
//		syscall(0, dir[i], 0, 0);
		if(string[i] != dir[i])
		{
			index = 0;
			break;
		}

		i++;
	}
	return index;
}

// function to recognize 'del' command
int deleteFile(char* string)
{
	int i = 0;
	int index = 1;
	
	while(i < 3) //searching for 'del'
	{
		if(string[i] != del[i])
		{
			index = 0;
			break;
		}

		i++;
	}
	return index;
}

//function to recognize "cp" command
int cpFile(char* string)
{
	int i = 0;
	int index = 1;
	
	while(i < 2) //searching for "cp"
	{
		if(string[i] != cp[i])
		{
			index = 0;
			break;
		}
		i++;
	}
	return index;
}

//function to recognize "create" command
int createFile(char* string)
{
	int i = 0;
	int index = 1;

	while(i < 6)
	{
		if(string[i] != create[i])
		{
			index = 0;
			break;
		}
		i++;
	}
	return index;
}
