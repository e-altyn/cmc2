#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

double eps1 = 0.00001;
double eps2 = 0.0001;

// функции из func.asm
extern double _f1(double);
extern double _f2(double);
extern double _f3(double);
extern double _df1(double);
extern double _df2(double);
extern double _df3(double);

extern double _f4(double);
extern double _f5(double);
extern double _f6(double);
extern double _df4(double);
extern double _df5(double);
extern double _df6(double);

// модуль числа типа double
double d_abs(double x){
    return (x > 0) ? x : -x;
}

// знак числа типа double
double d_sgn(double x){
    return (x > 0) ? 1. : -1.;
}

//вычисляем корень функции F=f-g комбинированным методом
double root(double(*f)(double), double(*g)(double), double(*df)(double), double(*dg)(double), double a, double b, double eps, int *steps){
    *steps = 0;
    double F_a, F_b;
    int d2F = (f(a)-g(a) + f(b)-g(b))/2. >= f((a+b)/2.) - g((a+b)/2.) ? -1 : 1; // знак 2й производной
    int dF = (df((a+b)/2.) - dg((a+b)/2.) >= 0) ? 1 : -1; // знак 1й производной

    // в зависимости от знака (F' * F") итерируем с одной стороны методом касательных, с другой - методом хорд
    if (dF * d2F > 0){
        while(d_abs(a - b) > eps){
            F_a = f(a) - g(a);
            F_b = f(b) - g(b);
            a -= (b-a) / (F_b - F_a) * F_a;
            b -= F_b / (df(b)-dg(b)); // dF(b)
            *steps += 1;
        }
    } else {
        while(d_abs(a - b) > eps){
            F_a = f(a) - g(a);
            F_b = f(b) - g(b);
            b -= (b-a) / (F_b - F_a) * F_b;
            a -= F_a / (df(a)-dg(a));
            *steps += 1;
        }
    }

    return (a + b)/2.;
}

// считаем интеграл функции f на [a, b]
double integral(double(*f)(double), double a, double b, double eps){
    // в зависимости от порядка a и b меняем знак
    double sign = 1.;
    if (a > b) {
        double tmp = a;
        a = b;
        b = tmp;
        sign = -1.;
    }

    // приближаем площадь набором трапеций на размечении [a,b]
    // каждый раз все отрезки размечения делятся пополам
    double new_area = d_abs(f(a) - f(b)) * (b - a) /2.;
    double area, dist;
    int steps = 0;
    while (d_abs(new_area - area) > eps){
        steps++;
        area = new_area;
        new_area = 0;
        dist = (b - a) / (double)(pow(2, steps));

        for (int i = 0; i < pow(2, steps); i++)
            new_area +=  (f(a + dist*(double)i) + f(a + dist*(double)(i+1))) * dist/2.;
    }
    return sign * new_area;
}

// считаем площадь фигру между графиками
// суммируем, т.к. области интегрирования расположены "по кругу" (1-2, 2-3, 3-1)
// из-за этого у суммы может быть любой знак (в зависимости от функций)
// соответственно, в конце берем еще модуль
double compute_area(void){
    double x12, x23, x31, a1, a2, a3;
    int steps = 0;
    x12 = root(_f1, _f2, _df1, _df2, 1., 2., eps1, &steps);
    x23 = root(_f2, _f3, _df2, _df3, 0., 1., eps1, &steps);
    x31 = root(_f1, _f3, _df1, _df3, -3., -2., eps1, &steps);
    a1 = integral(_f1, x31, x12, eps2);
    a2 = integral(_f2, x12, x23, eps2);
    a3 = integral(_f3, x23, x31, eps2);
    return d_abs(a1 + a2 + a3);
}

// ДАЛЬШЕ -- ФУНКЦИИ ПЕЧАТИ

// печатаем количество шагов в комбинированном методе для всех трех точек пересечения
void print_iterations(void){
    double x12, x23, x31, a1, a2, a3;
    int steps1, steps2, steps3;
    x12 = root(_f1, _f2, _df1, _df2, 1., 2., eps1, &steps1);
    x23 = root(_f2, _f3, _df2, _df3, 0., 1., eps1, &steps2);
    x31 = root(_f1, _f3, _df1, _df3, -3., -2., eps1, &steps3);
    printf("solving f1 = f2 took %d iterations \nsolving f2 = f3 took %d iterations \nsolving f3 = f1 took %d iterations\n", steps1, steps2, steps3);
}

