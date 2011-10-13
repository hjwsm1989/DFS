/* write the program to test examples in <<The Art of Computer Programming>>
 *  * huangjun 2011-10-13
 *   * hjwsm1989@gmail.com
 *    */
#include <stdio.h>
#include <stdlib.h>

int maxcommonfactor(int m, int n)
{
        int r = 0;

        if(m < n){
                int temp;
                temp = m;
                m = n;
                n = temp;
        }
                                                                           
        while (1) {
                r = m % n;
                if(r == 0)
                        break;
                m = n;
                n = r;

        };
                                                                                                                                    
        return n;
}

int main ()
{
        int m, n, r;
        printf("m = ");
        scanf("%d", &m);
        printf("n = ");
        scanf("%d", &n);
        r = maxcommonfactor(m,n);
        printf("max common factor = %d \n", r);

}