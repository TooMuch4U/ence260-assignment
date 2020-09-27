//
// Created by Emma Hogan on 27/09/20.
// Last edited by Emma Hogan on 28/09/20
//

#ifndef CODER_H
#include <stdint.h>

#define CODE_LENGTH 4
#define MESSAGE_LENGTH 2
#define PARITY_DIM 2
#define NUM_SYNDROMES 16


// multiply a message vector by a generator matrix with terms over F_8
void multiply_generator(int vector[], int matrix[][CODE_LENGTH], int result[]);


// multiply a vector by a parity check matrix with terms over F_4
void multiply_parity_check(int vector[], int matrix[][PARITY_DIM], int result[]);


// subtract 2 vectors over F_4
void subtract_vectors(int vector1[], int vector2[], int result[]);


// convert the codeword into an ascii characters for ease of transmission
uint8_t convert_to_char(int vector[]);


// assign values to vector based on transmitted char
void convert_to_vector(uint8_t c, int vector[]);


// encode an arbitrary message of length 4 into a 1 char long string
uint8_t encode(int message);


uint8_t decode(uint8_t transmission);

#endif