// печатаем абсциссы точек пересечения всех пар функций
void print_roots(void){
    double x12, x23, x31, a1, a2, a3;
    int steps = 0;
    x12 = root(_f1, _f2, _df1, _df2, 1., 2., eps1, &steps);
    x23 = root(_f2, _f3, _df2, _df3, 0., 1., eps1, &steps);
    x31 = root(_f1, _f3, _df1, _df3, -3., -2., eps1, &steps);
    printf("f1 = f2 at x = %lf\nf2 = f3 at x = %lf\nf1 = f3 at x = %lf\n", x12, x23, x31);
}

// печатаем площадь
void print_area(void) {
    printf("total area = %f\n", compute_area());
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("\t'%s -help' to request help message\n\n", argv[0]);
        return 0;
     }
    int i = 1;
    while (i < argc){
        if (!strcmp(argv[i], "-help")) {
            printf("\t%s -help              get help\n", argv[0]);
            printf("\t%s -roots             get intersection points\n", argv[0]);
            printf("\t%s -iter              get number of iterations\n", argv[0]);
            printf("\t%s -area              get area of the figure\n", argv[0]);
            printf("\t%s -testr             test \"root\" function\n", argv[0]);
            printf("\t                         4 parameters needed: i,j,a,b\n");
            printf("\t                         i,j - indices of funcions (1-3)\n");
            printf("\t                         test functons: 1)1+x 2)1-x 3)0\n");
            printf("\t                         [a,b] - interval of search\n");
            printf("\t%s -testi             test \"integral\" function\n", argv[0]);
            printf("\t                         3 parameters needed: i,a,b\n");
            printf("\t                         i - index of funcion (1-3)\n");
            printf("\t                         test functons: 1)1+x 2)1-x 3)0\n");
            printf("\t                         [a,b] - interval of integral\n");
            i++;
        } else if (!strcmp(argv[i], "-roots")) {
            print_roots();
            i++;
        } else if (!strcmp(argv[i], "-area")) {
            print_area();
            i++;
        } else if (!strcmp(argv[i], "-iter")) {
            print_iterations();
            i++;
        } else if (!strcmp(argv[i], "-testr")) {
            if (argc - i < 5) {
                printf("'-testr' operation requires 4 parameters\n\n");
                break;
            } else {
                int q1, q2, steps = 0;
                double a, b;
                sscanf(argv[i+1], "%d", &q1);
                sscanf(argv[i+2], "%d", &q2);
                sscanf(argv[i+3], "%lf", &a);
                sscanf(argv[i+4], "%lf", &b);
                if (((q1 == 1) && (q2 == 2)) || ((q1 == 2) && (q2 == 1) )) {
                    printf("~ 1+x = 1-x at x = %lf\n", root(_f4, _f5, _df4, _df5, a, b, eps1, &steps));
                    printf("~ took %d iterations\n", steps);
                } else if (((q1 == 1) && (q2 == 3)) || ((q1 == 3) && (q2 == 1) )) {
                    printf("~ 1+x = 0 at x = %lf\n", root(_f4, _f6, _df4, _df6, a, b, eps1, &steps));
                    printf("~ took %d iterations\n", steps);
                } else if (((q1 == 3) && (q2 == 2)) || ((q1 == 2) && (q2 == 3) )) {
                    printf("~ 1-x = 0 at x = %lf\n", root(_f5, _f6, _df5, _df6, a, b, eps1, &steps));
                    printf("~ took %d iterations\n", steps);
                } else {
                    printf("wrong parameters, try again\n");
                    break;
                }
                i += 5;
            }
        } else if(!strcmp(argv[1], "-testi")) {
            if (argc - i < 4) {
                printf("'-testi' operation requires 3 parameters\n\n");
                break;
            } else {
                int q;
                double a, b;
                sscanf(argv[i+1], "%d", &q);
                sscanf(argv[i+2], "%lf", &a);
                sscanf(argv[i+3], "%lf", &b);

                if (q == 1) {
                    printf("~ integral of 1+x at [%lf,%lf]: %lf\n", a, b, integral(_f4, a, b, eps2));
                } else if (q == 2) {
                    printf("~ integral of 1-x at [%lf,%lf]: %lf\n", a, b, integral(_f5, a, b, eps2));
                } else if (q == 3) {
                    printf("~ integral of 0 at [%lf,%lf]: %lf\n", a, b, integral(_f6, a, b, eps2));
                } else {
                    printf("wrong parameters, try again\n\n");
                }
                i += 4;
            }
        } else {
            printf("\nwrong input %s\n\ntype %s -help for help.\n\n", argv[1], argv[0]);
            break;
        }
    }
    return 0;
}

