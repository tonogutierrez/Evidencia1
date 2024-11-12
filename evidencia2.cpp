#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <limits>
#include <tuple>
#include <set>

using namespace std;

struct Colonia {
    string nombre;
    int x, y;
    bool esCentral;
};

struct Conexion {
    int origen, destino, costo;
    bool operator<(const Conexion& otra) const {
        return costo < otra.costo;
    }
};

// Implementación Union-Find
class UnionFind {
private:
    vector<int> parent, rank;
public:
    UnionFind(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    void unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return;

        if (rank[px] < rank[py]) {
            parent[px] = py;
        } else if (rank[px] > rank[py]) {
            parent[py] = px;
        } else {
            parent[py] = px;
            rank[px]++;
        }
    }

    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

// Kruskal
vector<Conexion> kruskalMST(vector<Conexion>& conexiones, int n, int& costoTotal) {
    vector<Conexion> mst;
    sort(conexiones.begin(), conexiones.end());
    UnionFind uf(n);
    costoTotal = 0;

    for (const Conexion& conexion : conexiones) {
        if (!uf.connected(conexion.origen, conexion.destino)) {
            uf.unite(conexion.origen, conexion.destino);
            mst.push_back(conexion);
            costoTotal += conexion.costo;

            // Verificar si ya tenemos un árbol de expansión
            set<int> components;
            for (int i = 0; i < n; i++) {
                components.insert(uf.find(i));
            }
            if (components.size() == 1) break;
        }
    }
    return mst;
}

// TSP usando Branch and Bound
int tspSolver(const vector<vector<int>>& dist, vector<int>& bestPath, int n) {
    vector<int> vertices(n);
    for (int i = 0; i < n; i++) vertices[i] = i;

    int bestCost = numeric_limits<int>::max();
    vector<int> currentPath;

    do {
        if (vertices[0] != 0) continue; // Siempre empezar desde el primer vértice

        int currentCost = 0;
        bool validPath = true;

        for (int i = 0; i < n - 1; i++) {
            if (dist[vertices[i]][vertices[i + 1]] == numeric_limits<int>::max()) {
                validPath = false;
                break;
            }
            currentCost += dist[vertices[i]][vertices[i + 1]];
        }

        if (validPath && dist[vertices[n-1]][vertices[0]] != numeric_limits<int>::max()) {
            currentCost += dist[vertices[n-1]][vertices[0]];

            if (currentCost < bestCost) {
                bestCost = currentCost;
                bestPath = vertices;
            }
        }
    } while (next_permutation(vertices.begin() + 1, vertices.end()));

    return bestCost;
}

// Floyd-Warshall
void floydWarshall(vector<vector<int>>& dist, vector<vector<int>>& next, int n) {
    next.assign(n, vector<int>(n, -1));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (dist[i][j] != numeric_limits<int>::max()) {
                next[i][j] = j;
            }
        }
    }

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] != numeric_limits<int>::max() && 
                    dist[k][j] != numeric_limits<int>::max() && 
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

// Función para obtener el camino más corto
vector<int> getPath(const vector<vector<int>>& next, int i, int j) {
    if (next[i][j] == -1) return {};

    vector<int> path;
    path.push_back(i);

    while (i != j) {
        i = next[i][j];
        path.push_back(i);
    }

    return path;
}

int main() {
    int n, m, k, q;
    cin >> n >> m >> k >> q;

    vector<Colonia> colonias(n);
    for (int i = 0; i < n; i++) {
        cin >> colonias[i].nombre >> colonias[i].x >> colonias[i].y >> colonias[i].esCentral;
    }

    vector<Conexion> conexiones(m);
    for (int i = 0; i < m; i++) {
        string origen, destino;
        int costo;
        cin >> origen >> destino >> costo;

        int idxOrigen = -1, idxDestino = -1;
        for (int j = 0; j < n; j++) {
            if (colonias[j].nombre == origen) idxOrigen = j;
            if (colonias[j].nombre == destino) idxDestino = j;
        }
        conexiones[i] = {idxOrigen, idxDestino, costo};
    }

    vector<Conexion> conexionesNuevoCableado(k);
    for (int i = 0; i < k; i++) {
        string origen, destino;
        cin >> origen >> destino;

        int idxOrigen = -1, idxDestino = -1;
        for (int j = 0; j < n; j++) {
            if (colonias[j].nombre == origen) idxOrigen = j;
            if (colonias[j].nombre == destino) idxDestino = j;
        }
        conexionesNuevoCableado[i] = {idxOrigen, idxDestino, 0};
    }

    ofstream salida("checking2.txt");

    // 1. Cableado óptimo
    int costoMST;
    vector<Conexion> mst = kruskalMST(conexiones, n, costoMST);

    salida << "-------------------\n";
    salida << "1 - Cableado óptimo de nueva conexión.\n\n";
    for (const auto& conexion : mst) {
        salida << colonias[conexion.origen].nombre << " - " 
               << colonias[conexion.destino].nombre << " " 
               << conexion.costo << "\n";
    }
    salida << "\n" << "Costo Total: " << costoMST << "\n\n";
    salida << "-------------------\n";

    // 2. Ruta óptima
    vector<vector<int>> dist(n, vector<int>(n, numeric_limits<int>::max()));
    for (const auto& conexion : conexiones) {
        dist[conexion.origen][conexion.destino] = conexion.costo;
        dist[conexion.destino][conexion.origen] = conexion.costo;
    }

    vector<int> bestPath;
    int costoTSP = tspSolver(dist, bestPath, n);

    salida << "2 - La ruta óptima.\n\n";
    for (size_t i = 0; i < bestPath.size(); i++) {
        salida << colonias[bestPath[i]].nombre;
        if (i < bestPath.size() - 1) salida << " - ";
    }
    salida << " - " << colonias[bestPath[0]].nombre << "\n\n";
    salida << "La Ruta Óptima tiene un costo total de: " << costoTSP << "\n\n";
    salida << "-------------------\n";

    // 3. Caminos más cortos entre centrales
    vector<vector<int>> next;
    floydWarshall(dist, next, n);

    salida << "3 - Caminos más cortos entre centrales.\n\n";
    for (int i = 0; i < n; i++) {
        if (!colonias[i].esCentral) continue;
        for (int j = i + 1; j < n; j++) {
            if (!colonias[j].esCentral) continue;
            if (dist[i][j] != numeric_limits<int>::max()) {
                vector<int> path = getPath(next, i, j);
                for (size_t k = 0; k < path.size(); k++) {
                    salida << colonias[path[k]].nombre;
                    if (k < path.size() - 1) salida << " - ";
                }
                salida << " (" << dist[i][j] << ")\n";
            }
        }
    }
    salida << "\n" << "-------------------\n";

    // 4. Conexión de nuevas colonias
    salida << "4 - Conexión de nuevas colonias.\n\n";
    for (int i = 0; i < q; i++) {
        string nombreNueva;
        int x, y;
        cin >> nombreNueva >> x >> y;

        int menorDistancia = numeric_limits<int>::max();
        string coloniaCercana;
        for (const auto& colonia : colonias) {
            int distancia = round(sqrt(pow(x - colonia.x, 2) + pow(y - colonia.y, 2)));
            if (distancia < menorDistancia) {
                menorDistancia = distancia;
                coloniaCercana = colonia.nombre;
            }
        }
        salida << nombreNueva << " debe conectarse con " << coloniaCercana << "\n";
    }
    salida << "\n-------------------\n";

    salida.close();
    return 0;
}
