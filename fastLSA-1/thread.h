
float lsaForThreads ( float * x , float * y , int n , int d , int * D ) ;

struct DataItem
{
	float item[5] ; // stores outputs
};

struct LinkedListNode
{
	LinkedListNode * before ;
	LinkedListNode * after ;
	DataItem * dataItem ;
};

struct LinkedList
{
	LinkedListNode * start ;
	LinkedListNode * end ;
};

struct intPair
{
	int a ;
	int b ;
};

float * * convert ( vector< vector< float > > data , int nrow , int ncol ) ;

vector<intPair> param4 ( int n , int threads ) ;

struct tProgressArg
{
	long unsigned int * work ;
	int threads ;
	long unsigned int jobs ;
	bool * threadComplete ;
};

string convertSeconds( int s ) ;

void * tProgressFunc ( void * arg ) ;

struct tArg
{
	LinkedList * list ;
	intPair start ;
	intPair end ;
	float * * data ;
	int d ;
	int minij ;
	float alpha ;
	PNorm * p ;
	int m ; // time series length
	int n ; // number of time series
	float minLSA ;
	long unsigned int * work ;
	bool * threadComplete ;
};

void * tFunc ( void * arg ) ;

dataFrame lsaThreadRun1 ( dataFrame data , int d , int minij , float alpha , float minLSA , int pNormRes , int threads ) ;
