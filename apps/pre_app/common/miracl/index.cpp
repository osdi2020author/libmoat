/*
 *   Test program to find discrete logarithms using Pollard's rho method.
 *   Suitable primes are generated by "genprime" program
 *
 *   See "Monte Carlo Methods for Index Computation"
 *   by J.M. Pollard in Math. Comp. Vol. 32 1978 pp 918-924
 *
 *   Requires: big.cpp crt.cpp
 *
 *   Copyright (c) 1988-1997 Shamus Software Ltd.
 */

#include <iostream>
#include <fstream>
#include "big.h"   /* include MIRACL system */
#include "crt.h"   /* chinese remainder thereom */
#define NPRIMES 10
#define PROOT 2

using namespace std;

Miracl precision=100;

static Big p,p1,order,lim1,lim2;
static BOOL flag=FALSE;

void iterate(Big &x,Big &q,Big &r,Big &a,Big &b)
{ /* apply Pollards random mapping */
    if (x<lim1)
    {
        x=(x*q)%p;
        a+=1;
        if (a==order) a=0;
        return;
    }
    if (x<lim2)
    {
        x=(x*x)%p;
        a*=2;
        if (a>=order) a-=order;
        b*=2;
        if (b>=order) b-=order;
        return;
    }
    x=(x*r)%p;
    b+=1;
    if (b==order) b=0;
}

long rho(Big &q,Big &r,Big &m,Big &n)
{ /* find q^m = r^n */
    long iter,rr,i;
    Big ax,bx,ay,by,x,y;
    x=1;
    y=1;
    iter=0L;
    rr=1L;
    do
    { /* Brent's Cycle finder */
        x=y;
        ax=ay;
        bx=by;
        rr*=2;
        for (i=1;i<=rr;i++)
        {
            iter++;
            iterate(y,q,r,ay,by);
            if (x==y) break;
        }
    } while (x!=y);
    m=ax-ay;
    if (m<0) m+=order;
    n=by-bx;
    if (n<0) n+=order;
    return iter;
}

int main()
{
    int i,np;
    long iter;
    Big pp[NPRIMES],rem[NPRIMES];
    Big m,n,Q,R,q,w,x;
    ifstream prime_data("prime.dat");
    p1=1;
    prime_data >> np;
    for (i=0;i<np;i++) prime_data >> pp[i];
    for (i=0;i<np;i++) p1*=pp[i];
    p=p1+1;
    if (!prime(p))
    {
        cout << "Huh - modulus is not prime!";
        return 0;
    }
    lim1=p/3;
    lim2=2*lim1;
    cout << "solve discrete logarithm problem - using Pollard rho method\n";
    cout << "finds x in y=" << PROOT << "^x mod p, given y, for fixed p\n";
    cout << "p=";
    cout << p << endl;
    cout << "given factorisation of p-1 = \n2";
    for (i=1;i<np;i++) 
        cout << "*" << pp[i];
    cout << "\n\nEnter y= ";
    cin >> q;
    Crt chinese(np,pp);
    for (i=0;i<np;i++)
    { /* accumulate solutions for each pp */
        w=p1;
        w/=pp[i];
        Q=pow(q,w,p);
        R=pow(PROOT,w,p);
        order=pp[i];
        iter=rho(Q,R,m,n);
        w=inverse(m,order);
        rem[i]=(w*n)%order;
        cout << iter << " iterations needed" << endl;
    }
    x=chinese.eval(rem);   /* apply Chinese remainder thereom */
    cout << "Discrete log of y= ";
    cout << x << endl;
    w=pow(PROOT,x,p);
    cout << "check = ";
    cout << w << endl;
    return 0;
}

