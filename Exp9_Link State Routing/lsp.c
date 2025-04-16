#include <stdio.h>
#include <limits.h>

#define INFINITY 9999  
#define MAX 10

int minDistance(int dist[], int visited[], int n) {
    int min = INFINITY, min_index = -1;
    for (int v = 0; v < n; v++) {
        if (!visited[v] && dist[v] <= min) {
            min = dist[v], min_index = v;
        }
    }
    return min_index;
}

void dijkstra(int graph[MAX][MAX], int src, int n, int result[MAX]) {
    int dist[n], visited[n];

    for (int i = 0; i < n; i++) {
        dist[i] = INFINITY;
        visited[i] = 0;
    }

    dist[src] = 0;

    for (int count = 0; count < n - 1; count++) {
        int u = minDistance(dist, visited, n);
        if (u == -1) break; // No more reachable nodes
        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            if (!visited[v] && graph[u][v] && dist[u] + graph[u][v] < dist[v] && graph[u][v] != INFINITY) {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }

    for (int i = 0; i < n; i++)
        result[i] = dist[i];
}

int main() {
    int n;
    printf("How many routers? ");
    scanf("%d", &n);

    int graph[MAX][MAX];
    printf("Enter the cost matrix (%d x %d):\n", n, n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            scanf("%d", &graph[i][j]);
            if (graph[i][j] == -1)
                graph[i][j] = INFINITY;
        }

    int cost_matrix[MAX][MAX];

    for (int i = 0; i < n; i++) {
        dijkstra(graph, i, n, cost_matrix[i]);
    }

    printf("\nFinal Cost Matrix:\n      ");
    for (int i = 0; i < n; i++)
        printf("%3d ", i);
    printf("\n--------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%3d | ", i);
        for (int j = 0; j < n; j++) {
            if (cost_matrix[i][j] == INFINITY)
                printf("  - ");
            else
                printf("%3d ", cost_matrix[i][j]);
        }
        printf("\n");
    }

    return 0;
}
