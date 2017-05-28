/****************************************************************************************
 * Entradas: -f (char) -> arquivo txt de dataset ((obrigatório)
 *           -k (int) -> quantidade de conjutos para resultado (obrigatório)
 *           -j (int) -> número de melhores elementos (obrigatório)
 *
 * Lawrence Fernandes e Victor Lamanna
 * Maio 2017
*****************************************************************************************/

#include <iostream>
#include <unistd.h>

#include "element.h"
#include "instanceutils.h"
#include "rda.h"

using namespace std;

int main( int argc, char **argv )
{
    char* in_file_name = NULL;

    int p = -1;
    int k = -1;
    int j = 1; //Default: j=1 é o RM-CRAG

    int c;

    opterr = 0;

    while( ( c = getopt( argc, argv, "k:f:j:" ) ) != -1 )
    {
        switch( c )
        {
            case 'k':
            {
                k = atoi(optarg);
                break;
            }
            case 'p':
            {
                p = atoi(optarg);
                break;
            }
            case 'f':
            {
                in_file_name = optarg;
                break;
            }
            case 'j':
            {
                j = atoi(optarg);
                break;
            }
            //---------------------------------
            case '?':
            {
                if (optopt == 'f')
                {
                    fprintf (stderr, "Option -%c requires a file name as argument.\n", optopt);
                }
                else if (optopt == 'j')
                {
                    fprintf (stderr, "Option -%c requires an positive integer as argument.\n", optopt);
                }
                else if (optopt == 'k')
                {
                    fprintf (stderr, "Option -%c requires an positive integer as argument.\n", optopt);
                }
                else if ( isprint ( optopt ) )
                {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                }
                else
                {
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                }

                return 1;
            }
            default:
            {
                abort ();
            }
        }
    }

    double **pweight = InstanceUtils::load( in_file_name, &p );

    printf( "k = %d, j = %d, p = %d, filename = %s\n", k, j, p, in_file_name );

    if( k >= p )
    {
        cout << "Invalid values for parameters k and p: " << k << ", " << p << endl;
        exit(1);
    }

    set<int> stocks;

    for (int i=0; i<p; i++)
    {
        stocks.insert(i);
    }

    double minVal = InstanceUtils::matrixMinVal( pweight, p );

    clock_t time = clock();

    Element elementWithTopStocks = RDA::search( stocks, ( const double** ) pweight, p, k, 2, minVal );

    cout << "Top k stocks: " << elementWithTopStocks.getStocksString() << endl;

    time = clock() - time;
    cout << "Search took "<< static_cast<double>(time) / CLOCKS_PER_SEC * 1000 << " millisecond(s)."<< endl;

    cout << "Cost of solution: " << elementWithTopStocks.getCost() << endl;

    return 0;
}
