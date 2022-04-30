
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

#include "lsaPack.h"
#include "thread.h"

float lsaForThreads ( float * x , float * y , int n , int d , int * D )
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
			float temp = Ptemp + x[i+k] * y[k] ;
			if(temp > 0.0 )
			{
				Ptemp = temp ;
			}
			temp = Ntemp - x[i+k] * y[k] ;
			if( temp > 0.0 )
			{
				Ntemp = temp ;
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
			float temp = Ptemp + x[k] * y[j+k] ;
			if( temp > 0.0 )
			{
				Ptemp = temp ;
			}
			temp = Ntemp - x[k] * y[j+k] ;
			if( temp > 0.0 )
			{
				Ntemp = temp ;
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


inline float lsaPBoundForThreads ( float x , int n , int minij , int d , float variance , PNorm * p )
{
	if ( x < 0 )
		x = -x ;
	return(  2.0 * ( n*n - ( n - d - 1 ) * ( n - d ) ) * ( 1 - p->getCumProbability( x * sqrt( n / variance ) ) )  ) ;
}

vector<intPair> param4 ( int n , int threads )
{
	vector<intPair> out ;
	
	long int m = (long int) n ;
	long int jobs = m * (m-1) / 2 ;
	long int batchSize = jobs / ((long int) threads) ;
	
	long int c = 0 ;
	for ( int i = 0 ; i < (n-1) ; i ++ )
	{
		for ( int j = i+1 ; j < n ; j ++ )
		{
			if ( c % batchSize == 0 )
			{
				intPair ip1 ;
				ip1.a = i ;
				ip1.b = j ;
				out.push_back( ip1 ) ;
			}
			
			if ( c % batchSize == 1 && c!= 1 )
			{
				intPair ip1 ;
				ip1.a = i ;
				ip1.b = j ;
				out.push_back( ip1 ) ;
			}
			
			c ++ ;
		}
	}
	
	{
		intPair ip1 ;
		ip1.a = n-2 ;
		ip1.b = n-1 ;
		out.push_back( ip1 ) ;
	}
	
	return ( out ) ;
}

string intToString( int i )
{
	stringstream ss ;
	ss << i ;
	return ss.str() ;
}

string convertSeconds( long int s )
{
	string out = "" ;
	long int days , hours , min ;
	
	days = s / (60*60*24) ;
	s = s % (60*60*24) ;
	hours = s / (60*60) ;
	s = s % (60*60) ;
	min = s / 60 ;
	s = s % 60 ;
	
	out = intToString(days) + "d " + intToString(hours) + "h " + intToString(min) + "m " + intToString(s) + "s" ;
	
	return( out ) ;
}

void * tProgressFunc ( void * arg )
{
	tProgressArg * tpa = (tProgressArg*) arg ;
	time_t startTime = time (NULL) ;
	time_t totalTime ;
	
	long unsigned int total = 0 ;
	while ( total < tpa->jobs )
	{
		total = 0 ;
		for ( int i = 0 ; i < tpa->threads ; i ++ )
		{
			total = total + tpa->work[i] ;
		}
		totalTime = time (NULL) - startTime ;
		long double proportion = ((long double) total) / ((long double) tpa->jobs) ;
		cout << 100.0 * proportion << "%, ETA total : " << convertSeconds((long int) (totalTime / proportion) ) << ", remaining : " << convertSeconds((long int) ((totalTime / proportion) - totalTime)) << endl ;
		
		bool b = true ;
		for ( int i = 0 ; i < tpa->threads ; i ++ )
		{
			if ( tpa->threadComplete[i] == false )
				b = false ;
		}
		if ( b )
		{
			pthread_exit( NULL ) ;
			break ;
		}
		
		sleep( 1 ) ;
	}
	pthread_exit( NULL ) ;
}

void * tFunc ( void * arg )
{
	tArg * targ = (tArg*) arg ;
	
	int i = targ -> start.a ;
	int j = targ -> start.b ;
	
	while (true) // ( (i <= targ->end.a) || (j <= targ->end.b) )
	{
		int D ;
		float lsaVal = lsaForThreads ( targ->data[i] , targ->data[j] , targ->m , targ->d , &D ) ;
		float pval = lsaPBoundForThreads ( lsaVal , targ->m , targ->minij , targ->d , 1.0 , targ->p ) ;
		(*( targ -> work ))++ ;
		if( pval < targ->alpha && ( lsaVal > targ->minLSA || -lsaVal > targ->minLSA ) )
		{
			DataItem * temp = new DataItem () ;
			temp -> item[0] = i ;
			temp -> item[1] = j ;
			temp -> item[2] = lsaVal ;
			temp -> item[3] = D ;
			temp -> item[4] = pval ;
			
			LinkedListNode * node = new LinkedListNode () ;
			node -> dataItem = temp ;
			node -> before = NULL ;
			node -> after = NULL ;
			if( targ -> list -> start != NULL )
			{
				targ -> list -> end -> after = node ;
				node -> before = targ -> list -> end ;
				node -> after = NULL ;
				targ -> list -> end = node ;
			}
			else
			{
				node -> before = NULL ;
				node -> after = NULL ;
				targ -> list -> start = node ;
				targ -> list -> end = node ;
			}
		}
		
		if ( i == targ->end.a && j == targ->end.b )
			break ;
		
		// increment i & j
		j ++ ;
		if( j >= targ->n )
		{
			i ++ ;
			j = i + 1 ;
		}
		
	}
	
	*(targ -> threadComplete) = true ;
	
	pthread_exit( NULL ) ;
}

dataFrame lsaThreadRun1 ( dataFrame data , int d , int minij , float alpha , float minLSA , int pNormRes , int threads )
{
	dataFrame out ;
	
	cout << "Running LSA ... " << endl ;
	
	int n = data.getData().size() ;
	
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
	
	cout << "LSA has detected " << n << " rows and " << m << " columns" << endl;
	
	//float * * dataMatrix = convert ( data.getData() , n , m ) ;
	float * * dataMatrix ;
	dataMatrix = ( float * * ) malloc( sizeof(float *) * n ) ;
	for( int i = 0 ; i < n ; i ++ )
	{
		dataMatrix[i] = (float *) malloc( sizeof(float) * m ) ;
		if( dataMatrix[i] == NULL )
		{
			cout << "ERROR, thread, convert. Type 1." << endl ;
		}
		else
		{
			for( int j = 0 ; j < m ; j ++ )
			{
				dataMatrix[i][j] = data.get(i,j) ;
			}
		}
	}
	
	if( threads >= (((long int)n)*((long int)n-1)-((long int)n))/4 )
	{
		threads = (((long int)n)*((long int)n-1)-((long int)n))/4 ;
	}
	
	vector<intPair> vip = param4 ( n , threads ) ;
	
	pthread_t thr [ threads ] ;
	LinkedList array [ threads ] ; // No vectors allowed here. Change to a linked list.
	tArg targ [ threads ] ;
	long unsigned int work [ threads ] ;
	bool threadComplete [ threads ] ;
	
	cout << "initialized ... " << endl ;
	
	for( int i = 0 ; i < threads ; i ++ )
	{
		array[i].start = NULL ;
		array[i].end = NULL ;
		targ[i].list = &array[i] ;
		targ[i].start = vip[2*i] ;
		targ[i].end = vip[2*i+1] ;
		targ[i].data = dataMatrix ;
		targ[i].d = d ;
		targ[i].minij = minij ;
		targ[i].alpha = alpha ;
		targ[i].p = new PNorm( ((double)1.0)/((double)pNormRes) , 0.01 , 5.0 ) ;
		targ[i].m = m ;
		targ[i].n = n ;
		targ[i].minLSA = minLSA ;
		targ[i].work = & work[i] ;
		work[i] = 0 ;
		targ[i].threadComplete = & threadComplete[i] ;
		threadComplete[i] = false ;
		
		int code = pthread_create ( &thr[i] , NULL , tFunc , (void*) &targ[i] ) ;
		if( code > 0 )
		{
			cout << "ERROR, thread : type 3, code : " << code << endl ;
			exit ( -1 ) ;
		}
	}
	
	// Progress meter
	pthread_t progress ;
	tProgressArg parg ;
	parg.work = work ;
	parg.threads = threads ;
	parg.jobs = ( ((long unsigned int)n)*((long unsigned int)n)-((long unsigned int)n) )/((long unsigned int)2) ;
	pthread_create ( &progress , NULL , tProgressFunc , &parg ) ;
	parg.threadComplete = threadComplete ;
	
	// Join threads
	for( int i = 0 ; i < threads ; i ++ )
	{
		void * status ;
		int code = pthread_join( thr[i] , & status ) ;
		if( code > 0 )
		{
			cout << "ERROR, thread : type 4, code : " << code << endl ;
		}
	}
	
	{
		void * status ;
		int code = pthread_join( progress , & status ) ;
		if( code > 0 )
		{
			cout << "ERROR, thread : type 5, code : " << code << endl ;
		}
	}
	
	cout << "joining thread results ..." << endl ;
	
	// Construct output
	for( int i = 0 ; i < threads ; i ++ )
	{
		LinkedListNode * node = targ[i].list->start ;
		while( node != NULL )
		{
			vector<float> temp ;
			temp.push_back ( node->dataItem->item[0] ) ;
			temp.push_back ( node->dataItem->item[1] ) ;
			temp.push_back ( node->dataItem->item[2] ) ;
			temp.push_back ( node->dataItem->item[3] ) ;
			temp.push_back ( node->dataItem->item[4] ) ;
			out.addRow ( temp ) ;
			
			node = node->after ;
		}
	}
	
	cout << "LSA complete" << endl ;
	
	//pthread_exit(NULL);
	return ( out ) ;
}


















