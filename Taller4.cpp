#include <stdio.h>
#include <omp.h>
#include <time.h>
void ValorPi(){
    clock_t inicio, fin;
    double pi = 0, time_ms;
    long n;
    int signo = 1;
    inicio = clock();
    printf("Ingrese el valor de n: ");
    scanf("%ld", &n);
    for (long i = 0; i <= n; i++){
        pi = pi + (signo*(4.0/(i*2.0+1)));
        signo = signo*-1;
    }
    fin = clock();
    time_ms = ((double)(fin-inicio)/CLOCKS_PER_SEC)*1000;
    printf("Tiempo %.9f ms\n", time_ms);
    printf("El valor de PI es: %.10f", pi);
}

main(){
    ValorPi();
}

