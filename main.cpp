/****************************************************************************************
 * Algoritmo para calcular os conjuntos menos diversos. Baseado no A* e RM-CRAG.
 *
 * Entradas: -f (char) -> arquivo txt de dataset ((obrigatório)
 *           -k (char) -> quantidade de conjutos para resultado (obrigatório)
 *
 * Adaptação: Lawrence Fernandes e Victor Lamanna
*****************************************************************************************/

#include <functional>
#include <queue>
#include <set>
#include <vector>
#include <iostream>
#include <limits>
#include <math.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>


#define ALS(X,Y,Z) if ((X=(Y *)calloc(Z,sizeof(Y)))==NULL) \
       {fprintf(out,"  failure in memory allocation\n");exit(0);}
#define ALI(X,Z) if ((X=(int *)calloc(Z,sizeof(int)))==NULL) \
       {fprintf(out,"  failure in memory allocation\n");exit(0);}
#define ALF(X,Z) if ((X=(double *)calloc(Z,sizeof(double)))==NULL) \
       {fprintf(out,"  failure in memory allocation\n");exit(0);}
#define ALM(X,Z) if ((X=(int **)calloc(Z,sizeof(int *)))==NULL) \
       {fprintf(out,"  failure in memory allocation\n");exit(0);}
#define ALMF(X,Z) if ((X=(double **)calloc(Z,sizeof(double *)))==NULL) \
       {fprintf(out,"  failure in memory allocation\n");exit(0);}

#define weight(X,Y) *(*(pweight+X)+Y)

using namespace std;

template<typename T> void print_queue( T& q1 )
{
    T q = q1;

    while( !q.empty() )
    {
        cout << q.top() << " ";
        q.pop();
    }
    cout << '\n';
}

void print_set( set<int>& a_set )
{
    for( set<int>::iterator itr = a_set.begin(); itr != a_set.end(); ++itr )
    {
        cout << *itr << ", " << endl;
    }
}

void printClusteringsLabels( ostream& os, const set<int>& stocks )
{
    for( int const& label : stocks )
    {
        os << label << ' ';
    }
}

class Element
{

private:
public:
    set<int> m_stocks;
    double m_cost;

    class DereferenceCompareElement : public binary_function<Element*, Element*, bool>
    {
    public:
        bool operator()(const Element* lhs, const Element* rhs) const
        {
            /* Mod - Invertemos a comparação. Assim, o heap da std::priority_queue
              * fica com o menor valor */
            return lhs->m_cost > rhs->m_cost;
        }
    };

    friend ostream& operator<<(std::ostream& os, const Element* dt)
    {
        os << "(labels of stocks: ";
        printClusteringsLabels( os, dt->m_stocks );
        os << ", cost: " << dt->m_cost << ")";
        return os;
    }

    ~Element()
    {
        //	    cout << "Object is being deleted" << endl;
    }

    void appendTop( set<int> initialStocks, set<int> allStocks, const double **pweight, int k )
    {
        priority_queue<Element*, vector<Element*>, DereferenceCompareElement> q;

        for( set<int>::iterator it = allStocks.begin(); it != allStocks.end(); ++it )
        {
            const bool is_in = initialStocks.find(*it) != initialStocks.end();

            if( !is_in )
            {
                Element * e = new Element();
                for( set<int>::iterator itr = initialStocks.begin(); itr != initialStocks.end(); ++itr )
                {
                    e->m_stocks.insert(*itr);
                }
                e->m_stocks.insert(*it);
                e->m_cost = getCost( e->m_stocks, pweight, k );
                q.push(e);
            }
        }

        this->m_stocks = q.top()->m_stocks;
        this->m_cost = getCost( this->m_stocks, pweight, k );
        q.pop();

        while( !q.empty() )
        {
            Element* e = q.top();
            delete e;
            q.pop();
        }
    }

    static double getCost( set<int>& stocks, const double **pweight, int k, double minVal = 1 )
    {
        int h = ( k - stocks.size() ) * minVal;
        double f = calcG( stocks, pweight ) + h; //Mod -> f = g + h
        return f;
    }

    //Mod -> Calcula g
    static double calcG( set<int>& stocks, const double **pweight )
    {
        int p = stocks.size();

        if( p > 1 )
        {
            //double factor = 1.0 / binomial_coefficient(p, 2);
            double sum = 0.0;

            for( set<int>::iterator it1 = stocks.begin(); it1 != stocks.end(); ++it1 )
            {
                int elem1 = *it1;

                for( set<int>::iterator it2 = stocks.begin(); it2 != stocks.end(); ++it2 )
                {
                    int elem2 = *it2;
                    if( elem1 < elem2 )
                    {
                        sum += pweight[*it1][*it2];
                    }
                }
            }

            return /*factor **/ sum;
        }
        else
        {
            cout << "Error: number of stocks must be greater than one!" << endl;
            exit(1);
        }
    }
};

