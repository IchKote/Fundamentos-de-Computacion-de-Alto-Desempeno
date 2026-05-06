#include <stdio.h>
#include <omp.h>

void ValorPi(int n) {
    double pi = 0;
    int signo = 1;
    double inicio = omp_get_wtime();
    for (long i = 0; i <= n; i++) {
        pi = pi + (signo * (4.0 / (i * 2.0 + 1)));
        signo = signo * -1;
    }
    double time_ms = (omp_get_wtime() - inicio) * 1000;
    printf("---SECUENCIAL---\n");
    printf("Tiempo %.9f ms\n", time_ms);
    printf("El valor de PI es: %.9f\n\n", pi);
}
 
void ValorPiParalelo(int n) {
    double pi = 0;
    double inicio = omp_get_wtime();
    #pragma omp parallel for reduction(+:pi)
    for (long i = 0; i <= n; i++) {
        double signo = (i % 2 == 0) ? 1.0 : -1.0;
        pi = pi + (signo * (4.0 / (i * 2.0 + 1)));
    }
    double time_ms = (omp_get_wtime() - inicio) * 1000;
    printf("---PARALELO---\n");
    printf("Tiempo %.9f ms\n", time_ms);
    printf("El valor de PI es: %.9f\n\n", pi);
}

int main(){
    int muestra[] = {1000, 10000, 3};
    for (int n : muestra){
        printf("Muestra n = %d\n", n);
        ValorPi(n);
        ValorPiParalelo(n);
    }
    return 0;
}