#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <climits>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

// Constantes y estructuras
const int MAX = 1;
const int MIN = -1;
const int FILAS = 6;
const int COLUMNAS = 7;

struct NodoTablero {
    int tablero[FILAS][COLUMNAS];
    int columna;
    int valor;
    vector<NodoTablero*> hijos;

    NodoTablero(int col) : columna(col), valor(0) {
        // No es necesario inicializar el tablero aquí
    }

    ~NodoTablero() {
        for (auto hijo : hijos) {
            delete hijo;
        }
    }
};

// Declaraciones de funciones
int negamax(NodoTablero* nodo, int jugador, int profundidad, int alfa, int beta);
int evalua_jugada(int tablero[FILAS][COLUMNAS], int jugador);
pair<int, int> comprueba_linea(int tablero[FILAS][COLUMNAS], int n, int jugador);
bool game_over(int tablero[FILAS][COLUMNAS], int jugador);
void ver_tablero(int tablero[FILAS][COLUMNAS]);
bool inserta_ficha(int tablero[FILAS][COLUMNAS], int columna, int jugador);
int juega_humano(int tablero[FILAS][COLUMNAS]);
int juega_ordenador(int tablero[FILAS][COLUMNAS], int dificultad);
void generar_hijos(NodoTablero* nodo, int jugador);
void guardar_partida_csv(int tablero[FILAS][COLUMNAS], int jugador, int dificultad);
bool cargar_partida_csv(int tablero[FILAS][COLUMNAS], int& jugador, int& dificultad);
void ver_partidas_guardadas();
bool bloquea_ganador(int tablero[FILAS][COLUMNAS], int columna, int jugador);

int main() {
    int tablero[FILAS][COLUMNAS] = {{0}};
    int jugador = MIN;
    int dificultad;
    string gana = "Tablas";

    bool salir = false;

    while (!salir) {
        cout << "Menú Principal" << endl;
        cout << "1. Nueva Partida" << endl;
        cout << "2. Cargar Partida" << endl;
        cout << "3. Ver Partidas Guardadas" << endl;
        cout << "4. Salir" << endl;
        cout << "Ingrese su opción: ";

        int opcion;
        cin >> opcion;

        auto ganador_info = comprueba_linea(tablero, 4, jugador);
        int g = ganador_info.first;

        switch (opcion) {
            case 1:
                // Nueva partida
                cout << "Seleccione la dificultad:" << endl;
                cout << "1. Fácil" << endl;
                cout << "2. Medio" << endl;
                cout << "3. Difícil" << endl;
                cout << "Ingrese su opción: ";
                cin >> dificultad;

                if (dificultad < 1 || dificultad > 3) {
                    cout << "Opción no válida. Seleccionando dificultad fácil por defecto." << endl;
                    dificultad = 1;
                }

                // Reiniciar variables relacionadas con la partida
                jugador = MIN;
                gana = "Tablas";
                // Reiniciar el tablero
                for (int i = 0; i < FILAS; ++i) {
                    for (int j = 0; j < COLUMNAS; ++j) {
                        tablero[i][j] = 0;
                    }
                }

                // Dentro del bucle principal donde juegan el humano y la IA
                while (true) {
                    ver_tablero(tablero);

                    // Turno del jugador humano
                    int col = juega_humano(tablero);

                    if (col == -1) {
                        guardar_partida_csv(tablero, jugador, dificultad);
                        salir = true;
                        break;
                    }

                    // Verificar si el jugador humano ha ganado después de su movimiento
                    ganador_info = comprueba_linea(tablero, 4, MIN);
                    g = ganador_info.first;
                    if (g != 0) {
                        gana = "Jugador";
                        break;
                    }

                    // Turno de la IA
                    int movimientoOrdenador = juega_ordenador(tablero, dificultad);

                    // Verificar si la IA ha ganado después de su movimiento
                    ganador_info = comprueba_linea(tablero, 4, MAX);
                    g = ganador_info.first;
                    if (g != 0) {
                        gana = "Ordenador";
                        break;
                    }

                    if (game_over(tablero, MIN)) {
                        break;
                    }
                }

                ver_tablero(tablero);

                ganador_info = comprueba_linea(tablero, 4, jugador);
                g = ganador_info.first;

                if (g == jugador) {
                    gana = "Jugador";
                } else if (g == MIN) {
                    gana = "Ordenador";
                } else {
                    gana = "Tablas";
                }

                cout << "Ganador: " << gana << endl;
                break;


            case 2:
                // Cargar partida
                if (cargar_partida_csv(tablero, dificultad, jugador)) {
                    cout << "Partida cargada exitosamente." << endl;
                    // Ya no necesitas asignar el valor de jugador aquí
                    break;
                } else {
                    cout << "No hay partidas guardadas o no se pudo cargar la partida." << endl;
                }
                break;



            case 3:
                // Ver partidas guardadas
                ver_partidas_guardadas();
                break;

            case 4:
                // Salir
                salir = true;
                break;

            default:
                cout << "Opción no válida. Por favor, ingrese una opción válida." << endl;
                break;
        }
    }

    return 0;
}

