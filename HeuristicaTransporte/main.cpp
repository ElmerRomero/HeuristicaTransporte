#include <iostream>
#include <algorithm>
#include <iomanip>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

using namespace std;

#define INF 1000000 // Un valor suficientemente grande para simular infinito

struct Asignacion {
    int fila;
    int columna;
    int valor;
};

void heuristicaPersonalizada(int* ofertas, int* demandas, int** costos, int** transporte, int n_origenes, int n_destinos, int& variables_basicas, Asignacion asignaciones[], int& total_asignaciones) {
    // Inicializar la matriz de transporte con ceros
    for (int i = 0; i < n_origenes; ++i) {
        for (int j = 0; j < n_destinos; ++j) {
            transporte[i][j] = 0;
        }
    }

    variables_basicas = 0;
    total_asignaciones = 0;
    int max_variables_basicas = n_origenes + n_destinos - 1;
    int i = 0, j = 0;

    while (variables_basicas < max_variables_basicas) {
        // Encontrar la celda con el costo más bajo en la fila o columna actuales
        int min_cost = costos[i][j];
        int min_i = i, min_j = j;

        for (int k = 0; k < n_destinos; ++k) {
            if (costos[i][k] < min_cost && demandas[k] > 0) {
                min_cost = costos[i][k];
                min_i = i;
                min_j = k;
            }
        }

        for (int k = 0; k < n_origenes; ++k) {
            if (costos[k][j] < min_cost && ofertas[k] > 0) {
                min_cost = costos[k][j];
                min_i = k;
                min_j = j;
            }
        }

        // Asignar la cantidad máxima posible a esa celda
        int cantidad = min(ofertas[min_i], demandas[min_j]);
        transporte[min_i][min_j] = cantidad;

        // Almacenar la asignación
        asignaciones[total_asignaciones++] = {min_i, min_j, cantidad};

        // Actualizar las ofertas y demandas
        ofertas[min_i] -= cantidad;
        demandas[min_j] -= cantidad;

        // Eliminar filas o columnas agotadas, priorizando eliminar solo una
        if (ofertas[min_i] == 0 && demandas[min_j] == 0) {
            if (variables_basicas + 1 < max_variables_basicas) {
                // Si tanto la fila como la columna están agotadas, eliminar una y asignar 0 a una celda adyacente no eliminada
                for (int k = 0; k < n_destinos; ++k) {
                    costos[min_i][k] = INF;
                }
                asignaciones[total_asignaciones++] = {min_i, (min_j == n_destinos - 1) ? min_j - 1 : min_j + 1, 0};
                for (int k = 0; k < n_origenes; ++k) {
                    costos[k][min_j] = INF;
                }
                variables_basicas++;
            } else if (ofertas[min_i] == 0) {
                for (int k = 0; k < n_destinos; ++k) {
                    costos[min_i][k] = INF;
                }
            } else {
                for (int k = 0; k < n_origenes; ++k) {
                    costos[k][min_j] = INF;
                }
            }
        } else if (ofertas[min_i] == 0) {
            for (int k = 0; k < n_destinos; ++k) {
                costos[min_i][k] = INF;
            }
        } else if (demandas[min_j] == 0) {
            for (int k = 0; k < n_origenes; ++k) {
                costos[k][min_j] = INF;
            }
        }

        // Incrementar el número de variables básicas
        variables_basicas++;

        // Si la celda de inicio no ha sido eliminada, buscar otro mínimo en la fila o columna
        if (costos[i][j] != INF) {
            continue;
        }

        // Si la celda de inicio fue eliminada, pasar a la siguiente posición adyacente que no haya sido eliminada
        do {
            if (j < n_destinos - 1) {
                j++;
            } else if (i < n_origenes - 1) {
                i++;
                j = 0;
            } else {
                break;
            }
        } while (costos[i][j] == INF);
    }
}
void verificarDatos(int ofertas[], int n_origenes, int demandas[], int n_destinos) {
    bool datosValidos = true;

    for (int i = 0; i < n_origenes; ++i) {
        if (ofertas[i] <= 0) {
            cout << "Error: La oferta " << i+1 << " debe ser un número positivo." << endl;
            datosValidos = false;
        }
    }

    for (int i = 0; i < n_destinos; ++i) {
        if (demandas[i] <= 0) {
            cout << "Error: La demanda " << i+1 << " debe ser un número positivo." << endl;
            datosValidos = false;
        }
    }

    if (!datosValidos) {
        cout << "Hay errores en los datos ingresados. Por favor, ingrese valores positivos." << endl;
        exit(1); // Terminar el programa en caso de errores
    }
}



