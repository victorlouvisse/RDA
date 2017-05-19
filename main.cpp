/****************************************************************************************
 * Algoritmo para calcular diversidade de conjuntos. Baseado no A* e RM-CRAG.
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
#include <iomanip>
#include <limits>
#include <math.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>

#include <memory>


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

clock_t TIME;

const string ITERATOR_COUNT_STRING = "i";
const string ELEMENT_SIZE_STRING = "sz";
const string ELEMENT_COST_STRING = "cost";
const string SOLUTION_TIME_STRING = "time (ms)";
const string PRIORITY_QUEUE_SIZE_STRING = "Qsz";

/** Utils ************************************************************************************************************************************ */
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

void printVerboseHeader()
{
    cout << endl
         << ITERATOR_COUNT_STRING << " = " << "Número de iterações" << endl
         << ELEMENT_SIZE_STRING << " = " << "Tamanho atual da carteira" << endl
         << ELEMENT_COST_STRING << " = " << "Custo atual da solução" << endl
         << SOLUTION_TIME_STRING << " = " << "Tempo em milisegundos para encontrar a solução" << endl
         << PRIORITY_QUEUE_SIZE_STRING << " = " << "Tamanho da fila de prioridades" << endl
         << endl;
}

/** ****************************************************************************************************************************************** */

/** Class Element ********************************************************************************************************************* */
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
            /*  Assim, o heap da std::priority_queue fica com o menor valor. */
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
    }

    void appendTop( set<int> initialStocks, set<int> allStocks, const double **pweight, int k, double minVal )
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
                e->m_cost = getCost( e->m_stocks, pweight, k, minVal );
                q.push(e);
            }
        }

        this->m_stocks = q.top()->m_stocks;
        this->m_cost = getCost( this->m_stocks, pweight, k, minVal );
        q.pop();

        while( !q.empty() )
        {
            Element* e = q.top();
            delete e;
            q.pop();
        }
    }

    static double getCost( set<int>& stocks, const double **pweight, int k, double minVal )
    {
        int h = ( k - stocks.size() ) * minVal;
        double f = calcG( stocks, pweight ) + h; //f = g + h
        return f;
    }

    //Calcula g
    static double calcG( set<int>& stocks, const double **pweight )
    {
        int p = stocks.size();

        if( p > 1 )
        {
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

            return sum;
        }
        else
        {
            cout << "Error: number of stocks must be greater than one!" << endl;
            exit(1);
        }
    }
};
/** *************************************************************************************************************************************** */

/** RM-CRAG ********************************************************************************************************************************** */
Element searchRmCrag( const set<int> stocks, const double **pweight, int p, unsigned int k, double minVal )
{
    priority_queue<Element*, vector<Element*>, Element::DereferenceCompareElement> Q;

    for( int i=0; i < p; i++ )
    {
        Element* e = new Element();
        set<int> originalStocks;

        originalStocks.insert( i );
        e->appendTop( originalStocks, stocks, pweight, k, minVal );
        Q.push( e );
    }

    Element anotherE = *Q.top();
    delete Q.top();
    Q.pop();

    while( anotherE.m_stocks.size() < k )
    {
        anotherE.appendTop( anotherE.m_stocks, stocks, pweight, k, minVal );
        Q.push(  &anotherE );
        anotherE = *Q.top();
        Q.pop();
    }

    return anotherE;
}
/** ****************************************************************************************************************************************** */

/** RDA (RM-CRAG Alternativo) ************************************************************************************************************* */
/* Adiciona os j primeiros elementos na fila de prioridades Q */
void findTopElements( set<int> currStocks, set<int> allStocks, const double **pweight, int k, int j, double minVal, priority_queue<Element*, vector<Element*>, Element::DereferenceCompareElement> *Q )
{
    priority_queue<Element*, vector<Element*>, Element::DereferenceCompareElement> auxQueue;

    /* Combina as ações de currStocks com todas as ações do dataset */
    for( set<int>::iterator it = allStocks.begin(); it != allStocks.end(); ++it )
    {
        bool isIn = currStocks.find(*it) != currStocks.end();

        if( !isIn )
        {
            Element * e = new Element();
            for( set<int>::iterator itr = currStocks.begin(); itr != currStocks.end(); ++itr )
            {
                e->m_stocks.insert(*itr);
            }
            e->m_stocks.insert(*it);
            e->m_cost = Element::getCost( e->m_stocks, pweight, k, minVal );
            auxQueue.push(e);
        }
    }

    /* Adiciona j primeiros elementos na fila de prioridades Q e deleta o resto */
    int count = 0;
    while( !auxQueue.empty() )
    {
        Element* e = auxQueue.top();

        if( count < j )
        {
            Q->push( e );
        }
        else
        {
            delete e;
        }

        auxQueue.pop();

        count++;
    }
}