int negamax(NodoTablero* nodo, int jugador, int profundidad, int alfa, int beta) {
    if (game_over(nodo->tablero, jugador) || profundidad == 0) {
        return evalua_jugada(nodo->tablero, jugador);
    }

    generar_hijos(nodo, jugador);

    int max_puntuacion = INT_MIN;

    for (auto hijo : nodo->hijos) {
        int puntuacion = -negamax(hijo, jugador * -1, profundidad - 1, -beta, -alfa);

        if (puntuacion > max_puntuacion) {
            max_puntuacion = puntuacion;
            nodo->columna = hijo->columna;
            nodo->valor = max_puntuacion;
        }

        if (max_puntuacion >= beta) {
            break;
        }

        if (max_puntuacion > alfa) {
            alfa = max_puntuacion;
        }
    }

    return max_puntuacion;
}

int evalua_jugada(int tablero[FILAS][COLUMNAS], int jugador) {
    auto ganador_info = comprueba_linea(tablero, 4, jugador);
    int g = ganador_info.first;

    if (g == jugador) {
        return 100;
    } else if (g == MIN) {
        return -100;
    } else {
        return 0;
    }
}

pair<int, int> comprueba_linea(int tablero[FILAS][COLUMNAS], int n, int jugador) {
    int ganador = 0;
    int num_lineas = 0;
    int lineas_posibles = 8 - n;

    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            // Buscar línea horizontal
            if (j <= COLUMNAS - n) {
                bool linea_horizontal = true;
                for (int k = 0; k < n; ++k) {
                    if (tablero[i][j + k] != jugador) {
                        linea_horizontal = false;
                        break;
                    }
                }
                if (linea_horizontal && (j == 0 || tablero[i][j - 1] != jugador) &&
                    (j + n == COLUMNAS || tablero[i][j + n] != jugador)) {
                    ganador = jugador;
                    num_lineas++;
                }
            }

            // Buscar línea vertical
            if (i <= FILAS - n) {
                bool linea_vertical = true;
                for (int k = 0; k < n; ++k) {
                    if (tablero[i + k][j] != jugador) {
                        linea_vertical = false;
                        break;
                    }
                }
                if (linea_vertical && (i == 0 || tablero[i - 1][j] != jugador) &&
                    (i + n == FILAS || tablero[i + n][j] != jugador)) {
                    ganador = jugador;
                    num_lineas++;
                }
            }

            // Buscar líneas diagonales
            if (i <= FILAS - n && j <= COLUMNAS - n) {
                bool diagonal1 = true;
                bool diagonal2 = true;
                for (int k = 0; k < n; ++k) {
                    if (tablero[i + k][j + k] != jugador) {
                        diagonal1 = false;
                    }
                    if (tablero[i + k][j + n - k - 1] != jugador) {
                        diagonal2 = false;
                    }
                }
                if (diagonal1 && (i == 0 || j == 0 || tablero[i - 1][j - 1] != jugador) &&
                    (i + n == FILAS || j + n == COLUMNAS || tablero[i + n][j + n] != jugador)) {
                    ganador = jugador;
                    num_lineas++;
                }
                if (diagonal2 && (i == 0 || j == COLUMNAS - n || tablero[i - 1][j + n] != jugador) &&
                    (i + n == FILAS || j - 1 == 0 || tablero[i + n][j - 1] != jugador)) {
                    ganador = jugador;
                    num_lineas++;
                }
            }
        }
    }

    return make_pair(ganador, num_lineas);
}

