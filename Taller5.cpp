#include <limits.h> // Para INT_MAX
#include <omp.h>    // Para OpenMP
#include <stdio.h>  // Para printf
#include <stdlib.h> // Para rand, srand
#include <time.h>   // Para time
#include <fstream>  // Para archivos CSV
#include <string>   // Para std::string
#include <algorithm>// Para std::replace

#define MAX_N 13

// ============================================================
// Función de backtracking para encontrar el camino más corto
// ============================================================
void backtracking(int matrix[MAX_N][MAX_N], int n, int current, int end,
                  int dist, int *minDist, int visited[]) {
  if (current == end) {
    // Si hemos llegado al nodo destino, actualizamos la
    // distancia mínima si es menor que la actual.
    if (dist < *minDist) {
      *minDist = dist;
    }
    return;
  }
  for (int i = 0; i < n; i++) {
    // Si hay arista hacia i y no está visitado
    if (matrix[current][i] != 0 && visited[i] == 0) {
      visited[i] = 1;
      backtracking(matrix, n, i, end, dist + matrix[current][i], minDist,
                   visited);
      visited[i] = 0; // backtrack
    }
  }
}

int main(void) {
  int matrix[MAX_N][MAX_N];
  int visited[MAX_N];
  int i, j;

  // Inicializa semilla para generar números aleatorios.
  srand((unsigned)time(NULL));

  printf("=============================================\n");
  printf("   TALLER 5 - Backtracking Paralelo\n");
  printf("   Pruebas progresivas de 2x2 a 13x13\n");
  printf("=============================================\n\n");

  printf("%-10s %-10s %-15s %-15s\n", "Matriz", "Nucleos", "Tiempo(ms)",
         "Dist.Min");
  printf("----------------------------------------------------\n");

  // Crear y abrir el archivo CSV (Excel)
  std::ofstream archivo_csv("resultados_taller5.csv");
  archivo_csv << "Matriz;Nucleos;Tiempo(ms)\n";

  // Pruebas progresivas desde 2x2 hasta 13x13
  for (int n = 2; n <= MAX_N; n++) {
    int start = 0;
    int end = n - 1;

    // Genera la matriz con valores aleatorios entre 1 y 10
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
        // rand() % 10 devuelve un número entre 0 y 9; le sumamos
        // 1 para que sea 1..10
        matrix[i][j] = rand() % 10 + 1;
      }
    }
    // ========================================================
    // Ejecución SECUENCIAL (1 núcleo)
    // ========================================================
    char label[10];
    sprintf(label, "%dx%d", n, n);

    int minDist = INT_MAX;
    for (i = 0; i < n; i++) {
      visited[i] = 0;
    }
    visited[start] = 1; // Marcamos el nodo inicial como visitado

    double inicio = omp_get_wtime();

    // Llamamos a la función de backtracking
    backtracking(matrix, n, start, end, 0, &minDist, visited);

    double tiempo_sec = (omp_get_wtime() - inicio) * 1000.0;

    printf("%-10s %-10d %-15.4f %-15d\n", label, 1, tiempo_sec, minDist);
    
    // Convertir tiempo a string y cambiar punto por coma para LibreOffice/Excel en español
    std::string tiempo_sec_str = std::to_string(tiempo_sec);
    std::replace(tiempo_sec_str.begin(), tiempo_sec_str.end(), '.', ',');

    // Guardar resultado secuencial en el CSV usando punto y coma como separador
    archivo_csv << label << ";1;" << tiempo_sec_str << "\n";

    // ========================================================
    // Ejecución PARALELA con 2 y 3
    // ========================================================
    int nucleos[] = {2, 3};
    int num_pruebas = 2;

    for (int p = 0; p < num_pruebas; p++) {
      int hilos = nucleos[p];
      int minDistParalelo = INT_MAX;

      // Obtener vecinos iniciales del nodo start
      int vecinos[MAX_N];
      int numVecinos = 0;
      for (i = 0; i < n; i++) {
        if (i != start && matrix[start][i] != 0) {
          vecinos[numVecinos++] = i;
        }
      }

      inicio = omp_get_wtime();

#pragma omp parallel num_threads(hilos)
      {
        int minDistLocal = INT_MAX;

#pragma omp for schedule(dynamic)
        for (int idx = 0; idx < numVecinos; idx++) {
          int vecino = vecinos[idx];
          int visitedLocal[MAX_N];

          // Inicializar visitados locales
          for (int k = 0; k < n; k++) {
            visitedLocal[k] = 0;
          }
          visitedLocal[start] = 1;
          visitedLocal[vecino] = 1;

          int distInicial = matrix[start][vecino];
          backtracking(matrix, n, vecino, end, distInicial, &minDistLocal,
                       visitedLocal);
        }

#pragma omp critical
        {
          if (minDistLocal < minDistParalelo) {
            minDistParalelo = minDistLocal;
          }
        }
      }

      double tiempo_par = (omp_get_wtime() - inicio) * 1000.0;

      printf("%-10s %-10d %-15.4f %-15d\n", label, hilos, tiempo_par,
             minDistParalelo);
             
      // Convertir tiempo a string y cambiar punto por coma
      std::string tiempo_par_str = std::to_string(tiempo_par);
      std::replace(tiempo_par_str.begin(), tiempo_par_str.end(), '.', ',');

      // Guardar resultado paralelo en el CSV usando punto y coma
      archivo_csv << label << ";" << hilos << ";" << tiempo_par_str << "\n";
    }
  }

  // Cerrar el archivo CSV
  archivo_csv.close();

  printf("\n=============================================\n");
  printf("   Analisis completado.\n");
  printf("   Los resultados han sido guardados en 'resultados_taller5.csv'\n");
  printf("=============================================\n");

  return 0;
}
