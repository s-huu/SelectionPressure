#include "lsaPack.h"
#include <vector>
#include "pnorm.h"
#include "lsaParse.h"

using namespace std;


float lsa ( vector<float> * x , vector<float> * y , int n , int d , int * D )
{
	float Pmax = 0.0 ;
	float Nmax = 0.0 ;
	
	// P[i][0] , N[i][0]
	for( int i = 0 ; i <= d ; i ++ )
	{
		float Ptemp = 0.0 ;
		float Ntemp = 0.0 ;
		
		for( int k = 0 ; k + i < n ; k ++ )
		{
			if( Ptemp + x->at(i+k) * y->at(k) > 0.0 )
			{
				Ptemp = Ptemp + x->at(i+k) * y->at(k) ;
			}
			if( Ntemp - x->at(i+k) * y->at(k) > 0.0 )
			{
				Ntemp = Ntemp - x->at(i+k) * y->at(k) ;
			}
			if( Ptemp > Pmax )
			{
				Pmax = Ptemp ;
				if( Pmax > Nmax )
				{
					*D = i ;
				}
			}
			if( Ntemp > Nmax )
			{
				Nmax = Ntemp ;
				if( Nmax > Pmax )
				{
					*D = i ;
				}
			}
		}
	}
	
	// P[0][j] , N[0][j]
	for( int j = 1 ; j <= d ; j ++ )
	{
		float Ptemp = 0.0 ;
		float Ntemp = 0.0 ;
		
		for( int k = 0 ; k + j < n ; k ++ )
		{
			if( Ptemp + x->at(k) * y->at(j+k) > 0.0 )
			{
				Ptemp = Ptemp + x->at(k) * y->at(j+k) ;
			}
			if( Ntemp - x->at(k) * y->at(j+k) > 0.0 )
			{
				Ntemp = Ntemp - x->at(k) * y->at(j+k) ;
			}
			if( Ptemp > Pmax )
			{
				Pmax = Ptemp ;
				*D = -j ;
			}
			if( Ntemp > Nmax )
			{
				Nmax = Ntemp ;
				*D = -j ;
			}
		}
	}
	
	if( Pmax > Nmax )
	{
		return( Pmax / n ) ;
	}
	else 
	{
		return ( - Nmax / n ) ;
	}
	
	return( 0.0 ) ;
}

float lsaPBound ( float x , int n , int minij , int d , float variance , PNorm * p )
{
	if ( x < 0 )
		x = -x ;
	return(   2.0 * ( n*n - ( n - d - 1 ) * ( n - d ) ) * ( 1 - p->getCumProbability( x * sqrt( n / variance ) ) )  ) ;
}

vector<float> standardize1 ( vector< float > v )
{
	int n = v.size();
	float mean = 0.0 ;
	for( int i = 0 ; i < n ; i ++ )
	{
		mean = mean +  v[i] ;
	}
	mean = mean / ((float) n ) ;
	float sd = 0.0 ;
	for( int i = 0 ; i < n ; i ++ )
	{
		sd = sd + pow( v[i] - mean , 2 ) ;
	}
	sd = sqrt( sd / ( n - 1 ) ) ;
	for( int i = 0 ; i < n ; i ++ )
	{
		v[i] = ( v[i] - mean ) / sd  ;
	}
	return( v );
}

vector<float> standardize2 ( vector< float > v )
{
	int n = v.size();
	float mean = 0.0 ;
	for( int i = 0 ; i < n ; i ++ )
	{
		mean = mean +  v[i] ;
	}
	mean = mean / ((float) n ) ;
	float sd = 0.0 ;
	for( int i = 0 ; i < n ; i ++ )
	{
		sd = sd + pow( v[i] - mean , 2 ) ;
	}
	sd = sqrt( sd / ( n - 1 ) ) ;
	for( int i = 0 ; i < n ; i ++ )
	{
		v[i] = v[i] / sd  ;
	}
	return( v );
}

