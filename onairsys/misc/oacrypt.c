#include <unistd.h>
#include <sys/time.h>

int main (int argc, char **argv) {

  char *pass, *cpass;

  char *saltarr = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
  char salt[2];

  struct timeval tnow;

  if(! (pass = argv[1])) exit (1);

  gettimeofday(&tnow, NULL);
  srandom(tnow.tv_usec);

  salt[0] = saltarr[(int)(random() % 63)];
  salt[1] = saltarr[(int)(random() % 63)];

  cpass = (char *)crypt(pass, salt);

  printf( "%s\n", cpass );
}