bool game_over(int tablero[FILAS][COLUMNAS], int jugador) {
    auto ganador_info = comprueba_linea(tablero, 4, jugador);

    int ganador = ganador_info.first;

    if (ganador != 0) {
        return true;
    }

    // Verificar si el tablero está lleno
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            if (tablero[i][j] == 0) {
                return false;
            }
        }
    }

    return true;
}

void ver_tablero(int tablero[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            if (tablero[i][j] == MAX) {
                cout << 'X' << ' ';
            } else if (tablero[i][j] == MIN) {
                cout << 'O' << ' ';
            } else {
                cout << ". ";
            }
        }
        cout << endl;
    }
    cout << "-------------" << endl;
    cout << "1 2 3 4 5 6 7" << endl;
}

bool inserta_ficha(int tablero[FILAS][COLUMNAS], int columna, int jugador) {
    for (int i = FILAS - 1; i >= 0; --i) {
        if (tablero[i][columna] == 0) {
            tablero[i][columna] = jugador;
            return true;
        }
    }

    return false;
}

int juega_humano(int tablero[FILAS][COLUMNAS]) {
    int col;
    bool movimiento_valido = false;

    while (!movimiento_valido) {
        cout << "Columna (0 para salir, -1 para guardar): ";
        cin >> col;

        if (col == -1) {
            guardar_partida_csv(tablero, MIN, 0);
            cout << "Partida guardada exitosamente. Volviendo al menú principal." << endl;
            return col;
        } else if (col == 0 || col < 1 || col > COLUMNAS || !inserta_ficha(tablero, col - 1, MIN)) {
            cout << "Movimiento ilegal. Intente de nuevo." << endl;
        } else {
            movimiento_valido = true;
        }
    }

    return col - 1;
}

int juega_ordenador(int tablero[FILAS][COLUMNAS], int dificultad) {
    srand(time(nullptr));

    int col;

    switch (dificultad) {
        case 1: // Fácil
            // En fácil, la IA realiza movimientos aleatorios sin mucha estrategia
            do {
                col = rand() % COLUMNAS;
            } while (tablero[0][col] != 0);

            inserta_ficha(tablero, col, MAX);
            return col;

        case 2: // Medio
            // En medio, la IA realiza movimientos que a veces bloquean jugadas del oponente
            for (int i = 0; i < COLUMNAS; ++i) {
                if (tablero[0][i] == 0) {
                    // Verificar si bloquear al oponente evita que gane en la próxima jugada
                    if (bloquea_ganador(tablero, i, MIN)) {
                        inserta_ficha(tablero, i, MAX);
                        return i;
                    }
                }
            }

            // Si no puede bloquear al oponente, realiza un movimiento aleatorio
            do {
                col = rand() % COLUMNAS;
            } while (tablero[0][col] != 0);

            inserta_ficha(tablero, col, MAX);
            return col;

        case 3: // Difícil
            // En difícil, la IA utiliza el algoritmo negamax y bloquea jugadas del oponente
            NodoTablero* nodo = new NodoTablero(0);
            nodo->valor = INT_MIN;

            for (int i = 0; i < COLUMNAS; ++i) {
                if (tablero[0][i] == 0) {
                    NodoTablero* hijo = new NodoTablero(i);
                    hijo->valor = evalua_jugada(tablero, MAX);
                    inserta_ficha(hijo->tablero, i, MAX);
                    nodo->hijos.push_back(hijo);
                }
            }

            for (auto hijo : nodo->hijos) {
                int puntuacion = -negamax(hijo, MAX, 5, INT_MIN, INT_MAX);
                if (puntuacion > nodo->valor) {
                    nodo->valor = puntuacion;
                    col = hijo->columna;
                }
            }

            inserta_ficha(tablero, col, MAX);
            delete nodo;

            return col;
    }

    return -1; // Error
}

