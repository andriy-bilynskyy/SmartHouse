#ifndef CVALVECTRL_H_
#define CVALVECTRL_H_

#include <stdint.h>



class CValveCtrl
{
public:
    CValveCtrl();
    ~CValveCtrl();
    static void turn(bool on);

    static volatile bool    m_terminate;

private:

};

#endif
