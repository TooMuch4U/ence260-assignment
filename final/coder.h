/** @file coder.h
 * @author Emma Hogan, Tom Rizzi
 * @date 27 September 2020
 * @brief communications encoding module
 * last edited 11 October 2020 by Emma Hogan
 */


#ifndef CODER_H
#include <stdint.h>

#define CODE_LENGTH 4
#define MESSAGE_LENGTH 2
#define PARITY_DIM 2
#define NUM_SYNDROMES 16


/** Encode an arbitrary message of length 4 in bits into a 1 char long string via reed-solomon code:
    @param message, an integer between 0 and 15
    @return an integer representing the ascii encoding of the message */
uint8_t encode (uint8_t message);


/** Use reed-solomon code to decode and error correct received transmission:
    @param transmission, the received ascii character
    @return an integer representing the most likely original message after error correcting */
uint8_t decode (uint8_t transmission);


#endif
