#include <iostream>
#include <fstream> //para leer archivos
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <regex>

using namespace std;

//Leo el file 
string readFile(const string& fileName) {
    ifstream archivo(fileName);
    // Si no se encuentra el archivo o hay un error al abrirlo
    if (!archivo) {
        cerr << "No se pudo abrir este archivo: " << fileName << endl;
        return "";  // Devolver cadena vacía en caso de error
    }
    string text((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    return text;  // Devolver el contenido leído
}

// Función para encontrar las posiciones del código malicioso en un archivo
vector<int> findCodePositions(const string& fileContent, const string& code) {
    vector<int> positions;
    size_t pos = fileContent.find(code, 0);
    while (pos != string::npos) {
        positions.push_back(pos);
        pos = fileContent.find(code, pos + 1);
    }
    return positions;
}

// Función para escribir las posiciones encontradas en un archivo de salida
void writeCodePositions(ofstream& offfile, const string& fileName, const vector<int>& positions) {
    offfile << "Posiciones del código malicioso en " << fileName << ":\n";
    if (positions.empty()) {
        offfile << "No se encontró el código malicioso en este archivo.\n";
    } else {
        for (int pos : positions) {
            offfile << pos << " ";
        }
        offfile << "\n";
    }
    offfile << "---------------\n";
}

//Metodo mas eficiente para encontrar el palindromo es el de Manacher 
//constante para que no se modifique el string
string juntarString(const string& text){
    string newString = "#";
    for (char c : text) {
        newString += c;
        newString += "#";
    }
    return newString;
}

pair<string, int> palindromoManacher(const string& texto){

    int startPos = 0;
    int maxSize = 1;
    string maxStr = texto.substr(0,1);

    string stringTemp = juntarString(texto);
    int sizeNewString = stringTemp.size();
    vector<int> dp(sizeNewString,0);
    int center = 0;
    int right = 0;

    for(int i=0; i < sizeNewString; i++ ){
        if (i < right){
            dp[i] = min(right-i, dp[2*center-i]);
        }
        while(i-dp[i] - 1 >= 0 && i+dp[i]+1 < sizeNewString && stringTemp[i-dp[i]-1] == stringTemp[i+dp[i]+1]){
            dp[i] += 1;
        }
        if(dp[i]+i > right){
            center = i;
            right = dp[i]+i;
        }
        if(maxSize < dp[i]){
            maxSize = dp[i];
            //Obtener la subcadena del palíndromo más largo
            maxStr = stringTemp.substr(i-dp[i],2*dp[i]+1);
            // Eliminar los caracteres '#' para obtener la cadena original
            maxStr.erase(remove(maxStr.begin(), maxStr.end(), '#'), maxStr.end());
            //Calcular la posición de inicio en la cadena original
            startPos = (i - dp[i]) / 2;
        }
    }
    return {maxStr, startPos};

}

string solve(string w1, string w2) {
    int n = w1.length();
    int m = w2.length();

    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));
    int maxlength = 0;  // Longitud máxima de la subcadena común
    int endIndex = 0;   // Índice donde termina la subcadena común más larga en w1

    // Lleno la matriz DP
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (w1[i - 1] == w2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;

                // Actualizo el tamaño máximo y el índice de fin si encuentro una subcadena más larga
                if (dp[i][j] > maxlength) {
                    maxlength = dp[i][j];
                    endIndex = i;  // Guardo el índice donde termina la subcadena en w1
                }
            } else {
                dp[i][j] = 0;
            }
        }
    }

    // Si no hay subcadena común, retornamos una cadena vacía
    if (maxlength == 0) {
        return "";
    }

    // Reconstruir la subcadena más larga usando el índice de fin
    return w1.substr(endIndex - maxlength, maxlength);
}
int main(){

    string transmission1 = readFile("transmission1.txt");
    string transmission2 = readFile("transmission2.txt");
    string transmission3 = readFile("transmission3.txt");
    string mcode = readFile("mcode.txt");
    

    // Dividir el contenido de mcode.txt en líneas (cada línea es un código malicioso)
    vector<string> maliciousCodes;
    stringstream ss(mcode);
    string line;
    while (getline(ss, line)) {
        if (!line.empty()) {
            maliciousCodes.push_back(line);
        }
    }
    cout << "Obteniendo los datos..............." << endl;

    ofstream offfile("checking.txt");
    //string texto = "abacdfgdcaba";
    pair<string, int> resultado = palindromoManacher(transmission1);
    pair<string, int> resultado2 = palindromoManacher(transmission2);
    pair<string, int> resultado3 = palindromoManacher(transmission3);
    //Subcadena
    string resultado4 = solve(transmission1,transmission2);
    string resultado5 = solve(transmission1,transmission3);
    string resultado6 = solve(transmission2,transmission3);
    // Encontrar código malicioso
    vector<int> posiciones1 = findCodePositions(transmission1, mcode);
    vector<int> posiciones2 = findCodePositions(transmission2, mcode);
    vector<int> posiciones3 = findCodePositions(transmission3, mcode);

    if (!offfile) {
        cerr << "No se pudo abrir el archivo verificacion.txt para escribir." << endl;
        return 1;
    }

    offfile << "==============\n";
    offfile << "Resultados de la verificación:\n";
    offfile << "==============\n\n";

    offfile << "==============\n";
    offfile << "Resultados de la verificación del código malicioso:\n";
    offfile << "==============\n\n";
    // Para cada código malicioso, buscarlo en los tres archivos de transmisión
    for (const string& code : maliciousCodes) {
        offfile << "Buscando el código malicioso: " << code << "\n";
        offfile << "==============\n";
        vector<int> positions1 = findCodePositions(transmission1, code);
        writeCodePositions(offfile, "transmission1.txt", positions1);

        vector<int> positions2 = findCodePositions(transmission2, code);
        writeCodePositions(offfile, "transmission2.txt", positions2);

        vector<int> positions3 = findCodePositions(transmission3, code);
        writeCodePositions(offfile, "transmission3.txt", positions3);
    }
    // Escribir el palíndromo más largo
    offfile << "====================\n";
    offfile <<" PALINDROMO\n";
    offfile << "====================\n";
    offfile << "Palindromo más largo en transmission1.txt:\n";
    offfile << "Posicion de inicio: " << resultado.second<< endl;
    offfile << "Palíndromo: " << resultado.first << endl;
    offfile << "----\n";

    offfile << "Palindromo más largo en transmission2.txt:\n";
    offfile << "Posicion de inicio: " << resultado2.second<< endl;
    offfile << "Palindromo: " << resultado2.first << endl;
    offfile << "----\n";

    offfile << "Palindromo más largo en transmission3.txt:\n";
    offfile << "Posicion de inicio: " << resultado3.second<< endl;
    offfile << "Palindromo: " << resultado3.first << endl;
    offfile << "====================\n";
    offfile << "SUBCADENAS:\n";
    offfile << "====================\n";

    // Escribir la subcadena común más larga
    offfile << "Subcadena comun mas larga entre transmission1.txt y transmission2.txt:\n";
    offfile << "Subcadena: " << resultado4 << endl;
    offfile << "----\n";

    offfile << "Subcadena comun mas larga entre transmission1.txt y transmission3.txt:\n";
    offfile << "Subcadena: " << resultado5 << endl;
    offfile << "----\n";

    offfile << "Subcadena comun mas larga entre transmission2.txt y transmission3.txt:\n";
    offfile << "Subcadena: " << resultado6 << endl;
    offfile << "----\n";

    // Cerrar el archivo
    offfile.close();

    cout << "Los resultados han sido guardados en checking.txt." << endl;
    return 0;
}