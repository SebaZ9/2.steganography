// Steganography Program
// CourseWork 1
// By Sebastian Zebrowski
// sz42@hw.ac.uk
// H00252155
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PPM { char * code; char * comments; int width; int height; int max; int ** r; int ** g; int ** b; };

struct PPM * getPPM(FILE * file){
	//Allocate space for everything
	struct PPM * ppmPointer = (struct PPM*) malloc(sizeof(struct PPM));		// PPM struct Pointer
	char * code = malloc((3)  * sizeof(char));								// Code Pointer
	char * comment = malloc((999) * sizeof(char));							// Comment Pointer
	char * buffer = malloc((999) * sizeof(char));							// Buffer Pointer for comments
	
	// PPM file code
	fgets(code, 3, file);						// fgets used to get the first line of the file	
	fgetc(file);								// Have to clear a character from the file

	// Comments - Undefined amount of comment
	fgets(buffer, 999, file); 						// Gets the second line of the file	
	while(buffer[0] == 35){							// If the first char of buffer is a hashtag (35) then go into while loop
		strcat(comment, buffer);					// Concatenate buffer on to the main comment  
		fgets(buffer, 999, file);		
	}	
	
	// Width & Height
	int width, height;	
	while(1){												// The last buffer contains the widht and height, strsep seperates the number
		if(buffer[0] == 32){								// and then sscanf converts the strings into integers of the width and height
			sscanf(strsep(&buffer," "), "%d", &width);			
		}else{												// The while loops are ther to get rid of spaces that are present in
			sscanf(strsep(&buffer," "), "%d", &width);   	// some of the files such as pbmlib.ascii.ppm and sines.ascii.ppm
			break;
		}													// To get rid of the spaces I loop over the string and if the first character
	}														// is a space ( character 32 ) I split the string with spaces meaning I get
	while(1){												// rid off the first character and I am left with the rest of the string
		if(buffer[0] == 32){
			sscanf(strsep(&buffer," "), "%d", &height);			
		}else{
			sscanf(strsep(&buffer," "), "%d", &height);
			break;
		}
	}

	// Max value
	int maxValue;
	fscanf(file, "%d",&maxValue);		// Max color value is gotten with fscanf
	
	// RGB 2-D array pointes - Had to be lower down since  we need width and height	
	int i;
	int **red;																// Red value 2-D array pointer
	red = malloc(height * sizeof(int*));
	for(i = 0; i < height; i++){
		red[i] = malloc(width * sizeof(int));
	}
	int **green;															// Green value 2-D array pointer
	green = malloc(height * sizeof(int*));
	for(i = 0; i < height; i++){
		green[i] = malloc(width * sizeof(int));
	}
	int **blue;																// Blue value 2-D array pointer
	blue = malloc(height * sizeof(int*));
	for(i = 0; i < height; i++){
		blue[i] = malloc(width * sizeof(int));
	}
	
	// Get RGB Value 
	int j, redInt, greenInt, blueInt;
	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){											// Loops over the entire image and
			fscanf(file, "%d %d %d", &redInt, &greenInt, &blueInt);			// Gets the three values with fscanf
			red[i][j] = redInt;												// And saves them in the correct array
			green[i][j] = greenInt;
			blue[i][j] = blueInt;
		}
	}
	
	// Save everything in the PPM Struct
	ppmPointer->code = code;
	ppmPointer->comments = comment;
	ppmPointer->width = width;
	ppmPointer->height = height;
	ppmPointer->max = maxValue;	
	ppmPointer->r = red;
	ppmPointer->g = green;
	ppmPointer->b = blue; 

	return ppmPointer;
}

void showPPM(struct PPM * image){
	printf("%s\n%s%d %d\n%d\n", image->code, image->comments, image->width, image->height, image->max);
	int i, j;														// ShowPPM prints the entire PPM structure to the console
	for (i = 0; i < image->height; i++) {							// The first print deals with the top part of the PPM, Code
    		for (j = 0; j < image->width; j++) {					// Comments, width/height and the max value
      			printf("%d ", *(*((image->r)+i)+j));
				printf("%d ", *(*((image->g)+i)+j));				// And then I loop over the 2-D arrays and print each 
				printf("%d    \n", *(*((image->b)+i)+j));			// pixel colour
			}
  	}
}

void savePPM(FILE * file, struct PPM * image){
	int i, j;															
	fprintf(file, "%s\n%s%d %d\n%d\n", image->code, image->comments, image->width, image->height, image->max);		// SavePPM works in the same way as showPPM except I use fprintf instead of the standard
	for (i = 0; i < image->height; i++) {																			// printf to write to the file which is passed in as a parameter
    		for (j = 0; j < image->width; j++) {
      			fprintf(file, "%d %d %d\n", *(*((image->r)+i)+j), *(*((image->g)+i)+j), *(*((image->b)+i)+j));
			}
  	}
}