int main() {
    int n_origenes, n_destinos;

    cout << CYAN << "\n-----------------------------------------" << RESET << endl;
    cout << GREEN << "  Programa de Optimización de Transporte" << RESET << endl;
    cout << CYAN << "-----------------------------------------" << RESET << endl;

    cout << "Ingrese el número de orígenes (filas): ";
    cin >> n_origenes;
    cout << "Ingrese el número de destinos (columnas): ";
    cin >> n_destinos;

    // Asignación dinámica de memoria
    int* ofertas = new int[n_origenes];
    int* demandas = new int[n_destinos];
    int** costos = new int*[n_origenes];
    int** transporte = new int*[n_origenes];
    int** costos_originales = new int*[n_origenes];
    for (int i = 0; i < n_origenes; ++i) {
        costos[i] = new int[n_destinos];
        transporte[i] = new int[n_destinos];
        costos_originales[i] = new int[n_destinos];
    }

    cout << CYAN << "\n-----------------------------------------" << RESET << endl;
    cout << YELLOW << "          Entrada de Datos" << RESET << endl;
    cout << CYAN << "-----------------------------------------" << RESET << endl;

    cout << "Ingrese las " << n_origenes << " ofertas: ";
    for (int i = 0; i < n_origenes; ++i) {
        cin >> ofertas[i];
    }

    cout << "Ingrese las " << n_destinos << " demandas: ";
    for (int i = 0; i < n_destinos; ++i) {
        cin >> demandas[i];
    }

    cout << "Ingrese los costos:" << endl;
    for (int i = 0; i < n_origenes; ++i) {
        for (int j = 0; j < n_destinos; ++j) {
            cin >> costos[i][j];
            costos_originales[i][j] = costos[i][j];
        }
    }

    cout << CYAN << "\n-----------------------------------------" << RESET << endl;
    cout << MAGENTA << "          Datos Ingresados" << RESET << endl;
    cout << CYAN << "-----------------------------------------" << RESET << endl;

    cout << "Ofertas: ";
    for (int i = 0; i < n_origenes; ++i) {
        cout << ofertas[i] << " ";
    }
    cout << endl;

    cout << "Demandas: ";
    for (int i = 0; i < n_destinos; ++i) {
        cout << demandas[i] << " ";
    }
    cout << endl;

    cout << "\nMatriz de costos:" << endl;
    cout << "   ";
    for (int j = 0; j < n_destinos; ++j) {
        cout << setw(7) << YELLOW << "D" << j+1 << RESET;
    }
    cout << endl;

    for (int i = 0; i < n_origenes; ++i) {
        cout << YELLOW << "O" << i+1 << RESET << " ";
        for (int j = 0; j < n_destinos; ++j) {
            cout << setw(8) << BLUE << costos[i][j] << RESET;
        }
        cout << endl;
    }

        // Verificar los datos ingresados
    verificarDatos(ofertas, n_origenes, demandas, n_destinos);

    int variables_basicas;
    Asignacion asignaciones[100];
    int total_asignaciones;

    int suma_ofertas = 0, suma_demandas = 0;
    for (int i = 0; i < n_origenes; ++i) suma_ofertas += ofertas[i];
    for (int i = 0; i < n_destinos; ++i) suma_demandas += demandas[i];

    if (suma_ofertas == suma_demandas) {
        heuristicaPersonalizada(ofertas, demandas, costos, transporte, n_origenes, n_destinos, variables_basicas, asignaciones, total_asignaciones);

        cout << CYAN << "\n-----------------------------------------" << RESET << endl;
        cout << GREEN << "             Resultados" << RESET << endl;
        cout << CYAN << "-----------------------------------------" << RESET << endl;

        cout << "\nMatriz de transporte:" << endl;
        cout << "   ";
        for (int j = 0; j < n_destinos; ++j) {
            cout << setw(7) << YELLOW << "D" << j+1 << RESET;
        }
        cout << endl;

        for (int i = 0; i < n_origenes; ++i) {
            cout << YELLOW << "O" << i+1 << RESET << " ";
            for (int j = 0; j < n_destinos; ++j) {
                cout << setw(8) << BLUE << transporte[i][j] << RESET;
            }
            cout << endl;
        }
        cout << CYAN << "\n-----------------------------------------" << RESET << endl;
        cout << MAGENTA << "        Asignaciones" << RESET << endl;
        cout << CYAN << "-----------------------------------------" << RESET << endl;


        for (int i = 0; i < total_asignaciones; ++i) {
            cout << YELLOW <<"["<<i+1<<"]  "<< "Fila " << asignaciones[i].fila + 1 << ", Columna " << asignaciones[i].columna + 1 << ", Valor " << asignaciones[i].valor << RESET << endl;
        }

        cout << CYAN << "\n-----------------------------------------" << RESET << endl;
        cout << MAGENTA << "        Cálculo del Costo Total" << RESET << endl;
        cout << CYAN << "-----------------------------------------" << RESET << endl;

        int costo_total = 0;
        for (int i = 0; i < n_origenes; ++i) {
            for (int j = 0; j < n_destinos; ++j) {
                if (transporte[i][j] > 0) {
                    int costo = transporte[i][j] * costos_originales[i][j];
                    cout << MAGENTA << "x" << i + 1 << j + 1 << ": " << transporte[i][j]
                         << " unidades * costo " << costos_originales[i][j]
                         << " = " << costo << RESET << endl;
                    costo_total += costo;
                }
            }
        }

        cout << CYAN << "\n-----------------------------------------" << RESET << endl;
        cout << GREEN << "Costo total: " << costo_total << RESET << endl;
        cout << CYAN << "-----------------------------------------" << RESET << endl;
    } else {
        cout << CYAN << "\n-----------------------------------------" << RESET << endl;
        cout << RED << "Error: Las ofertas y demandas no están balanceadas." << RESET << endl;
        cout << CYAN << "-----------------------------------------" << RESET << endl;
    }

    // Liberar memoria dinámica
    delete[] ofertas;
    delete[] demandas;
    for (int i = 0; i < n_origenes; ++i) {
        delete[] costos[i];
        delete[] transporte[i];
        delete[] costos_originales[i];
    }
    delete[] costos;
    delete[] transporte;
    delete[] costos_originales;

    return 0;
}
