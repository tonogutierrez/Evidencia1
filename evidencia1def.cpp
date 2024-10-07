/*
    Antonio Gutiérrez Mireles A01198527
    Alejandro Hernandez A01571408
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <unordered_map>

using namespace std;

//leer archivo
string readFile(const string& fileName) {
    ifstream archivo(fileName);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo: " << fileName << endl;
        return "";  
    }
    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    return contenido;
}

//Funcion para encontrar las posiciones de un código en un archivo
vector<int> encontrarPosicionesCodigo(const string& contenidoArchivo, const string& codigo) {
    vector<int> posiciones;
    size_t pos = contenidoArchivo.find(codigo, 0);
    while (pos != string::npos) {
        posiciones.push_back(pos);
        pos = contenidoArchivo.find(codigo, pos + 1);
    }
    return posiciones;
}

// contar ocurrencias
int contarOcurrencias(const string& texto, const string& patron) {
    int cantidad = 0;
    size_t pos = 0;
    while ((pos = texto.find(patron, pos)) != string::npos) {
        cantidad++;
        pos += patron.length(); //avanza la posición para evitar contar la misma aparición
    }
    return cantidad;
}

//Elimina cada carácter de una cadena una vez, y cuenta las apariciones en una lista de textos
unordered_map<string, vector<int>> contarSubcadenasSinCaracter(const string& cadenaOriginal, const vector<string>& documentos) {
    unordered_map<string, vector<int>> subcadenasYConteo;

    for (size_t indice = 0; indice < cadenaOriginal.size(); ++indice) {
        string subcadenaModificada = cadenaOriginal;
        subcadenaModificada.erase(subcadenaModificada.begin() + indice);  //eliminar carácter

        vector<int> conteosPorDocumento;
        for (const auto& documento : documentos) {
            int cantidad = contarOcurrencias(documento, subcadenaModificada);
            conteosPorDocumento.push_back(cantidad);
        }
        subcadenasYConteo[subcadenaModificada] = conteosPorDocumento; 
    }

    return subcadenasYConteo;
}
//Función para encontrar el palíndromo más largo usando Manacher
string prepararCadenaPalindromo(const string& texto) {
    // Ejemplo: "#h#e#l#l#o#"
    string cadenaPreparada = "#";
    for (char c : texto) {
        cadenaPreparada += c;
        cadenaPreparada += "#";
    }
    return cadenaPreparada;
}
//Manacher
pair<string, int> encontrarPalindromoMasLargo(const string& texto) {
    int posicionInicial = 0;
    int tamanoMaximo = 1;
    string palindromoMaximo = texto.substr(0, 1);
    string cadenaProcesada = prepararCadenaPalindromo(texto);
    int tamanoCadena = cadenaProcesada.size();
    vector<int> dp(tamanoCadena, 0);
    int centro = 0;
    int limiteDerecho = 0;

    for (int i = 0; i < tamanoCadena; i++) {
        if (i < limiteDerecho) {
            dp[i] = min(limiteDerecho - i, dp[2 * centro - i]);
        }
        while (i - dp[i] - 1 >= 0 && i + dp[i] + 1 < tamanoCadena && cadenaProcesada[i - dp[i] - 1] == cadenaProcesada[i + dp[i] + 1]) {
            dp[i] += 1;
        }
        if (dp[i] + i > limiteDerecho) {
            centro = i;
            limiteDerecho = dp[i] + i;
        }
        if (tamanoMaximo < dp[i]) {
            tamanoMaximo = dp[i];
            palindromoMaximo = cadenaProcesada.substr(i - dp[i], 2 * dp[i] + 1);
            palindromoMaximo.erase(remove(palindromoMaximo.begin(), palindromoMaximo.end(), '#'), palindromoMaximo.end());
            posicionInicial = (i - dp[i]) / 2;
        }
    }
    return {palindromoMaximo, posicionInicial};
}

// Longest Common Substring (LCS)
string encontrarSubcadenaComun(const string& texto1, const string& texto2) {
    int longitud1 = texto1.length();
    int longitud2 = texto2.length();
    vector<vector<int>> dp(longitud1 + 1, vector<int>(longitud2 + 1, 0));
    int longitudMaxima = 0;
    int indiceFinal = 0;
    for (int i = 1; i <= longitud1; i++) {
        for (int j = 1; j <= longitud2; j++) {
            if (texto1[i - 1] == texto2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] > longitudMaxima) {
                    longitudMaxima = dp[i][j];
                    indiceFinal = i;
                }
            } else {
                dp[i][j] = 0;
            }
        }
    }
    if (longitudMaxima == 0) {
        return "";
    }
    return texto1.substr(indiceFinal - longitudMaxima, longitudMaxima);
}
int main() {
    string transmision1 = readFile("transmission1.txt");
    string transmision2 = readFile("transmission2.txt");
    string transmision3 = readFile("transmission3.txt");
    string codigosMaliciosos = readFile("mcode.txt");

    vector<string> archivosTransmision = {transmision1, transmision2, transmision3};
    vector<string> listaCodigosMaliciosos;
    stringstream ss(codigosMaliciosos);
    string linea;

    //leer los códigos maliciosos
    while (getline(ss, linea)) {
        if (!linea.empty()) {
            listaCodigosMaliciosos.push_back(linea);
        }
    }
    ofstream archivoSalida("checking.txt");
    if (!archivoSalida) {
        cerr << "No se pudo abrir el archivo checking.txt para escribir." << endl;
        return 1;
    }

    archivoSalida << "==============\n";
    archivoSalida << "Resultados de la verificación:\n";
    archivoSalida << "==============\n\n";

    //buscar códigos maliciosos y escribir en el formato deseado
    for (const string& codigo : listaCodigosMaliciosos) {
        archivoSalida << "Código: " << codigo << "\n";
        for (size_t i = 0; i < archivosTransmision.size(); i++) {
            vector<int> posiciones = encontrarPosicionesCodigo(archivosTransmision[i], codigo);
            archivoSalida << "Transmission" << (i + 1) << ".txt ==> " << posiciones.size() << " veces\n";
            for (int pos : posiciones) {
                archivoSalida << pos << " ";
            }
            archivoSalida << "\n";
        }

        //contar subcadenas para cada código
        auto conteoSubcadenas = contarSubcadenasSinCaracter(codigo, archivosTransmision);

        //encontrar la subcadena más frecuente
        string subcadenaMasFrecuente;
        int maxFrecuencia = 0;
        string archivoConMasFrecuencia;

        //Evaluar conteos para determinar la subcadena más frecuente
        for (const auto& par : conteoSubcadenas) {
            const string& subcadena = par.first;
            const vector<int>& conteosPorArchivo = par.second;

            for (size_t j = 0; j < conteosPorArchivo.size(); ++j) {
                if (conteosPorArchivo[j] > maxFrecuencia) {
                    maxFrecuencia = conteosPorArchivo[j];
                    subcadenaMasFrecuente = subcadena;
                    archivoConMasFrecuencia = "Transmission" + to_string(j + 1) + ".txt";
                }
            }
        }
        if (maxFrecuencia > 0) {
            archivoSalida << "La subsecuencia mas encontrada es: " << subcadenaMasFrecuente 
                          << " con " << maxFrecuencia << " veces en " << archivoConMasFrecuencia << "\n";
        } else {
            archivoSalida << "No se encontró ninguna subsecuencia válida.\n";
        }
        archivoSalida << "--------------\n";
    }
    //Escribir el palíndromo más largo en cada transmisión
    pair<string, int> resultado1 = encontrarPalindromoMasLargo(transmision1);
    pair<string, int> resultado2 = encontrarPalindromoMasLargo(transmision2);
    pair<string, int> resultado3 = encontrarPalindromoMasLargo(transmision3);

    archivoSalida << "==============\n";
    archivoSalida << "Palíndromo mas grande:\n";
    archivoSalida << "Transmission1.txt ==> Posicion: " << resultado1.second << "\n" << resultado1.first << "\n";
    archivoSalida << "----\n";
    archivoSalida << "Transmission2.txt ==> Posicion: " << resultado2.second << "\n" << resultado2.first << "\n";
    archivoSalida << "----\n";
    archivoSalida << "Transmission3.txt ==> Posicion: " << resultado3.second << "\n" << resultado3.first << "\n";
    archivoSalida << "==============\n";

    //Substrings más largos entre transmisiones
    string subcadenaEntreT1yT2 = encontrarSubcadenaComun(transmision1, transmision2);
    string subcadenaEntreT1yT3 = encontrarSubcadenaComun(transmision1, transmision3);
    string subcadenaEntreT2yT3 = encontrarSubcadenaComun(transmision2, transmision3);

    archivoSalida << "Los Substring mas largos son:\n";
    archivoSalida << "T1-T2 ==> " << subcadenaEntreT1yT2 << "\n";
    archivoSalida << "T1-T3 ==> " << subcadenaEntreT1yT3 << "\n";
    archivoSalida << "T2-T3 ==> " << subcadenaEntreT2yT3 << "\n";

    archivoSalida.close();
    return 0;
}
