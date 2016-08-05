/***************************** RANDOMC.H *********************** 2001-10-24 AF *
*
* This file contains class declarations for the C++ library of uniform
* random number generators.
*
* Overview of classes:
* ====================
*
* class TRanrotBGenerator:
* Random number generator of type RANROT-B.
* Source file ranrotb.cpp
*
* class TRanrotWGenerator:
* Random number generator of type RANROT-W.
* Source file ranrotw.cpp
*
* class TRandomMotherOfAll:
* Random number generator of type Mother-of-All (Multiply with carry).
* Source file mother.cpp
*
* class TRandomMersenne:
* Random number generator of type Mersenne twister.
* Source file mersenne.cpp
*
* class TRandomMotRot:
* Combination of Mother-of-All and RANROT-W generators.
* Source file ranmoro.cpp and motrot.asm.
* Coded in assembly language for improved speed.
* Must link in RANDOMAO.LIB or RANDOMAC.LIB.
*
*
* Member functions (methods):
* ===========================
*
* All these classes have identical member functions:
*
* Constructor(long int seed):
* The seed can be any integer. Usually the time is used as seed.
* Executing a program twice with the same seed will give the same sequence of
* random numbers. A different seed will give a different sequence.
*
* double Random();
* Gives a floating point random number in the interval 0 <= x < 1.
* The resolution is 32 bits in TRanrotBGenerator, TRandomMotherOfAll and
* TRandomMersenne. 52 or 63 bits in TRanrotWGenerator. 63 bits in 
* TRandomMotRot.
*
* int IRandom(int min, int max);
* Gives an integer random number in the interval min <= x <= max. (max-min < MAXINT).
* The resolution is the same as for Random(). 
*
* unsigned long BRandom();
* Gives 32 random bits. 
* Only available in the classes TRanrotWGenerator and TRandomMersenne.
*
*
* Example:
* ========
* The file EX-RAN.CPP contains an example of how to generate random numbers.
*
*
* Further documentation:
* ======================
* The file randomc.htm contains further documentation on these random number
* generators.
*
* © 2002 Agner Fog. GNU General Public License www.gnu.org/copyleft/gpl.html

**************************************************************************
* This File has been modified by Santi Ontañón Villar (2005-2006)        *
* The modifications are:                                                 *
*  - Removed the headers for all random number generators but            *
*    TRanrotBGenerator.                                                  *
*  - This file has been modified to work with AMD64 systems.             *
*    The changes made are substitute all 'long' keywords by 'int'.       *
*    Note: it has not been tested with intel EMT64 machines              *
*         (but it should work as well).                                  *
*************************************************************************/

#ifndef RANDOMC_H
#define RANDOMC_H

#include <math.h>
#include <assert.h>
#include <stdio.h>


class TRanrotBGenerator {              // encapsulate random number generator
  enum constants {                     // define parameters
    KK = 17, JJ = 10, R1 = 13, R2 =  9};
  public:
  void RandomInit(int seed);      // initialization
  int IRandom(int min, int max);       // get integer random number in desired interval
  double Random();                     // get floating point random number
  TRanrotBGenerator(int seed);    // constructor
  protected:
  int p1, p2;                          // indexes into buffer
  unsigned int randbuffer[KK];        // history buffer
  unsigned int randbufcopy[KK*2];     // used for self-test
  enum TArch {TA_LITTLE_ENDIAN, TA_BIG_ENDIAN, TA_NON_IEEE};
  TArch Architecture;                  // conversion to float depends on computer architecture
};

  
#endif  

