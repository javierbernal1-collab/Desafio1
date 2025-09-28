#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// ---------------- ROTACIONES DE BITS ----------------
unsigned char rotarIzquierda(unsigned char valor, int n) {
    return (valor << n) | (valor >> (8 - n));
}

unsigned char rotarDerecha(unsigned char valor, int n) {
    return (valor >> n) | (valor << (8 - n));
}

// ---------------- XOR ----------------
unsigned char aplicarXOR(unsigned char valor, unsigned char clave) {
    return valor ^ clave;
}

// ---------------- DESENCRIPTADO ----------------
void desencriptar(unsigned char *entrada, unsigned char *salida, int longitud, int n, unsigned char clave) {
    for (int i = 0; i < longitud; i++) {
        unsigned char temp = aplicarXOR(entrada[i], clave);
        temp = rotarDerecha(temp, n);
        salida[i] = temp;
    }
}

// ---------------- DESCOMPRESIÓN RLE ----------------
int descomprimirRLE(char *entrada, int longitud, char *salida) {
    int j = 0;
    for (int i = 0; i < longitud; i++) {
        if (entrada[i] >= '0' && entrada[i] <= '9') {
            int cantidad = 0;
            while (i < longitud && entrada[i] >= '0' && entrada[i] <= '9') {
                cantidad = cantidad * 10 + (entrada[i] - '0');
                i++;
            }
            if (i < longitud) {
                char simbolo = entrada[i];
                for (int k = 0; k < cantidad; k++) {
                    salida[j++] = simbolo;
                }
            }
        }
    }
    salida[j] = '\0';
    return j;
}

// ---------------- DESCOMPRESIÓN LZ78 ----------------
int descomprimirLZ78(unsigned short *indices, char *chars, int n, char *salida) {
    char *diccionario[65536];
    int tamanos[65536];
    int dicSize = 0;
    int pos = 0;

    for (int i = 0; i < n; i++) {
        int idx = indices[i];
        char c = chars[i];

        if (idx == 0) {
            salida[pos++] = c;
            diccionario[dicSize] = new char[2];
            diccionario[dicSize][0] = c;
            tamanos[dicSize] = 1;
            dicSize++;
        } else {
            int len = tamanos[idx - 1];
            for (int j = 0; j < len; j++) {
                salida[pos++] = diccionario[idx - 1][j];
            }
            salida[pos++] = c;

            diccionario[dicSize] = new char[len + 1];
            for (int j = 0; j < len; j++) {
                diccionario[dicSize][j] = diccionario[idx - 1][j];
            }
            diccionario[dicSize][len] = c;
            tamanos[dicSize] = len + 1;
            dicSize++;
        }
    }
    salida[pos] = '\0';

    for (int i = 0; i < dicSize; i++) {
        delete[] diccionario[i];
    }

    return pos;
}

// ---------------- BUSCAR FRAGMENTO ----------------
bool contieneFragmento(char *texto, int lenTexto, char *fragmento, int lenFrag) {
    for (int i = 0; i <= lenTexto - lenFrag; i++) {
        int ok = 1;
        for (int j = 0; j < lenFrag; j++) {
            if (texto[i + j] != fragmento[j]) {
                ok = 0;
                break;
            }
        }
        if (ok) return true;
    }
    return false;
}

// ---------------- MAIN ----------------
int main() {
    int caso = 1;

    while (true) {
        // Construir nombres de archivo (subir un nivel a dataset/)
        stringstream nombreEnc, nombrePista;
        nombreEnc << "../dataset/Encriptado" << caso << ".txt";
        nombrePista << "../dataset/pista" << caso << ".txt";

        // Intentar abrir los archivos
        ifstream fin(nombreEnc.str().c_str());
        ifstream ffrag(nombrePista.str().c_str());

        if (!fin || !ffrag) {
            // Si alguno no existe, terminamos
            break;
        }

        // Leer archivo encriptado
        string contenido;
        char c;
        while (fin.get(c)) {
            contenido.push_back(c);
        }
        fin.close();

        int lenEnc = contenido.size();
        unsigned char *mensajeEncriptado = new unsigned char[lenEnc];
        for (int i = 0; i < lenEnc; i++) {
            mensajeEncriptado[i] = (unsigned char)contenido[i];
        }

        // Leer pista
        string fragStr;
        ffrag >> fragStr;
        ffrag.close();

        int lenFrag = fragStr.size();
        char *fragmento = new char[lenFrag + 1];
        for (int i = 0; i < lenFrag; i++) fragmento[i] = fragStr[i];
        fragmento[lenFrag] = '\0';

        // Buffers
        unsigned char *buffer = new unsigned char[lenEnc];
        char *salidaTexto = new char[200000];

        bool encontrado = false;

        for (int n = 1; n < 8 && !encontrado; n++) {
            for (int K = 0; K < 256 && !encontrado; K++) {
                desencriptar(mensajeEncriptado, buffer, lenEnc, n, (unsigned char)K);

                // ---- Intentar RLE ----
                int len = descomprimirRLE((char*)buffer, lenEnc, salidaTexto);
                if (len > 0 && contieneFragmento(salidaTexto, len, fragmento, lenFrag)) {
                    cout << "\nCaso " << caso << endl;
                    cout << "Método: RLE" << endl;
                    cout << "n = " << n << ", K = " << K << endl;
                    cout << "Texto original: " << salidaTexto << endl;
                    encontrado = true;
                    break;
                }

                // ---- Intentar LZ78 ----
                int nPares = lenEnc / 3;
                if (nPares > 0) {
                    unsigned short *indices = new unsigned short[nPares];
                    char *chars = new char[nPares];
                    for (int i = 0; i < nPares; i++) {
                        indices[i] = ((unsigned short)buffer[i * 3] << 8) | buffer[i * 3 + 1];
                        chars[i] = buffer[i * 3 + 2];
                    }
                    int lenLZ = descomprimirLZ78(indices, chars, nPares, salidaTexto);
                    delete[] indices;
                    delete[] chars;

                    if (lenLZ > 0 && contieneFragmento(salidaTexto, lenLZ, fragmento, lenFrag)) {
                        cout << "\nCaso " << caso << endl;
                        cout << "Método: LZ78" << endl;
                        cout << "n = " << n << ", K = " << K << endl;
                        cout << "Texto original: " << salidaTexto << endl;
                        encontrado = true;
                        break;
                    }
                }
            }
        }

        if (!encontrado) {
            cout << "\nCaso " << caso << ": No se encontró coincidencia con el fragmento." << endl;
        }

        delete[] mensajeEncriptado;
        delete[] buffer;
        delete[] salidaTexto;
        delete[] fragmento;

        caso++; // Pasar al siguiente archivo
    }

    return 0;
}
