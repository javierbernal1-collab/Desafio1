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

int main() {
    unsigned char mensaje[5] = {65, 66, 67, 68, 69}; // Simulación (A B C D E)
    unsigned char salida[5];

    int n = 3;              // rotaciones
    unsigned char clave = 0x5A;  // clave XOR

    // Desencriptar manualmente (ejemplo pequeño)
    for (int i = 0; i < 5; i++) {
        unsigned char temp = aplicarXOR(mensaje[i], clave);
        temp = rotarDerecha(temp, n);
        salida[i] = temp;
    }

    cout << "Mensaje desencriptado: ";
    for (int i = 0; i < 5; i++) {
        cout << (int)salida[i] << " ";
    }
    cout << endl;

    return 0;
}
