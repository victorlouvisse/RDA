/****************************************************************************************
 * Classe para carregar em memória a instância
 *
 * Lawrence Fernandes e Victor Lamanna
 * Maio 2017
*****************************************************************************************/

#include "instanceutils.h"

#include <stdio.h>
#include <memory>
#include <iostream>

using namespace std;

#define ALS(X,Y,Z) if ((X=(Y *)calloc(Z,sizeof(Y)))==NULL) \
       {cout << "ALS failure in memory allocation\n";}
#define ALI(X,Z) if ((X=(int *)calloc(Z,sizeof(int)))==NULL) \
       {cout << "ALI failure in memory allocation\n";}
#define ALF(X,Z) if ((X=(double *)calloc(Z,sizeof(double)))==NULL) \
       {cout << " ALFfailure in memory allocation\n";}
#define ALM(X,Z) if ((X=(int **)calloc(Z,sizeof(int *)))==NULL) \
       {cout << "ALM failure in memory allocation\n";}
#define ALMF(X,Z) if ((X=(double **)calloc(Z,sizeof(double *)))==NULL) \
       {cout << "ALMF failure in memory allocation\n";}

#define weight(X,Y) *(*(pweight+X)+Y)

double** InstanceUtils::load( const char *in_file_name, int* p )
{
    FILE  *in;
    double **pweight;
    int i, j, lo, k;
    double w;
    long e_count;
    int size = 0;

    if( ( in = fopen( in_file_name, "r" ) ) == NULL )
    {
        printf( "  fopen failed for input\n" );
    }

    fscanf( in, "%d %d", &size, &i );

    *p = size;

    e_count = ( static_cast<long>( size ) ) * ( size - 1 ) / 2;

    ALMF( pweight, size )

    for ( k = 0; k < size; k++ )
    {
        ALF( *(pweight+k), size )
    }

    for( lo = 0; lo < e_count; lo++ )
    {
        fscanf( in, "%d %d %lf\n", &i, &j, &w );
        weight( i,j ) = w;
        weight( j,i ) = w;
    }

    return pweight;
}

double InstanceUtils::matrixMinVal( double **pweight, int p )
{
    double min = pweight[0][1];
    for( int i=0; i < p; ++i )
    {
        for( int j=0; j < p; ++j )
        {
            double currVal = pweight[i][j];
            if( i < j
                    && currVal < min )
            {
                min = currVal;
            }
        }
    }

    return min;
}
