/****************************************************************************************
 * Lawrence Fernandes e Victor Lamanna
 * Maio 2017
*****************************************************************************************/

#include "element.h"

#include <iostream>
#include <sstream>

Element::Element()
{
}

Element::~Element()
{
}

double Element::calcCost( set<int>& stocks, const double **pweight, int k, double minVal )
{
    int h = ( k - stocks.size() ) * minVal;
    double f = calcG( stocks, pweight ) + h; //f = g + h
    return f;
}

double Element::calcG( set<int>& stocks, const double **pweight )
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

string Element::getStocksString()
{
    stringstream stream;
    set<int>::iterator it;
    for (it = m_stocks.begin(); it != m_stocks.end(); ++it)
    {
        stream << *it << " ";
    }

    return stream.str();
}