void generar_hijos(NodoTablero* nodo, int jugador) {
    for (int i = 0; i < COLUMNAS; ++i) {
        if (nodo->tablero[0][i] == 0) {
            NodoTablero* hijo = new NodoTablero(i);
            for (int j = 0; j < FILAS; ++j) {
                for (int k = 0; k < COLUMNAS; ++k) {
                    hijo->tablero[j][k] = nodo->tablero[j][k];
                }
            }
            inserta_ficha(hijo->tablero, i, jugador);
            nodo->hijos.push_back(hijo);
        }
    }
}

bool bloquea_ganador(int tablero[FILAS][COLUMNAS], int columna, int jugador) {
    int filas = FILAS;
    int fil = 0;
    while (fil < filas && tablero[fil][columna] != 0) {
        ++fil;
    }

    if (fil == filas) {
        return false; // La columna está llena
    }

    // Intentar bloquear una victoria del jugador en la columna
    tablero[fil][columna] = jugador;
    auto ganador_info = comprueba_linea(tablero, 4, jugador);

    if (ganador_info.first == jugador) {
        // Bloquear la victoria del jugador
        tablero[fil][columna] = 0;
        return true;
    }

    // Deshacer el movimiento
    tablero[fil][columna] = 0;

    return false;
}

void guardar_partida_csv(int tablero[FILAS][COLUMNAS], int jugador, int dificultad) {
    ofstream archivo("partida_guardada.csv");

    if (archivo.is_open()) {
        archivo << jugador << "," << dificultad << endl;
        for (int i = 0; i < FILAS; ++i) {
            for (int j = 0; j < COLUMNAS; ++j) {
                archivo << tablero[i][j] << ",";
            }
            archivo << endl;
        }

        archivo.close();
    } else {
        cout << "No se pudo abrir el archivo para guardar la partida." << endl;
    }
}

bool cargar_partida_csv(int tablero[FILAS][COLUMNAS], int& jugador, int& dificultad) {
    ifstream archivo("partida_guardada.csv");

    if (archivo.is_open()) {
        int seleccion;
        int partida = 1; // Para numerar las partidas
        string linea;

        cout << "Seleccione una partida para cargar:" << endl;

        while (getline(archivo, linea)) {
            cout << partida << ": " << linea << endl;
            partida++;
        }

        cout << "Ingrese el número de la partida (o -1 para cancelar): ";
        cin >> seleccion;

        if (seleccion == -1) {
            return false; // Cancelar la carga de partida
        }

        // Reiniciar el archivo y leer la partida seleccionada
        archivo.clear();
        archivo.seekg(0, ios::beg);
        partida = 1;

        while (getline(archivo, linea)) {
            if (partida == seleccion) {
                break;
            }
            partida++;
        }

        if (partida != seleccion) {
            cout << "Número de partida no válido." << endl;
            archivo.close();
            return false;
        }

        istringstream ss(linea);
        ss >> jugador >> dificultad;

        for (int i = 0; i < FILAS; ++i) {
            for (int j = 0; j < COLUMNAS; ++j) {
                ss >> tablero[i][j];
            }
        }

        archivo.close();
        return true;
    } else {
        cout << "No se pudo abrir el archivo para cargar la partida." << endl;
        return false;
    }
}

void ver_partidas_guardadas() {
    ifstream archivo("partida_guardada.csv");

    if (archivo.is_open()) {
        int partida = 1; // Para numerar las partidas
        string linea;

        while (getline(archivo, linea)) {
            cout << "Partida " << partida << ": " << linea << endl;
            partida++;
        }

        archivo.close();
    } else {
        cout << "No se pudo abrir el archivo para ver las partidas guardadas." << endl;
    }
}