struct PPM * encode(struct PPM * image, char * message, unsigned int messSize, unsigned int secret){	
	int width = image->width, height = image->height, i, j, x, y, bitCount, count, num, lsb;
	srand(secret);																				// Seed the PRNG with the given secret
	for(i=0;i<messSize;i++){
		num = message[i];																		// Num is the i'th character of the string to encode
		bitCount = 8;																			// I have a counter to keep track of what bit of the current
		for(j=0;j<3;j++){																		// character I'm currently encoding
			// Set red's LSB
			x = rand() % height; y = rand() % width;											// Get the X and Y values from the PRNG
			lsb = (num >> bitCount) & 1;														// Get the n'th bit of character that is currecntly being encoded
			*(*((image->r)+x)+y) = (*(*((image->r)+x)+y) & (~(1 << 0))) | (lsb << 0);			// Set the LSB of the red value in position (x,y) to either 1 or 0 depending on the previous result
			bitCount--;																			// update the bit that needs to be encoded next.
			// Set green's LSB																	// Repeat for all the colours
			x = rand() % height; y = rand() % width;
			lsb = (num >> bitCount) & 1;
			*(*((image->g)+x)+y) = (*(*((image->g)+x)+y) & (~(1 << 0))) | (lsb << 0);
			bitCount--;
			// Set blue's LSB
			x = rand() % height; y = rand() % width;
			lsb = (num >> bitCount) & 1;
			*(*((image->b)+x)+y) = (*(*((image->b)+x)+y) & (~(1 << 0))) | (lsb << 0);
			bitCount--;
		}		
	}	
	return image;
}

char * decode(struct PPM * image, unsigned int secret){
	char * message = malloc((999)  * sizeof(char));
	int width = image->width, height = image->height;
	int i, ch, x, y, bitCount, lsb, chCount = 0;
	srand(secret);
	while(1){ 														// Keep running until ch == 0 which is the end of message.
		ch = 0; bitCount = 8; 										// Reset the character and the bitCount for each letter.
		for(i = 0; i<3;i++){
			// Decode value from red
			x = rand() % height; y = rand() % width; 				// Chose the x,y position of the encoded bits.
			lsb = (*(*((image->r)+x)+y) >> 0) & 1;					// Set LSB oto the value of the least significant bit of the currect colour.
			ch = (ch & (~(1 << bitCount))) | (lsb << bitCount);		// Set ch's (bitCount)'th bit to the LSB.
			bitCount--;												// Repeat for all 3 colours.
			// Decode value from green
			x = rand() % height; y = rand() % width;
			lsb = (*(*((image->g)+x)+y) >> 0) & 1;
			ch = (ch & (~(1 << bitCount))) | (lsb << bitCount);
			bitCount--; 
			// Decode value from blue
			x = rand() % height; y = rand() % width;
			lsb = (*(*((image->b)+x)+y) >> 0) & 1;
			ch = (ch & (~(1 << bitCount))) | (lsb << bitCount);
			bitCount--;
		}			
		message[chCount++] = ch;
		if(ch == 0) { break; }										// Break the while loop if the character == 0 / the end of the message.		
	}	
	return message;
}

int main(int argc, char * argv[]){
	printf("\nSteg, a program to encyrpt and decrypt messages into a ppm image.\n");							// Instruction's on how to use the program and its functions.
	printf("Correct usage for encoding: ./steg 'text to be encoded' 'secret int' 'file location'\n");
	printf("Correct usage for decoding: ./steg 'secret int' 'file location'\n\n");
	if(argc == 4){																								// If there are 4 arguments we encode the image.
		printf("Encoding '%s' into '%s'\n", argv[1], argv[3]);
		FILE * file;
		file = fopen(argv[3], "r");
		struct PPM* ppm = getPPM(file);
		int length = strlen(argv[1]) + 1;
		FILE * encodedFile;
		encodedFile = fopen("encodedImage.ppm", "w");
		encode(ppm, argv[1], length, atoi(argv[2]));
		savePPM(encodedFile, ppm);
		printf("Encoding succesful.\n");
	}else if(argc == 3){																						// If there are 3 arguments we decode the image
		printf("Decoding file '%s'\n", argv[2]);	
		FILE * file;
		file = fopen(argv[2], "r");
		struct PPM* ppm = getPPM(file);		
		printf("Decoding succesful.\nMessage: %s\n", decode(ppm,atoi(argv[1])));		
	}else{																										// If there is an invalid amount of arguments we inform the user
		printf("Incorect Arguments\n");																			// of the correct usage of the program
		printf("\nCorrect usage for encoding: ./steg 'text to be encoded' 'secret int' 'file location'\n");
		printf("Correct usage for decoding: ./steg 'secret int' 'file location'\n\n");
	}
}