/* Retorna o primeiro elemento com k ações */
Element searchRDA( const set<int> stocks, const double **pweight, int p, unsigned int k, int j, const double minVal )
{
    priority_queue<Element*, vector<Element*>, Element::DereferenceCompareElement> Q;

    /* Faz a primeira rodada para ter as melhores j combinações */
    for( int i=0; i < p; i++ )
    {
        set<int> initialStocks;

        initialStocks.insert( i );
        findTopElements( initialStocks, stocks, pweight, k, j, minVal, &Q );
    }

    /* Pega o melhor resultado da primeira rodada */
    Element topElement = *Q.top();
    delete Q.top();
    Q.pop();

    int iCount = 0;
    unsigned currStockSize = 1;

    printVerboseHeader(); //verbose

    cout << endl << "**************************************************" << endl; //verbose

    /* Enquanto o tamanho da carteira não for o desejado, adiciona as primeiras j combinações na fila Q.
     * Logo, a saída acontece quando primeira combinação da fila de prioridades Q possuir k ações */
    while( topElement.m_stocks.size() < k )
    {
        //verbose ---------------------------------------------------------------------------------------
        iCount++;
        if( topElement.m_stocks.size() > currStockSize ) //verbose
        {
            TIME = clock() - TIME;

            cout << ITERATOR_COUNT_STRING << " = " << left << setw(10) << iCount
                 << ELEMENT_SIZE_STRING << " = " <<  left << setw(3) << topElement.m_stocks.size()
                 << ELEMENT_COST_STRING << " = " <<  left << setw(13) << topElement.m_cost
                 << SOLUTION_TIME_STRING << " = " <<  left << setw(10) << double(TIME) / CLOCKS_PER_SEC * 1000
                 << PRIORITY_QUEUE_SIZE_STRING << " = " <<  left << setw(10) << Q.size() <<  endl;

            currStockSize = topElement.m_stocks.size();
        }
        // -----------------------------------------------------------------------------------------------

        findTopElements( topElement.m_stocks, stocks, pweight, k, j, minVal, &Q );

        topElement = *Q.top();
        delete Q.top();
        Q.pop();
    }

    cout << "**************************************************" << endl << endl; //verbose
    cout << "QSz final = " << Q.size() << endl; //verbose

    // Apaga fila da memória
    while( !Q.empty() )
    {
        delete Q.top();
        Q.pop();
    }

    return topElement;
}
/** ****************************************************************************************************************************************** */

/** Matriz *********************************************************************************************************************************** */
double** load_matrix( const char *in_file_name, int* p )
{
    FILE  *in;
    FILE  *out = NULL;
    double **pweight;
    int i, j, lo, k;
    double w;
    long e_count;
    int size = 0;

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
/** ****************************************************************************************************************************************** */

int main( int argc, char **argv )
{
    TIME = clock();

    char* in_file_name = NULL;

    int p = -1;
    int k = -1;

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

    Element elementWithTopStocks = searchRDA( stocks, ( const double** ) pweight, p, k, 2, minVal );
//    Element elementWithTopStocks = searchRmCrag( stocks, ( const double** ) pweight, p, k, minVal );

    cout << "Top k stocks: ";
    set<int>::iterator it;
    for (it = elementWithTopStocks.m_stocks.begin(); it != elementWithTopStocks.m_stocks.end(); ++it)
    {
        cout << *it << " ";
    }

    cout << endl;

    TIME = clock() - TIME;
    cout << "Search took "<< double(TIME) / CLOCKS_PER_SEC * 1000 << " millisecond(s)."<< endl;

    cout << "Cost of solution (AVG): " << elementWithTopStocks.m_cost << endl;

    /** Print cost of solution **/
    set<int>::iterator itA, itB;
    double value_from_sol = 0.0;

    for ( itA = elementWithTopStocks.m_stocks.begin(); itA != elementWithTopStocks.m_stocks.end(); ++itA )
    {
        for ( itB = elementWithTopStocks.m_stocks.begin(); itB != elementWithTopStocks.m_stocks.end(); ++itB )
        {
            if( *itA > *itB )
            {
                value_from_sol += pweight[*itA][*itB];
            }
        }
    }

    cout << "Cost of solution (SUM): " << value_from_sol << endl;

    return 0;
}
