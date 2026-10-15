#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
using namespace std;

class Carta {
private:
    string valor;
    string figura;
    int puntos;

public:
    Carta() {
        valor = "";
        figura = "";
        puntos = 0;
    }

    void asignar(string v, string f, int p) {
        valor = v;
        figura = f;
        puntos = p;
    }

    string getValor() {
        return valor;
    }

    int getPuntos() {
        return puntos;
    }

    void mostrar() {
        cout << valor << " de " << figura << endl;
    }
};

class Jugador {
private:
    Carta mano[11];
    int cantidad;

public:
    Jugador() {
        cantidad = 0;
    }

    void recibirCarta(Carta carta) {
        if (cantidad < 11) {
            mano[cantidad] = carta;
            cantidad++;
        }
    }

    int calcularPuntos(int objetivo = 21) {
        int total = 0;
        int ases = 0;

        for (int i = 0; i < cantidad; i++) {
            total += mano[i].getPuntos();

            if (mano[i].getValor() == "A") {
                ases++;
            }
        }

        while (total > objetivo && ases > 0) {
            total -= 10;
            ases--;
        }

        return total;
    }

    void mostrarMano(int objetivo = 21) {
        for (int i = 0; i < cantidad; i++) {
            mano[i].mostrar();
        }

        cout << "Puntos: " << calcularPuntos(objetivo) << endl;
    }
};

class JuegoBlackjack {
private:
    Carta baraja[52];
    int siguienteCarta;
    int numeroJugadores;
    int valorGanar;
    string palos[4];

public:
    JuegoBlackjack() {
        siguienteCarta = 0;
        numeroJugadores = 0;
        valorGanar = 21;
    }

    bool cargarConfiguracion() {
        ifstream archivo("setup.txt");

        if (!archivo.is_open()) {
            cout << "Error: no se encontro setup.txt." << endl;
            return false;
        }

        string linea;

        if (!getline(archivo, linea)) {
            archivo.close();
            return false;
        }

        stringstream(linea) >> numeroJugadores;

        if (!getline(archivo, linea)) {
            archivo.close();
            return false;
        }

        stringstream lectorPalos(linea);

        for (int i = 0; i < 4; i++) {
            if (!getline(lectorPalos, palos[i], ',')) {
                cout << "Error: deben existir cuatro palos." << endl;
                archivo.close();
                return false;
            }

            size_t inicio = palos[i].find_first_not_of(" \t");
            size_t fin = palos[i].find_last_not_of(" \t");

            if (inicio == string::npos) {
                cout << "Error: nombre de palo vacio." << endl;
                archivo.close();
                return false;
            }

            palos[i] = palos[i].substr(inicio, fin - inicio + 1);
        }

        if (!getline(archivo, linea)) {
            archivo.close();
            return false;
        }

        stringstream(linea) >> valorGanar;

        archivo.close();

        if (numeroJugadores < 1 || numeroJugadores > 5) {
            cout << "La cantidad de jugadores debe ser de 1 a 5." << endl;
            return false;
        }

        if (valorGanar < 1) {
            cout << "El puntaje objetivo no es valido." << endl;
            return false;
        }

        cout << "\nConfiguracion cargada correctamente." << endl;
        cout << "Jugadores: " << numeroJugadores << endl;
        cout << "Puntaje objetivo: " << valorGanar << endl;

        return true;
    }

    void crearBaraja() {
        string valores[13] = {
            "A", "2", "3", "4", "5", "6", "7",
            "8", "9", "10", "J", "Q", "K"
        };

        int posicion = 0;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 13; j++) {
                int puntos;

                if (valores[j] == "A") {
                    puntos = 11;
                } else if (j >= 10) {
                    puntos = 10;
                } else {
                    puntos = j + 1;
                }

                baraja[posicion].asignar(
                    valores[j], palos[i], puntos
                );

                posicion++;
            }
        }
    }

    void mezclarBaraja() {
        for (int i = 0; i < 52; i++) {
            int j = rand() % 52;

            Carta temporal = baraja[i];
            baraja[i] = baraja[j];
            baraja[j] = temporal;
        }
    }

    Carta repartirCarta() {
        if (siguienteCarta >= 52) {
            return Carta();
        }

        Carta carta = baraja[siguienteCarta];
        siguienteCarta++;
        return carta;
    }

    void jugar() {
        if (!cargarConfiguracion()) {
            return;
        }

        Jugador jugadores[5];
        Jugador computadora;
        char opcion;

        crearBaraja();
        mezclarBaraja();

        cout << "\n===== BLACKJACK CONFIGURABLE =====" << endl;

        // Repartir dos cartas a cada jugador.
        for (int i = 0; i < numeroJugadores; i++) {
            jugadores[i].recibirCarta(repartirCarta());
            jugadores[i].recibirCarta(repartirCarta());
        }

        // La computadora tambien recibe dos cartas.
        computadora.recibirCarta(repartirCarta());
        computadora.recibirCarta(repartirCarta());

        // Turno de los jugadores.
        for (int i = 0; i < numeroJugadores; i++) {
            cout << "\n===== TURNO DEL JUGADOR "
                 << i + 1 << " =====" << endl;

            jugadores[i].mostrarMano(valorGanar);

            while (jugadores[i].calcularPuntos(valorGanar)
                   < valorGanar) {
                cout << "Deseas otra carta? (s/n): ";
                cin >> opcion;

                if (opcion == 's' || opcion == 'S') {
                    jugadores[i].recibirCarta(repartirCarta());

                    cout << "\nTu mano:" << endl;
                    jugadores[i].mostrarMano(valorGanar);

                    if (jugadores[i].calcularPuntos(valorGanar)
                        > valorGanar) {
                        cout << "Te pasaste del puntaje objetivo."
                             << endl;
                        break;
                    }
                } else if (opcion == 'n' || opcion == 'N') {
                    break;
                } else {
                    cout << "Opcion no valida." << endl;
                }
            }
        }

        // Turno de la computadora: regla clasica de plantarse en 17.
        cout << "\n===== TURNO DE LA COMPUTADORA =====" << endl;

        while (computadora.calcularPuntos(valorGanar) < 17) {
            computadora.recibirCarta(repartirCarta());
        }

        cout << "\nCartas de la computadora:" << endl;
        computadora.mostrarMano(valorGanar);

        int puntosComputadora =
            computadora.calcularPuntos(valorGanar);

        // Mostrar resultados individuales.
        cout << "\n===== RESULTADOS FINALES =====" << endl;

        for (int i = 0; i < numeroJugadores; i++) {
            int puntosJugador =
                jugadores[i].calcularPuntos(valorGanar);

            cout << "\nJugador " << i + 1
                 << ": " << puntosJugador << " puntos. ";

            if (puntosJugador > valorGanar) {
                cout << "Perdio por pasarse del objetivo.";
            } else if (puntosComputadora > valorGanar) {
                cout << "Gano: la computadora se paso.";
            } else if (puntosJugador > puntosComputadora) {
                cout << "Gano.";
            } else if (puntosJugador == puntosComputadora) {
                cout << "Empate.";
            } else {
                cout << "Gano la computadora.";
            }

            cout << endl;
        }
    }
};

int main() {
    srand(time(0));

    char opcion;

    do {
        JuegoBlackjack juego;
        juego.jugar();

        cout << "\n========================================" << endl;
        cout << "Deseas jugar otra partida? (s/n): ";
        cin >> opcion;

    } while (opcion == 's' || opcion == 'S');

    cout << "\nGracias por jugar Blackjack." << endl;

    return 0;
}
