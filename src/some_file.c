// global
static int current = 0;
static int compute()
{
 return current++;
}

int main()
{
 // inside main
 current = 5;
 int res = compute();
 return 0;
}