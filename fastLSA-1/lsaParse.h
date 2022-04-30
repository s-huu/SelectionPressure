#ifndef  LSAPARSE_H
#define  LSAPARSE_H

// This version of parse is meant to be more specialized toward Pyrotaq data.
// Primary data storage type changed from int to float

#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <sstream>

using namespace std;

class dataFrame {
private:
	
	vector<vector<float> > data;
	
public:
	
	bool load( string fileName );
	vector < vector < float > > getData();
	void print();
	
	void set( int n , int m );
	void minLength( int n );
	void removeCol( int n );
	void removeRow( int n );
	
	void set ( int i , int j , float value );
	float get ( int i , int j );
	vector < float > get( int i );
	string floatToString( float d );
	
	void write( string fileName );
	
	void addRow( vector < float > v );
	
	int setRow( unsigned int i , vector < float > v );
	
	vector<float> * getVectorPointer( int i );
	
	// removes entries without at least i non-zero values
	void groom1( int min );
	void groom2( int col , float min );
	
	void binaryTransform( float partition );
	
	void logTransform();
	int n();
	
};

#endif











