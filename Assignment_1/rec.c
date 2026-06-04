#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void sig_handler(int signum){
while(1){
printf("a\n");
sleep(1);
}
}
int main(){
signal(SIGUSR1,sig_handler);
while (1) {
printf("z\n");
sleep(1);
}
}
