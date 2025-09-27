#include <iostream>
using namespace std;

// Rotar bits a la izquierda
unsigned char rotarIzquierda(unsigned char valor, int n) {
    return (valor << n) | (valor >> (8 - n));
}

// Rotar bits a la derecha
unsigned char rotarDerecha(unsigned char valor, int n) {
    return (valor >> n) | (valor << (8 - n));
}

// XOR
unsigned char aplicarXOR(unsigned char valor, unsigned char clave) {
    return valor ^ clave;
}

// Función para desencriptar un arreglo
void desencriptar(unsigned char *entrada, unsigned char *salida, int longitud, int n, unsigned char clave) {
    for (int i = 0; i < longitud; i++) {
        unsigned char temp = aplicarXOR(entrada[i], clave);
        temp = rotarDerecha(temp, n);
        salida[i] = temp;
    }
}

int main() {
    unsigned char mensaje[5] = {65, 66, 67, 68, 69}; // Simulación (A B C D E)
    unsigned char salida[5];

    int n = 3;              // rotaciones
    unsigned char clave = 0x5A;  // clave XOR

    desencriptar(mensaje, salida, 5, n, clave);


    cout << "Mensaje desencriptado: ";
    for (int i = 0; i < 5; i++) {
        cout << (int)salida[i] << " ";
    }
    cout << endl;

    return 0;
}
