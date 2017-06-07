//#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
typedef struct arrayOfChars {
    unsigned char array[1024];
    int position;
} arrayOfChars;

typedef struct charByte {
    unsigned char byte;
    int numberOfBits;
} charByte;

void lfsr(int, int[],int, arrayOfChars*);
int getRegstrFeed(int , int [], int);
int getBit(int , int );
void saveLsb(unsigned, arrayOfChars*);
void printArrayToFile(arrayOfChars*);
unsigned char convertByteToChar(unsigned);
void printCharInBits(char);
int getArrayLength(int []);

charByte lsbByte;                               // la pienso quitar como global, pero lo hago despues,es la misma logica que la otra creo


int main(int argc, char* argv[]){
    //int taps[3] = {0, 3, 5};
    //printf("E: %d\n",argc-1);
    int taps[argc-1];
    int i;
    if(argc -1 > 0)                                   // el propio ejecutable cuenta como 1 parametro, asi que es necesario restarlo
        for(i=0;i < argc-1; i++)
            taps[i]= atoi(argv[i+1]);
    else
    {
        printf("No se refieron Taps para realimentar el registro");
        exit(EXIT_FAILURE);
    }
    arrayOfChars arrayToPrint;
    arrayToPrint.position = 0;
    lsbByte.numberOfBits = 0;
//  int numberOfTaps = NELEMS(taps);
    printf("number of elements = %d \n" , sizeof(taps)/sizeof(int));        // probar en otras pcs
    lfsr(7, taps, sizeof(taps)/sizeof(int) , &arrayToPrint);                                // para no usar arraytoprintf global necesito mandar un puntero ya que si mando una copia pierdo referencia a la posicion

    return 0;
}

void lfsr(int length, int taps[], int numberOfTaps, arrayOfChars* arrayToPrint) {

    unsigned regstr = 0b10101001;
    //int regstrFeed;
    long i;

    unsigned lsb;

    for (i = 0; i <  1024 * 8; i++) {	// rondas q corro el algoritmo , esto tambien podria ir por parametro para generar el tamaño del archivo final
        lsb = regstr & 1;
        saveLsb(lsb,arrayToPrint);
        regstr >>= 1;							                    // desplaza 1 lugar a la derecha, rellena con 0 en ppio
        regstr ^= getRegstrFeed(regstr, taps, numberOfTaps);		// cambia 0 x 1, 1 x 0 y lo guarda en el reg
        //printf("register = %d", regstr);
    }


}

int getRegstrFeed(int regstr, int taps[], int numberOfTaps) {
    int i;
    int regstrFeed = getBit(regstr, 7 - taps[0]);

    for (i = 1; i < numberOfTaps; i++) {
        regstrFeed ^= getBit(regstr, 7 - taps[i]);
    }

    return regstrFeed << 7;
}

int getBit(int number, int bitNumber) {
    int mask =  1 << bitNumber;			// 4 = 00000100	00001000
    int masked_n = number & mask;		// 0b10101001    & = 00001000		= 00010000

    return masked_n >> bitNumber;
}

void saveLsb(unsigned lsb, arrayOfChars* arrayToPrint) {			// guarda "caracter" en el array a imprimir
//    int i;
//    int bitToAdd = lsb << 1; //lsb & 1;


    lsbByte.byte <<= 1;				//	00000010?
    lsbByte.byte = lsbByte.byte | lsb;
    lsbByte.numberOfBits++;

    if (lsbByte.numberOfBits == 7) {
        lsbByte.numberOfBits = 0;

        arrayToPrint->array[arrayToPrint->position] = lsbByte.byte;
        arrayToPrint->position++;
        //printf("%d\n",arrayToPrint->position);
        if (arrayToPrint->position == 1023) printArrayToFile(arrayToPrint);
    }
}

void printArrayToFile(arrayOfChars* arrayToPrint) {
    int i;
    FILE* arch;
    FILE* arch2;
    if( (arch = fopen("lfsr.txt","wt") ) == NULL)
    {
        printf("ERROR CREANDO ARCHIVO\n");
        exit(EXIT_FAILURE);                                     // 1
    }
	fprintf(arch,"BBBBBBBB\n");
    for(i = 0 ; i < 1024 ; i++){
    	fprintf(arch , "Bit %d : %c\n", i, arrayToPrint->array[i]);
    }
    /**ARCHIVO QUE ESTOY PROBANDO PARA VER SI RESPETA LOS TAMAÑOS*/
    if( (arch2 = fopen("lfsraux.txt","wt") ) == NULL)
    {
        printf("ERROR CREANDO ARCHIVO\n");
        exit(EXIT_FAILURE);                                     // 1
    }
    for(i = 0 ; i < 1024 ; i++){
    	fprintf(arch2 ,"%c", arrayToPrint->array[i]);
    }
    /**---------------------------------------*/
    fclose(arch2);
    fclose(arch);
    arrayToPrint->position = 0;
}


/*unsigned char convertByteToChar(unsigned byte) {
    unsigned char b = byte & 0xffl;
    return b;
}
*/

/*void printCharInBits (char c) {
    int i;
    for (i = 0; i < 8; i++) {
        printf("%d", !!((c << i) & 0x80));
    }
    printf("\n");
}

*/

/*int getArrayLength(int array[]) {
    int i = 0;
    while(array[i] != NULL && array[i] != '\0')
        i++;
    return i;
}
*/
