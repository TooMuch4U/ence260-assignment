#include "coder.h"

//
// Created by Emma Hogan on 27/09/20.
// Last edited by Emma Hogan on 28/09/20
//


/* A Reed-Solomon code is generated by a Vandermonde matrix over some field.
 * Because of the provided communication functions in the funkit API, it is easiest
 * to communicate data as bytes (ie by sending a char). Thus, it is best for the
 * binary representation of the code to have length a multiple of 8.
 *
 * In my case I have decided to work over the field F_4 which is isomorphic to (Z_2)^1.
 * Therefore every element of my field has a binary representation of length 3, and the
 * transmitted length of my code is 3 bytes. I have chosen a message length of k = 2,
 * which is 6 bits in binary transmission. Thus I have 64 possible code words in my code
 * and a distance of d = n - k + 1 = 7. This implies I can correct any transmission with
 * less than or equal to 25% bit error rate. */


/* I am generating my chosen field, F_4 with the irreducible polynomial x^2 + x + 1
 * over Z_2. This gives me the following equivalent representations for elements of F_4:
 *
 *
 *   Power: | Additive: | Binary: | Decimal:
 * --------------------------------------------------------
 *     0    |     0     |  (0,0)  |   0
 *     1    |     1     |  (0,1)  |   1
 *     x    |     x     |  (1,0)  |   2
 *    x^2   |   x + 1   |  (1,1)  |   3
 *
 *
 * Binary represents what these elements look like to the IR receiver. Power and Additive
 * forms are both useful for multiplying and adding elements respectively. The decimal
 * notation sheds little light on the behavior of the field, I merely use it as a convenient
 * representation for the coding side.
 */


/* Multiplication table over F_4 indexed by decimal representation of element. eg x^2 * x = mutliplication_table[3][2]
 * (or [2][3]) technically as multiplication is commutative*/


int multiplication_table[CODE_LENGTH][CODE_LENGTH] = {
    {0,0,0,0},
    {0,1,2,3},
    {0,2,3,1},
    {0,3,1,2},
};

// Similar for addition
int addition_table[CODE_LENGTH][CODE_LENGTH] = {
    {0,1,2,3},
    {1,0,3,2},
    {2,3,0,1},
    {3,2,1,0}
};

// Linear map to generate codewords from messages
int generator_matrix[MESSAGE_LENGTH][CODE_LENGTH] = {
    {1,1,1,1},
    {0,1,2,3}
};

// Parity check matrix to calculated syndromes from received messages
int transposed_parity_check_matrix[CODE_LENGTH][PARITY_DIM] = {
    {1,0},
    {1,1},
    {1,2},
    {1,3}
};

/*representative vectors for each syndrome. Indexed by decimal value of syndrome
interpreted as a quaternary value*/
int representatives[NUM_SYNDROMES][CODE_LENGTH] = {
    {0,0,0,0},
    {0,0,1,1},
    {0,0,2,2},
    {0,0,3,3},
    {1,0,0,0},
    {0,1,0,0},
    {0,0,1,0},
    {0,0,0,1},
    {2,0,0,0},
    {0,0,0,2},
    {0,2,0,0},
    {0,0,2,0},
    {3,0,0,0},
    {0,0,3,0},
    {0,0,0,3},
    {0,3,0,0}
};

// multiply a message vector by a generator matrix with terms over F_8
void multiply_generator(int vector[], int matrix[][CODE_LENGTH], int result[])
{
    int sum;
    for (int i = 0; i < CODE_LENGTH; i++) {
        //compute dot product of message and relevant column transpose
        sum = 0;
        for (int j = 0; j < MESSAGE_LENGTH; j++) {
            sum = addition_table[sum][multiplication_table[vector[j]][matrix[j][i]]]; //interpret multiplication and addition over F_4
        }
        result[i] = sum;
    }
}

// multiply a vector by a parity check matrix with terms over F_4
void multiply_parity_check(int vector[], int matrix[][PARITY_DIM], int result[])
{
    uint8_t sum;
    for (int i = 0; i < PARITY_DIM; i++) {
        sum = 0;
        for (int j = 0; j < CODE_LENGTH; j++) {
            sum = addition_table[sum][multiplication_table[vector[j]][matrix[j][i]]]; //interpret multiplication and addition over F_4
        }
        result[i] = sum;
    }
}

// subtract 2 vectors over F_4
void subtract_vectors(int vector1[], int vector2[], int result[])
{
    for (int i = 0; i < CODE_LENGTH; i++) {
        // addition and multiplication are identical since 1 = -1 over F_4
        result[i] = addition_table[vector1[i]][vector2[i]];
    }
}

// convert the codeword into an ascii characters for ease of transmission
uint8_t convert_to_char(int vector[])
{
    uint8_t c = 0;
    for (int i = 0; i < CODE_LENGTH; i++) {
        c = c << 2;
        c += vector[i];
    }
    return c;
}

// assign values to vector based on transmitted char
void convert_to_vector(uint8_t c, int vector[])
{
    for (int i = 0; i < CODE_LENGTH; i++) {
        vector[i] = c >> (8 - 2 * (i + 1)) & (0x3);
    }
}

// encode an arbitrary message of length 4 into a 1 char long string
uint8_t encode(int message)
{
    //split message into 2 2 bit sequences
    int a = message >> 2;
    int b = message % 4;

    //produce message vector over F_4 from binary
    int message_vector[] = {a, b};
    int codeword[CODE_LENGTH] = {0};

    // multiply message by generator matrix to get codeword
    multiply_generator(message_vector, generator_matrix, codeword);
    uint8_t transmission = convert_to_char(codeword); //get codeword as transmittable ascii

    return transmission;
}

uint8_t decode(uint8_t transmission)
{
    int vector[CODE_LENGTH];
    int syndrome[MESSAGE_LENGTH];
    int representative[CODE_LENGTH];
    int syndrome_val;
    int corrected_code[CODE_LENGTH];
    int original_message[MESSAGE_LENGTH];

    convert_to_vector(transmission, vector);
    multiply_parity_check(vector, transposed_parity_check_matrix, syndrome);

    syndrome_val = syndrome[1] + 4 * syndrome[0]; //interpret syndrome as quaternary value
    for (int i = 0; i < CODE_LENGTH; i++) {
        representative[i] = representatives[syndrome_val][i];
    }
    subtract_vectors(vector, representative, corrected_code); // get best guess at transmitted codeword

    original_message[0] = corrected_code[0];
    original_message[1] = addition_table[corrected_code[1]][original_message[0]];

    return original_message[1] + 4 * original_message[0];
}

