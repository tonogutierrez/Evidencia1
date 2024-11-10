#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <limits>
#include <tuple>

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

vector<Colonia> colonias;
vector<Conexion> conexiones, conexionesNuevoCableado;
vector<pair<string, pair<int, int>>> nuevasColonias;

int calcularDistancia(int x1, int y1, int x2, int y2) {
    return round(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)));
}

// Implementación Union-Find
class UnionFind {
public:
    UnionFind(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int u) {
        return (u == parent[u]) ? u : (parent[u] = find(parent[u]));
    }
    void unite(int u, int v) {
        int rootU = find(u);
        int rootV = find(v);
        if (rootU != rootV) {
            if (rank[rootU] < rank[rootV]) swap(rootU, rootV);
            parent[rootV] = rootU;
            if (rank[rootU] == rank[rootV]) rank[rootU]++;
        }
    }
private:
    vector<int> parent, rank;
};

// Algoritmo de Kruskal
int kruskalMST(vector<Conexion>& conexiones, vector<Conexion>& resultadoMST, int numColonias) {
    sort(conexiones.begin(), conexiones.end());
    UnionFind uf(numColonias);
    int totalCosto = 0;

    for (const auto& conexion : conexiones) {
        if (uf.find(conexion.origen) != uf.find(conexion.destino)) {
            uf.unite(conexion.origen, conexion.destino);
            resultadoMST.push_back(conexion);
            totalCosto += conexion.costo;
        }
    }
    return totalCosto;
}

// Algoritmo TSP básico
int tsp(int pos, int visitados, vector<vector<int>>& memo, vector<vector<int>>& distancias, vector<vector<int>>& camino) {
    if (visitados == (1 << distancias.size()) - 1) return distancias[pos][0];
    if (memo[pos][visitados] != -1) return memo[pos][visitados];

    int resultado = numeric_limits<int>::max();
    for (int i = 0; i < distancias.size(); i++) {
        if (!(visitados & (1 << i)) && distancias[pos][i] != numeric_limits<int>::max()) {
            int nuevoCosto = distancias[pos][i] + tsp(i, visitados | (1 << i), memo, distancias, camino);
            if (nuevoCosto < resultado) {
                resultado = nuevoCosto;
                camino[pos][visitados] = i;
            }
        }
    }
    return memo[pos][visitados] = resultado;
}

// Algoritmo de Floyd-Warshall para caminos mínimos
void floydWarshall(vector<vector<int>>& distancias, int n) {
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (distancias[i][k] < numeric_limits<int>::max() && distancias[k][j] < numeric_limits<int>::max())
                    distancias[i][j] = min(distancias[i][j], distancias[i][k] + distancias[k][j]);
}

// Conectar nuevas colonias
void conectarNuevasColonias(ofstream& salida) {
    salida << "-------------------\n";
    salida << "4 – Conexión de nuevas colonias.\n";

    for (const auto& nuevaColonia : nuevasColonias) {
        int menorDistancia = numeric_limits<int>::max();
        string coloniaCercana;
        for (const auto& colonia : colonias) {
            int distancia = calcularDistancia(nuevaColonia.second.first, nuevaColonia.second.second, colonia.x, colonia.y);
            if (distancia < menorDistancia) {
                menorDistancia = distancia;
                coloniaCercana = colonia.nombre;
            }
        }
        salida << nuevaColonia.first << " debe conectarse con " << coloniaCercana << endl;
    }
    salida << "-------------------\n";
}

// Imprimir la ruta TSP
void imprimirRutaTSP(int start, vector<vector<int>>& camino, int n, ofstream& salida) {
    salida << colonias[start].nombre;
    int estado = 1 << start;
    int posicion = start;
    while (camino[posicion][estado] != -1) {
        int siguiente = camino[posicion][estado];
        salida << " - " << colonias[siguiente].nombre;
        estado |= 1 << siguiente;
        posicion = siguiente;
    }
    salida << " - " << colonias[start].nombre << endl;
}

int main() {
    int n, m, k, q;
    cin >> n >> m >> k >> q;

    colonias.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> colonias[i].nombre >> colonias[i].x >> colonias[i].y >> colonias[i].esCentral;
    }

    conexiones.resize(m);
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

    conexionesNuevoCableado.resize(k);
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

    nuevasColonias.resize(q);
    for (int i = 0; i < q; i++) {
        cin >> nuevasColonias[i].first >> nuevasColonias[i].second.first >> nuevasColonias[i].second.second;
    }

    vector<Conexion> resultadoMST;
    int costoMST = kruskalMST(conexiones, resultadoMST, n);

    ofstream salida("checking2.txt");
    salida << "-------------------\n";
    salida << "1 – Cableado óptimo de nueva conexión.\n";
    for (const auto& conexion : resultadoMST) {
        salida << colonias[conexion.origen].nombre << " - " << colonias[conexion.destino].nombre << " " << conexion.costo << endl;
    }
    salida << "Costo Total: " << costoMST << endl;
    salida << "-------------------\n";

    vector<vector<int>> distancias(n, vector<int>(n, numeric_limits<int>::max()));
    for (const auto& conexion : conexiones) {
        distancias[conexion.origen][conexion.destino] = conexion.costo;
        distancias[conexion.destino][conexion.origen] = conexion.costo;
    }

    vector<vector<int>> memo(n, vector<int>((1 << n), -1));
    vector<vector<int>> camino(n, vector<int>((1 << n), -1));
    int costoTSP = tsp(0, 1, memo, distancias, camino);

    salida << "2 – La ruta óptima.\n";
    imprimirRutaTSP(0, camino, n, salida);
    salida << "La Ruta Óptima tiene un costo total de: " << costoTSP << endl;

    floydWarshall(distancias, n);
    salida << "-------------------\n";
    salida << "3 – Caminos más cortos entre centrales.\n";

    for (int i = 0; i < n; i++) {
        if (!colonias[i].esCentral) continue;
        for (int j = i + 1; j < n; j++) {
            if (colonias[j].esCentral && distancias[i][j] < numeric_limits<int>::max()) {
                salida << colonias[i].nombre << " - " << colonias[j].nombre << " (" << distancias[i][j] << ")" << endl;
            }
        }
    }

    conectarNuevasColonias(salida);
    salida.close();

    return 0;
}
