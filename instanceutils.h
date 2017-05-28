#ifndef INSTANCEUTILS_H
#define INSTANCEUTILS_H

class InstanceUtils
{
public:

    static double** load( const char *in_file_name, int* p );

    static double matrixMinVal( double **pweight, int p );

};

#endif // INSTANCEUTILS_H