Element* search( const set<int> stocks, const double **pweight, int p, unsigned int k )
{
    priority_queue<Element*, vector<Element*>, Element::DereferenceCompareElement> Q;

    for( int i=0; i < p; i++ )
    {
        Element* e = new Element();
        set<int> originalStocks;

        originalStocks.insert( i );
        e->appendTop( originalStocks, stocks, pweight, k );
        Q.push( e );
    }

//    print_queue(Q);

    Element* anotherE = Q.top();
    Q.pop();

    while( anotherE->m_stocks.size() < k )
    {
        anotherE->appendTop( anotherE->m_stocks, stocks, pweight, k );
        Q.push(  anotherE );
        anotherE = Q.top();
        Q.pop();
    }

    return anotherE;
}

Element* searchPseudoBrute( const set<int> stocks, const double **pweight, int p, unsigned int k, double minVal )
{
    priority_queue<Element*, vector<Element*>, Element::DereferenceCompareElement> Q;

    /* (1) Inicializa a fila com as primeiras ações */
    for( int i=0; i < p; i++ )
    {
        for( int j=0; j < p; j++ )
        {
            if( i < j )
            {
                Element* e = new Element();
                set<int> initialStocks;
                initialStocks.insert( i );
                initialStocks.insert( j );
                e->m_stocks = initialStocks;
                e->m_cost = Element::getCost( e->m_stocks, pweight, k, minVal );
                Q.push( e );
            }
        }
    }
    //----------------------------------------------------------------------------

    /* (2) Pega o elemento do topo (melhor candidato) e verifica a melhor combinação.
     * Repete o processo até ter o número k de ações. */
    Element* anotherE = Q.top();
    Q.pop();

    while( anotherE->m_stocks.size() < k )
    {
        for( set<int>::iterator it = stocks.begin(); it != stocks.end(); ++it )
        {
            const bool is_in = anotherE->m_stocks.find(*it) != anotherE->m_stocks.end();

            if( !is_in )
            {
                Element * e = new Element();
                for( set<int>::iterator itr = anotherE->m_stocks.begin(); itr != anotherE->m_stocks.end(); ++itr )
                {
                    e->m_stocks.insert(*itr);
                }
                e->m_stocks.insert(*it);
                e->m_cost = Element::getCost( e->m_stocks, pweight, k, minVal );
                Q.push(e);
            }
        }

        anotherE = Q.top();
        Q.pop();

        while( !Q.empty() )
        {
            Element* e = Q.top();
            delete e;
            Q.pop();
        }
    }
    //-------------------------------------------------------------------------------------------------------------

    return anotherE;
}

double** load_matrix( const char *in_file_name, unsigned int* p )
{
    FILE  *in;
    FILE  *out = NULL;
    double **pweight;
    int i, j, lo, k;
    double w;
    long e_count;
    unsigned int size = 0;

    if( ( in = fopen( in_file_name, "r" ) ) == NULL )
    {
        printf( "  fopen failed for input\n" );
        exit( 1 );
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

double matrixMinVal( double **pweight, int p )
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

int main( int argc, char **argv )
{
    time_t tstart, tend;
    tstart = time(0);

    char* in_file_name;

    unsigned int p;
    unsigned int k;

    int c;

    opterr = 0;

    while( ( c = getopt( argc, argv, "k:f:" ) ) != -1 )
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
            //---------------------------------
            case '?':
            {
                if (optopt == 'f')
                {
                    fprintf (stderr, "Option -%c requires a file name as argument.\n", optopt);
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

    double **pweight = load_matrix( in_file_name, &p );

    printf( "k = %d, p = %d, filename = %s\n", k, p, in_file_name );

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

    double minVal = matrixMinVal( pweight, p );

    Element* elementWithTopStocks = searchPseudoBrute( stocks, ( const double** ) pweight, p, k, minVal );
//    Element* elementWithTopStocks = search( stocks, ( const double** ) pweight, p, k );

    cout << "Top k stocks: ";
    set<int>::iterator it;
    for (it = elementWithTopStocks->m_stocks.begin(); it != elementWithTopStocks->m_stocks.end(); ++it)
    {
        cout << *it << " ";
    }

    cout << endl;

    tend = time(0);

    cout << "Search took "<< difftime( tend, tstart ) << " second(s)."<< endl;

    cout << "Cost of solution (AVG): " << elementWithTopStocks->m_cost << endl;

    /** Print cost of solution **/
    set<int>::iterator itA, itB;
    double value_from_sol = 0.0;

    for ( itA = elementWithTopStocks->m_stocks.begin(); itA != elementWithTopStocks->m_stocks.end(); ++itA )
    {
        for ( itB = elementWithTopStocks->m_stocks.begin(); itB != elementWithTopStocks->m_stocks.end(); ++itB )
        {
            if( *itA > *itB )
            {
                value_from_sol += pweight[*itA][*itB];
            }
        }
    }
    cout << "Cost of solution (SUM): " << value_from_sol << endl;

    ////Mod -> Imprime vetor--------------------------------------------------------------------
//    cout << endl << "-----------------------------------------" << endl;
//    for( int i=0; i < p; ++i )
//    {
//        for( int j=0; j < p; ++j )
//        {
//            if( i < j )
//            {
//                cout << "matrix[" << i << "][" << j << "] = " << pweight[i][j] << endl;
//            }
//        }
//    }
    /////-------------------------------------------------------------------------------------------------

    return 0;
}
