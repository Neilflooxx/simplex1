#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

vector<double> ingresarFuncionObjetivo(int num_vars) {
    vector<double> coeficientes(num_vars);
    cout << "\nIngrese los coeficientes de la funcion objetivo (" << num_vars << " variables):" << endl;
    for (int i = 0; i < num_vars; ++i) {
        cout << "Coeficiente de x" << i + 1 << ": ";
        cin >> coeficientes[i];
    }
    return coeficientes;
}

void ingresarRestricciones(int num_vars, vector<vector<double>>& A, vector<double>& b) {
    while (true) {
        char respuesta;
        cout << "\n¿Agregar una nueva restriccion? (s/n): ";
        cin >> respuesta;
        if (respuesta != 's') break;

        vector<double> restriccion(num_vars);
        cout << "Ingrese los coeficientes de la restriccion:" << endl;
        for (int i = 0; i < num_vars; ++i) {
            cout << "Coeficiente de x" << i + 1 << ": ";
            cin >> restriccion[i];
        }
        A.push_back(restriccion);

        double rhs;
        cout << "Lado derecho (RHS): ";
        cin >> rhs;
        b.push_back(rhs);
    }
}

void imprimirTabla(const vector<vector<double>>& tabla) {
    for (const auto& fila : tabla) {
        for (double val : fila) {
            cout << setw(8) << fixed << setprecision(2) << val << " ";
        }
        cout << endl;
    }
}

pair<vector<double>, double> metodoSimplex(vector<double> c, vector<vector<double>> A, vector<double> b, string tipo) {
    int num_vars = c.size();
    int num_rest = b.size();
    int total_cols = num_vars + num_rest + 1;

    vector<vector<double>> tabla(num_rest + 1, vector<double>(total_cols, 0.0));

    for (int i = 0; i < num_rest; ++i) {
        for (int j = 0; j < num_vars; ++j)
            tabla[i][j] = A[i][j];
        tabla[i][num_vars + i] = 1;
        tabla[i].back() = b[i];
    }

    for (int j = 0; j < num_vars; ++j)
        tabla.back()[j] = (tipo == "max" ? c[j] : -c[j]);

    int iteracion = 0;
    while (true) {
        cout << "\n------------------------------ Iteracion " << ++iteracion << " ------------------------------" << endl;
        imprimirTabla(tabla);

        int col_piv = min_element(tabla.back().begin(), tabla.back().end() - 1) - tabla.back().begin();
        if (tabla.back()[col_piv] >= 0) break;

        double min_ratio = numeric_limits<double>::infinity();
        int fila_piv = -1;
        for (int i = 0; i < num_rest; ++i) {
            if (tabla[i][col_piv] > 0) {
                double ratio = tabla[i].back() / tabla[i][col_piv];
                if (ratio < min_ratio) {
                    min_ratio = ratio;
                    fila_piv = i;
                }
            }
        }

        double pivote = tabla[fila_piv][col_piv];
        for (double& val : tabla[fila_piv])
            val /= pivote;

        for (int i = 0; i < tabla.size(); ++i) {
            if (i == fila_piv) continue;
            double factor = tabla[i][col_piv];
            for (int j = 0; j < total_cols; ++j)
                tabla[i][j] -= factor * tabla[fila_piv][j];
        }
    }

    vector<double> solucion(num_vars, 0.0);
    for (int i = 0; i < num_rest; ++i) {
        for (int j = 0; j < num_vars; ++j) {
            if (tabla[i][j] == 1) {
                bool es_base = true;
                for (int k = 0; k < num_rest; ++k) {
                    if (k != i && tabla[k][j] != 0) {
                        es_base = false;
                        break;
                    }
                }
                if (es_base) {
                    solucion[j] = tabla[i].back();
                    break;
                }
            }
        }
    }

    double z_opt = tabla.back().back();
    if (tipo == "min") z_opt *= -1;

    return {solucion, z_opt};
}

int main() {
    while (true) {
        limpiarPantalla();
        cout << "=== METODO SIMPLEX INTERACTIVO ===" << endl;

        int num_vars;
        cout << " cuantas variables tiene la funcion objetivo? ";
        cin >> num_vars;

        vector<double> c = ingresarFuncionObjetivo(num_vars);
        vector<vector<double>> A;
        vector<double> b;
        ingresarRestricciones(num_vars, A, b);

        if (A.empty()) {
            cout << "Debe ingresar al menos una restriccion." << endl;
            continue;
        }

        int eleccion;
        string tipo;
        do {
            cout << "\n¿Desea maximizar o minimizar la funcionn objetivo?" << endl;
            cout << "1. Maximizar" << endl;
            cout << "2. Minimizar" << endl;
            cout << "Seleccione una opciÃ³n (1/2): ";
            cin >> eleccion;
        } while (eleccion != 1 && eleccion != 2);
        tipo = (eleccion == 1 ? "max" : "min");

        cout << "\nResolviendo el problema..." << endl;
        auto [solucion, z] = metodoSimplex(c, A, b, tipo);

        cout << "\n SOLUCION OPTIMO \n" << endl;
        for (int i = 0; i < solucion.size(); ++i)
            cout << "x" << i + 1 << " = " << fixed << setprecision(4) << solucion[i] << endl;

        cout << "\n VALOR OPTIMO DE Z = " << fixed << setprecision(4) << z << " " << endl;

        cout << "\nPresiona Enter para finalizar";
        cin.ignore();
        cin.get();
    }
}