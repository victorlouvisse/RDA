/****************************************************************************************
 * Algoritmo para calcular diversidade de conjuntos. Baseado no A* e RM-CRAG.
 *
 * Adaptação: Lawrence Fernandes e Victor Lamanna
 * Maio 2017
*****************************************************************************************/

#include "rda.h"

set<ulong> RDA::m_elementsHashes;

bool RDA::isDuplicate( double elementHash )
{
    return m_elementsHashes.find( elementHash ) != m_elementsHashes.end();
}

void RDA::findTopElements(set<int> currStocks,
                           set<int> allStocks,
                           const double **pweight,
                           int k,
                           int j,
                           double minVal,
                           priority_queue<Element *, vector<Element *>, Element::MinCostCompare> *Q )
{
    priority_queue<Element*, vector<Element*>, Element::MinCostCompare> auxQueue;

    /* Combina as ações de currStocks com todas as ações do dataset */
    for( set<int>::iterator it = allStocks.begin(); it != allStocks.end(); ++it )
    {
        bool isIn = currStocks.find(*it) != currStocks.end();

        if( !isIn )
        {
            Element * e = new Element();
            for( set<int>::iterator itr = currStocks.begin(); itr != currStocks.end(); ++itr )
            {
                e->addStock(*itr);
            }
            e->addStock(*it);
            e->setCost( e->getStocks(), pweight, k, minVal );
            auxQueue.push(e);
        }
    }

    /* Adiciona j primeiros elementos na fila de prioridades Q e deleta o resto */
    int count = 0;
    while( !auxQueue.empty() )
    {
        Element* e = auxQueue.top();

        if( count < j && !isDuplicate( e->hash() ) )
        {
            addHash( e->hash() );
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

Element RDA::search( const set<int> stocks,
                     const double **pweight,
                     int p,
                     uint k,
                     int j,
                     const double maxVal )
{
    priority_queue<Element*, vector<Element*>, Element::MinCostCompare> Q;

    /* Faz a primeira rodada para ter as melhores j combinações */
    for( int i=0; i < p; i++ )
    {
        set<int> initialStocks;

        initialStocks.insert( i );
        findTopElements( initialStocks, stocks, pweight, k, j, maxVal, &Q );
    }

    /* Pega o melhor resultado da primeira rodada */
    Element topElement = *Q.top();
    delete Q.top();
    Q.pop();

    /* Enquanto o tamanho da carteira não for o desejado, adiciona as primeiras j combinações na fila Q.
     * Logo, a saída acontece quando primeira combinação da fila de prioridades Q possuir k ações */
    while( topElement.getStocksSize() < k )
    {
        findTopElements( topElement.getStocks(), stocks, pweight, k, j, maxVal, &Q );

        topElement = *Q.top();
        delete Q.top();
        Q.pop();
    }

    // Apaga fila da memória
    while( !Q.empty() )
    {
        delete Q.top();
        Q.pop();
    }

    return topElement;
}
