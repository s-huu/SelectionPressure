#include "lsaParse.h"

// This version of parse is meant to be more specialized toward Pyrotaq data.
// Primary data storage type changed from int to float
using namespace std;
	
bool dataFrame::load( string fileName ) {
		
		data.clear();
		
		//constants:
		//char space = (char) 32;
		char tab = (char) 9;
		char newLine = (char) 10;
		
		
		FILE * file = fopen( fileName.c_str() , "r" );
		
		if( file == NULL )
		{
			std::cout << "lsaParse1.h ERROR 1" << endl;
			return( false );
		}
		
		string s = "";
		
		char c = fgetc( file );
		vector<float> * temp = new vector<float>();
		while( c != EOF )
		{
			
			if( c == '#' )
			{
				// find next new line character
				c = fgetc( file );
				while( c != EOF && c != newLine )
				{
					c = fgetc( file );
				}
			}
			
			if( c != tab && c != newLine && c != '#' )
			{
				s.append( 1 , c );
			}
			
			if( c == tab )
			{
				(*temp).push_back( atof( s.c_str() ) );
				s = "";
			}
			
			if( c == newLine )
			{
				(*temp).push_back( atof( s.c_str() ) );
				data.push_back( (*temp) );
				temp = new vector<float>();
				s = "";
			}
			
			c = fgetc( file );
		}
		
		if( s.size() > 0 )
		{
			(*temp).push_back( atoi( s.c_str() ) );
		}
		
		if( (*temp).size() > 0 )
		{
			data.push_back( (*temp) );
		}
        return true;
}
	
vector < vector < float > > dataFrame::getData() {
		return data;
}
	
void dataFrame::print() {
	for( unsigned int i = 0 ; i < data.size() ; i ++ ) {
		for( unsigned int j = 0 ; j < data.at(i).size() ; j ++ ) {
				cout << data.at(i).at(j) << " " ;
		}
		cout << endl;
		}
}
	

void dataFrame::set( int n , int m ) {
	data.clear();
	for( int i = 0 ; i < n ; i ++ ) {
		data.push_back( (*( new vector<float>( m , 0.0 ) )) );
			/*
			for( int j = 0 ; j < m ; j ++ )
			{
				data.at(i).push_back( 0 ) ;
			}
			*/
			cout << (((float) i ) / ((float) n )) << endl;
	}
}
	
void dataFrame::minLength ( int n ) {
	vector < vector < float > > temp;
		
	for( unsigned int i = 0 ; i < data.size() ; i ++ ) {
		if( data.at(i).size() > (unsigned int) n )  { // emphasis here 
				temp.push_back( data.at(i) ) ;
		}
	}
	
	data = temp;
}
	
void dataFrame::removeCol ( int n ) {
	vector < vector < float > > temp1;
	vector < float > * temp2;
		
	for(unsigned int i = 0 ; i < data.size() ; i ++ ) {
		int m = data.at(i).size();
		if( m > n ) {
			temp2 = new vector<float>();
			for( int j = 0 ; j < n ; j ++ ) {
				(*temp2).push_back(  data.at(i).at(j)  );
			}
			//skip the element
			for( int j = n+1 ; j < m ; j ++ ) {
				(*temp2).push_back(  data.at(i).at(j)  );
			}
				
			temp1.push_back(  (*temp2)  );
				
		}
	}
		
	data = temp1;
}
	
void dataFrame::removeRow( int n ) {
	vector < vector < float > > temp;
		
	int m = data.size() ;
		
	if( ! m > n ) {
		return ;
	}
		
	for(unsigned int i = 0 ; i < (unsigned int)n ; i ++ ) {
		temp.push_back( data.at(i)  ) ;
	}
		//skip element n
	for( int i = n+1 ; i < m ; i ++ ) {
		temp.push_back(  data.at(i)  ) ;
	}
		
	data = temp ;
}
	
void dataFrame::set ( int i , int j , float value ) {
		data[i][j] = value;
}
	
float dataFrame::get ( int i , int j ) {
	return ( data[i][j] ) ;
}
	
vector <float> dataFrame::get( int i ) {
		return( data[i] ) ;
}
	
string dataFrame::floatToString( float d ) {
	std::ostringstream ss;
	ss << d ;
	return(  ss.str()  );
}
	
void dataFrame::write( string fileName ) {
	//constants:
	char tab = (char) 9;
	char newLine = (char) 10;
		
	FILE * file = fopen( fileName.c_str() , "w" );
	if( file == NULL ) {
		cout << "ERROR : dataFrame write failed" << endl;
	}
	
	string header = "index1	index2	LSA	lag	p-valueBound	" ;
	header.push_back( '\n' ) ;
	
	for ( int i = 0 ; i < (int) header.size() ; i ++ )
	{
		fputc( header[i] , file ) ;
	}
		
	for( unsigned int i = 0 ; i < data.size() ; i ++ ) {
		for( unsigned int j = 0 ; j < data[i].size() ; j ++ ) {
			string s = floatToString( data[i][j] );
			for(unsigned  int k = 0 ; k < s.size() ; k ++ ) {
				fputc( s[k] , file ) ;
			}
			fputc( tab , file ) ;
		}
		fputc( newLine , file ) ;
	}
}
	
void dataFrame::addRow( vector < float > v ) {
	data.push_back( v );
}
	
int dataFrame::setRow( unsigned int i , vector < float > v ) {
	if( 0 <= i && i < data.size() ) {
		data[i] = v ;
		return( 0 ) ;
	}
	return ( -1 ) ;
}
	
vector<float> * dataFrame::getVectorPointer( int i ) {
	return( & data[i] ) ;
}
	
	// removes entries without at least i non-zero values
void dataFrame::groom1 ( int min ) {
	int n = data.size() ;
	vector < vector < float > > out;
	for( int i = 0 ; i < n ; i ++ ) {
		int c = 0;
		for( unsigned int j = 0 ; j < data.at(i).size() ; j ++ ) {
			if( data.at(i).at(j) > 0 ) {
				c ++ ;
			}
		}
		if( c > min ) {
			out.push_back( data.at(i) ) ;
		}
			//cout <<  100.0 * (((float)i)/((float)n)) << "%"  << endl;
	}
		data = out ;
}
	
void dataFrame::groom2 ( int col , float min ) {
	vector < vector < float > > out;
	int n = data.size() ;
	for( int i = 0 ; i < n ; i ++ ) {
		if( data.at(i).at(col) > min ) {
			out.push_back( data.at(i) ) ;
		}
	}
	data = out ;
}
	
void dataFrame::binaryTransform ( float partition ) {
	int n = data.size();
	for( int i = 0 ; i < n ; i ++ ) {
		int m = data.at(i).size() ;
		for( int j = 0 ; j < m ; j ++ ) {
			if( get(i,j) > partition ) {
				set(i,j,1.0) ;
			}
			else {
				set(i,j,0.0) ;
			}
		}
	}
}
	
void dataFrame::logTransform () {
	int n = data.size();
	for( int i = 0 ; i < n ; i ++ ) {
		int m = data.at(i).size() ;
		for( int j = 0 ; j < m ; j ++ ) {
			if( get(i,j) > 0 ) {
				set(i,j, log(get(i,j))  ) ;
			}
			else {
				set(i,j,-1.0) ;
			}

		}
	}
}
	
int dataFrame::n () {
	return( data.size() ) ;
}
	