// a : lsa values
// b : lsa P-value bounds
dataFrame lsaRun1( dataFrame data , int d , int minij , float alpha , int pNormRes )
{
	cout << "Running LSA ... " << endl;
	
	PNorm * p = new PNorm( 1.0/((float)pNormRes) , 0.01 , 5.0 );
	
	int n = data.getData().size();
	dataFrame out;
	out.print();
	
	//get shortest vector length;
	int m = 32767; // max int value
	for ( int i = 0 ; i < n ; i ++ )
	{
		if( data.get(i).size() < (unsigned int) m )
			m = data.get(i).size() ;
	}
	
	//standardize
	for( int i = 0 ; i < n ; i ++ )
	{
		data.setRow( i , standardize1(data.get(i)) ) ;
	}
	
	cout << "initialized ... " << endl ;
	
	float count1 = 1.0 ;
	float count2 = 0.0 ;
	float count3 = 0.0 ;
	int D ;
	for( int i = 0 ; i < n - 1 ; i ++ )
	{
		for( int j = i+1 ; j < n ; j ++ )
		{
			float lsaVal = lsa( data.getVectorPointer(i) , data.getVectorPointer(j) , m , d , &D ) ;
			float pvaluebound = lsaPBound ( lsaVal , m , minij , d , 1.0 , p ) ;  // 1.0 == variance, pad till satisfied
			
			if( pvaluebound < alpha )
			{
				vector < float > temp;
				temp.push_back( (float) i );
				temp.push_back( (float) j );
				temp.push_back( lsaVal );
				temp.push_back( pvaluebound );
				temp.push_back( D ) ;
				out.addRow( temp ) ;
				count2 = count2 + 1.0 ;
				count3 = count3 + 1.0 ;
			}
			count1 = count1 + 1;
			if( ((int) count1) % 10000 == 0 )
			{
				cout << 100.0 * ( count1 / ((float) (n*n - n) / 2 ) ) << "% done, " << (count2/100.0) << "% significant" << endl ;
				count2 = 0.0 ;
			}
		}
	}
	
	cout << "LSA Complete, " << 100.0*( count3 / ((float) (n*n - n) / 2 ) ) << "% significant" << endl;
	return ( out ) ;
}

dataFrame lsaRun2( dataFrame data , int d , float rejectionBound )
{
	cout << "Running LSA ... " << endl;
	
	int n = data.getData().size();
	dataFrame out;
	
	//get shortest vector length;
	int m = 32767; // max int value
	for ( int i = 0 ; i < n ; i ++ )
	{
		if( data.get(i).size() < (unsigned int) m )
			m = data.get(i).size() ;
	}
	
	float count1 = 1.0 ;
	float count2 = 0.0 ;
	float count3 = 0.0 ;
	int * D ;
	for( int i = 0 ; i < n - 1 ; i ++ )
	{
		for( int j = i+1 ; j < n ; j ++ )
		{
			float lsaVal = lsa( data.getVectorPointer(i) , data.getVectorPointer(j) , m , d , D ) ;
			
			if( lsaVal > rejectionBound )
			{
				vector < float > temp;
				temp.push_back( (float) i );
				temp.push_back( (float) j );
				temp.push_back( lsaVal );
				temp.push_back( *D );
				out.addRow( temp ) ;
				count2 = count2 + 1.0 ;
				count3 = count3 + 1.0 ;
			}
			count1 = count1 + 1;
			if( ((int) count1) % 10000 == 0 )
			{
				cout << 100.0 * ( count1 / ((float) (n*n - n) / 2 ) ) << "% done, " << (count2/100.0) << "% significant" << endl ;
				count2 = 0.0 ;
			}
		}
	}
	
	cout << "LSA Complete, " << 100.0*( count3 / ((float) (n*n - n) / 2 ) ) << "% significant" << endl;
	return ( out ) ;
}






