#ifndef LSA_PACK_H
#define LSA_PACK_H
#include <vector>
#include "pnorm.h"
#include "lsaParse.h"

float lsa ( vector<float> * x , vector<float> * y , int n , int d , int * D );
float lsaPBound ( float x , int n , int minij , int d , float variance , PNorm * p );
vector<float> standardize1 ( vector< float > v );
vector<float> standardize2 ( vector< float > v );
dataFrame lsaRun1( dataFrame data , int d , int minij , float alpha , int pNormRes );
dataFrame lsaRun2( dataFrame data , int d , float rejectionBound );

#endif 
