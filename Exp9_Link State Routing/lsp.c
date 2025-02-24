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
	int costs[n][n], i, j, k, count = 0, d, src;
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

	printf("Enter the source router: ");
	scanf("%d", &src);

	do
	{
		for(i = 0; i < n; i++)
		{
			count = 0;

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
		}
	}while(count != 0);

	printf("Minimum Distance from %d to all the other routers:\n", src);
	printf("Destination Distance\n");
	for(i = 0; i < n; i++)
	{
		printf("%11d %8d\n", i, tab[src][i].dist);
	}

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
7

0 1 1 -1 1 1 -1
1 0 1 -1 -1 -1 -1
1 1 0 1 -1 -1 -1
-1 -1 1 0 -1 -1 1
1 -1 -1 -1 0 -1 -1
1 -1 -1 -1 -1 0 1
-1 -1 -1 1 -1 1 0

2

Sample Output:
Minimum Distance from 2 to all the other routers:
Destination Distance
          0        1
          1        1
          2        0
          3        1
          4        2
          5        2
          6        2
Cost Matrix:
        0   1   2   3   4   5   6
------------------------------------------
  0 |   0   1   1   2   1   1   2
  1 |   1   0   1   2   2   2   3
  2 |   1   1   0   1   2   2   2
  3 |   2   2   1   0   3   2   1
  4 |   1   2   2   3   0   2   3
  5 |   1   2   2   2   2   0   1
  6 |   2   3   2   1   3   1   0
*/