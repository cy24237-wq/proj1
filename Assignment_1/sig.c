#include <stdio.h>
#include <signal.h> 
#include <sys/types.h> 
#include <unistd.h>
int main(){
kill(8138,SIGUSR1);
return 0;
}
