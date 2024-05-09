#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]) {
if (argc != 4) {
fprintf(stderr, "Usage: %s infilename string1 string2\n", argv[0]);
return 1;
}
char *infilename = argv[1];
char *string1 = argv[2];
char *string2 = argv[3];
FILE *infile = fopen(infilename, "r");
if (infile == NULL) {
perror("Error opening input file");
return 1;
}
char buffer[1024];
int substitutions = 0;
while (fgets(buffer, 1024, infile) != NULL) {
char *sub = strstr(buffer, string1);
if (sub != NULL) {
// Only substitute once
strncpy(sub, string2, strlen(string2));
substitutions++;
}
printf("%s", buffer);
}
fclose(infile);
fprintf(stderr, "%d substitutions made\n", substitutions);
return 0;
}
