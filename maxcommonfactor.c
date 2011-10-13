/* write the program to test examples in <<The Art of Computer Programming>>
 * huangjun 2011-10-13
 * hjwsm1989@gmail.com
 */
#include <stdio.h>
#include <stdlib.h>

int main ()
{
  int m, n, r;
  r = 0;
  printf("Please Enter m and n \n");
  scanf("%d%d", m, n);

  if (m < n)
  {
    int temp;
    temp = m;
    m = n;
    n = temp;
  }
  
  while (1) {
    r = m % n;
    if (r == 0)
      return n;
    m = n;
    n = r;
  };
  
  return 0;
}