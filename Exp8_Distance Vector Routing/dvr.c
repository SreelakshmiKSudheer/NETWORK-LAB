#include  <stdio.h>

#define INFINITY -1

typedef struct vTab
{
        int next;
        int dist;
}vTab;

int min(int xn, int ny, int xy)
{
        if(xy == INFINITY)
                if(xn == INFINITY || ny == INFINITY)
                        return INFINITY;
                else
                        return xn+ny;
        else
                if(xn == INFINITY || ny == INFINITY)
                        return xy;
                else
                        if(xn+ny < xy)
                                return xn+ny;
                        else
                                return xy;

}

int main()
{
        int N;

        printf("How many routers? ");
        scanf("%d",&N);

        const int n = N;
        int costs[n][n], i, j, k, count = 0, d;
        vTab tab[n][n];

        printf("Enter initial Routing Table:\n");
        for(i = 0; i < n; i++)
                for(j = 0; j < n; j++)
                {
                        scanf("%d",&costs[i][j]);
                        tab[i][j].dist = costs[i][j];
                        if(costs[i][j] == INFINITY)
                                tab[i][j].next = -1;
                        else
                                tab[i][j].next = j;
                }
        do
        {
                count = 0;
                for(i = 0; i < n; i++)
                {
                        
                        printf("Router %d:\n",i);
                        printf("Destination Distance NextRouter\n");
                        for(j = 0; j < n; j++)
                        {
                                if(costs[i][j] != INFINITY)
                                {
                                        for(k = 0; k < n; k++)
                                        {
                                                d = min(costs[i][j], costs[j][k], costs[i][k]);
                                                if(d < costs[i][k])
                                                {
                                                        tab[i][k].next = j;
                                                        tab[i][k].dist = d;
                                                        count++;
                                                }
                                                else
                                                {
                                                        tab[i][k].next = k;
                                                        tab[i][k].dist = d;

                                                }
                                                costs[i][k] = d;
                                        }
                                }
                        }
                        for(j = 0; j < n; j++)
                                printf("%11d %8d %10d\n", j, tab[i][j].dist, tab[i][j].next);
                }
        }while(count != 0);

        printf("Cost Matrix:\n      ");
        for(i = 0; i < n; i++)
                printf("%3d ",i);
        printf("\n------------------------------------------\n");

        for(i = 0; i < n; i++)
        {
                printf("%3d | ",i);
                for(j = 0; j < n; j++)
                        printf("%3d ",costs[i][j]);
                printf("\n");
        }
}

/*
Sample Input:
6

0 4 -1 -1 5 -1
4 0 2 -1 -1 6
-1 2 0 3 1 -1
-1 -1 3 0 -1 7
5 -1 1 -1 0 8
-1 6 -1 7 8 0

Sample Output:
Router 0:
Destination Distance NextRouter
          0        0          0
          1        4          1
          2        6          2
          3        9          3
          4        5          4
          5       10          5
Router 1:
Destination Distance NextRouter
          0        4          0
          1        0          1
          2        2          2
          3        5          3
          4        3          4
          5        6          5
Router 2:
Destination Distance NextRouter
          0        6          0
          1        2          1
          2        0          2
          3        3          3
          4        1          4
          5        8          5
Router 3:
Destination Distance NextRouter
          0        9          0
          1        5          1
          2        3          2
          3        0          3
          4        4          4
          5        7          5
Router 4:
Destination Distance NextRouter
          0        5          0
          1        3          1
          2        1          2
          3        4          3
          4        0          4
          5        8          5
Router 5:
Destination Distance NextRouter
          0       10          0
          1        6          1
          2        8          2
          3        7          3
          4        8          4
          5        0          5
Cost Matrix:
        0   1   2   3   4   5
------------------------------------------
  0 |   0   4   6   9   5  10
  1 |   4   0   2   5   3   6
  2 |   6   2   0   3   1   8
  3 |   9   5   3   0   4   7
  4 |   5   3   1   4   0   8
  5 |  10   6   8   7   8   0
*/