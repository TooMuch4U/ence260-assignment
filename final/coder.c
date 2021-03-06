/** @file coder.c
 * @author Emma Hogan, Tom Rizzi
 * @date 27 September 2020
 * @brief communications encoding module
 * last edited 11 October 2020 by Emma Hogan
 */


#include "coder.h"

/* A Reed-Solomon code is generated by a Vandermonde matrix over some field.
 * Because of the provided communication functions in the funkit API, it is easiest
 * to communicate data as bytes (ie by sending a char). Thus, it is best for the
 * binary representation of the code to have length a multiple of 8.
 *
 * In my case I have decided to work over the field F_4 which is isomorphic to (Z_2)^2.
 * Therefore every element of my field has a binary representation of length 2, and the
 * transmitted length of my code is 1 byte. I have chosen a message length of k = 2,
 * which is 4 bits in binary transmission. Thus I have 16 possible code words in my code
 * and a distance of d = n - k + 1 = 3. This implies I can correct any transmission with
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
 * representation for the coding side - ie for indexing into arrays.
 */


/** Multiplication table over F_4 indexed by decimal representation of element. eg x^2 * x = mutliplication_table[3][2]
    (or [2][3]) technically as multiplication is commutative */
static uint8_t multiplication_table[CODE_LENGTH][CODE_LENGTH] = {
    {0,0,0,0},
    {0,1,2,3},
    {0,2,3,1},
    {0,3,1,2},
};


/** Similar for addition */
static uint8_t addition_table[CODE_LENGTH][CODE_LENGTH] = {
    {0,1,2,3},
    {1,0,3,2},
    {2,3,0,1},
    {3,2,1,0}
};


/** Linear map to generate codewords from messages */
static uint8_t generator_matrix[MESSAGE_LENGTH][CODE_LENGTH] = {
    {1,1,1,1},
    {0,1,2,3}
};


/** Parity check matrix to calculated syndromes from received messages */
static uint8_t transposed_parity_check_matrix[CODE_LENGTH][PARITY_DIM] = {
    {1,0},
    {1,1},
    {1,2},
    {1,3}
};


/** Representative vectors for each syndrome. Indexed by decimal value of syndrome
interpreted as a quaternary value */
static uint8_t representatives[NUM_SYNDROMES][CODE_LENGTH] = {
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


/** Multiply a message vector by a generator matrix with terms over F_4:
    @param vector represented as an array to be LHS of product
    @param matrix, a 2D array to be RHS of product
    @param result, an array in which to place result of multiplication */
static void multiply_generator (uint8_t vector[], uint8_t matrix[][CODE_LENGTH], uint8_t result[])
{
    uint8_t sum;
    uint8_t product;
    for (uint8_t i = 0; i < CODE_LENGTH; i++) {
        //compute dot product of message and relevant column transpose
        sum = 0;
        for (uint8_t j = 0; j < MESSAGE_LENGTH; j++) {
            //interpret multiplication and addition over F_4
            product = multiplication_table[vector[j]][matrix[j][i]];
            sum = addition_table[sum][product];
        }
        result[i] = sum;
    }
}


/** Multiply a vector by a parity check matrix with terms over F_4:
    @param vector represented as an array to be LHS of product
    @param matrix, a 2D array to be RHS of product
    @param result, an array in which to place result of multiplication */
static void multiply_parity_check (uint8_t vector[], uint8_t matrix[][PARITY_DIM], uint8_t result[])
{
    uint8_t sum;
    uint8_t product;
    for (uint8_t i = 0; i < PARITY_DIM; i++) {
        sum = 0;
        for (uint8_t j = 0; j < CODE_LENGTH; j++) {
            //interpret multiplication and addition over F_4
            product = multiplication_table[vector[j]][matrix[j][i]];
            sum = addition_table[sum][product];
        }
        result[i] = sum;
    }
}


/** Subtract 2 vectors over F_4:
    @param vector1 represented as an array for LHS of subtraction
    @param vector2 represented as an array for RHS of subtraction
    @param result, an array in which to place result of subtraction */
static void subtract_vectors (uint8_t vector1[], uint8_t vector2[], uint8_t result[])
{
    for (uint8_t i = 0; i < CODE_LENGTH; i++) {
        // addition and subtraction are identical since 1 = -1 over F_4
        result[i] = addition_table[vector1[i]][vector2[i]];
    }
}


/** Convert the codeword into an ascii characters for ease of transmission:
    @param vector represented as an array to be converted into a char
    eg: [2,1,0,3] -> (10)(01)(00)(11) = 10010011 */
static uint8_t convert_to_char (uint8_t vector[])
{
    uint8_t c = 0;
    for (uint8_t i = 0; i < CODE_LENGTH; i++) {
        c = c << 2;
        c += vector[i];
    }
    return c;
}


/** Convert the transmitted ascii character back into a vector (array) over F4:
    @param c the transmitted character
    @param vector represented as an array
    eg: B = 01000010 -> (01)(00)(00)(10) = [1,0,0,2] */
static void convert_to_vector (uint8_t c, uint8_t vector[])
{
    for (int i = 0; i < CODE_LENGTH; i++) {
        vector[i] = c >> (8 - 2 * (i + 1)) & (0x3);
    }
}


/** Encode an arbitrary message of length 4 in bits into a 1 char long string via reed-solomon code:
    @param message, an integer between 0 and 15
    @return an integer representing the ascii encoding of the message */
uint8_t encode (uint8_t message)
{
    //split message into 2 2 bit sequences
    uint8_t a = message >> 2;
    uint8_t b = message % 4;

    //produce message vector over F_4 from binary
    uint8_t message_vector[] = {a, b};
    uint8_t codeword[CODE_LENGTH] = {0};

    // multiply message by generator matrix to get codeword
    multiply_generator(message_vector, generator_matrix, codeword);
    uint8_t transmission = convert_to_char(codeword); //get codeword as transmittable ascii

    return transmission;
}


/** Use reed-solomon code to decode and error correct received transmission:
    @param transmission, the received ascii character
    @return an integer representing the most likely original message after error correcting */
uint8_t decode (uint8_t transmission)
{
    uint8_t vector[CODE_LENGTH];
    uint8_t syndrome[MESSAGE_LENGTH];
    uint8_t representative[CODE_LENGTH];
    uint8_t syndrome_val;
    uint8_t corrected_code[CODE_LENGTH];
    uint8_t original_message[MESSAGE_LENGTH];

    convert_to_vector(transmission, vector);
    multiply_parity_check(vector, transposed_parity_check_matrix, syndrome);

    syndrome_val = syndrome[1] + 4 * syndrome[0]; //interpret syndrome as quaternary value
    for (uint8_t i = 0; i < CODE_LENGTH; i++) {
        representative[i] = representatives[syndrome_val][i];
    }
    subtract_vectors(vector, representative, corrected_code); // get best guess at transmitted codeword

    original_message[0] = corrected_code[0];
    original_message[1] = addition_table[corrected_code[1]][original_message[0]];

    return original_message[1] + 4 * original_message[0];
}

