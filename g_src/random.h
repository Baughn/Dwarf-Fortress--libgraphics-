#define MT_BUFFER_NUM 10
#define MT_LEN 624

void mt_init();
unsigned long mt_trandom();
long trandom(unsigned long max=2147483647LU);
long loadtrandom(unsigned long max=2147483647LU);
void push_trandom_uniform_seed(unsigned long newseed);
void pop_trandom_uniform_seed();
void trandom_twist();

void r_num();
long basic_random(long max=2147483647);