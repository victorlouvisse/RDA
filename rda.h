#ifndef RDA_H
#define RDA_H

#include <queue>
#include <set>

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
                          priority_queue<Element*, vector<Element*>, Element> *Q );

/** Retorna o primeiro elemento com as k ações */
    static Element search( const set<int> stocks,
                    const double **pweight,
                    int p,
                    uint k,
                    int j,
                    const double minVal );
};

#endif // RDA_H
