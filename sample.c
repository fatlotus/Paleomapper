#include <stdlib.h>
float x;
char* input;

int main(int argc, char* argv[]) {
       input = "32";
       printf ("input is %s\n", input);
       x = atof(input);
       printf ("x is %f\n", x);
       return 0;
}
