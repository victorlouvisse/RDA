#ifndef RDA_H
#define RDA_H

#include <queue>
#include <set>
#include <iostream>

#include "element.h"

class RDA
{

public:
/** Adiciona os j primeiros elementos na fila de prioridades Q */
    static void findTopElements( std::set<int> currStocks,
                          std::set<int> allStocks,
                          const double **pweight,
                          int k,
                          int j,
                          double minVal,
                          priority_queue<Element*, vector<Element*>, Element::MinCostCompare> *Q );

/** Retorna o primeiro elemento com as k ações */
    static Element search( const set<int> stocks,
                    const double **pweight,
                    int p,
                    uint k,
                    int j,
                    const double maxVal );

private:
/** Estrutura auxiliar que guarda os hashes dos elementos da fila de prioridades Q para evitar duplicatas. */
    static set<ulong> m_elementsHashes;
    static void
    addHash( ulong elementHash ) { m_elementsHashes.insert( elementHash ); }
    static bool isDuplicate( double elementHash );


    template<typename T> void print_queue( T& q1 )
    {
        T q = q1;

        while( !q.empty() )
        {
            cout << q.top() << " ";
            q.pop();
        }
        cout << endl;
    }
};

#endif // RDA_H
