#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <sstream>
#include <string>
#include <math.h>
#include <pthread.h>
using namespace std;

#include "pnorm.h"
#include "lsaParse.h"
#include "thread.h"
#include "lsaPack.h"


int main(int argc, char **argv)
{
   bool unrecognized_param = false;
   string inputfile ;
   string outputfile = "" ;
   unsigned int maxLag = 3 ;
   float minLSA = 0.0 ;
   float alpha = 0.001 ;
   unsigned int distribResolution = 100000;
   unsigned int threads = 1 ;

   for( int i = 1; i < argc; i++)
   {
        if( strncmp(argv[i], "-i", strlen("-i")) == 0 && i + 1< argc) {
             inputfile = string(argv[++i]);
        }
        else if( strncmp(argv[i], "-o", strlen("-o")) == 0 && i + 1< argc) {
             outputfile = string(argv[++i]);
        }
        else if( strncmp(argv[i], "-d", strlen("-d")) == 0 && i + 1< argc) {
             maxLag = (unsigned int)atoi(argv[++i]);
        }
        else if( strncmp(argv[i], "-m", strlen("-m")) == 0 && i + 1< argc) {
             minLSA = (float )atof(argv[++i]);
        }
        else if( strncmp(argv[i], "-a", strlen("-a")) == 0 && i + 1< argc) {
             alpha = (float )atof(argv[++i]);
        }
        else if( strncmp(argv[i], "-r", strlen("-r")) == 0 && i + 1< argc) {
             distribResolution = (unsigned int) atol(argv[++i]);
        }
		else if( strncmp(argv[i], "-t", strlen("-t")) == 0 && i + 1< argc) {
			threads = (unsigned int) atol(argv[++i]);
        }
        else {
			cout << argv[i] << " not recognized." << endl ;
            unrecognized_param = true ;
        }
   }
	
   if( unrecognized_param == true || argc < 3) {
          cerr << "Usage: " << argv[0] << " -i  inputfile -o outputfile -d N -m f -a f -r N -t N" << endl;
          cerr << "    -i  inputfile " << endl;
          cerr << "    -o  outputfile " << endl;
          cerr << "    -d  maximum time lag " << endl;
          cerr << "    -m  minimum LSA value " << endl;
          cerr << "    -a  alpha" << endl;
          cerr << "    -r  distribution resolution" << endl;
	      cerr << "    -t  number of threads" << endl;
          return(-1);
    }

	dataFrame df ;
	
	df.load( inputfile );
	
	df = lsaThreadRun1( df , maxLag, 1 , alpha , minLSA , distribResolution , threads );
	
	if( outputfile.length() > 0 )
	{
		df.write( outputfile );
	}
	
	return( 0 );
}


				  
				  
				  
				  
				  
				  
				  
				  
				  
				  
				  
				  
				  
				  
				  
				  
				  
				  



