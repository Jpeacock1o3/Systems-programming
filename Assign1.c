#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
int main(int argc, char const *argv[])
{
DIR *directory;
struct dirent *type;
int i;
if ( argc <= 2 )
{
printf("usage: ls directory_name \n");
exit(0);
}
if((directory = opendir(argv[1])) == NULL){
printf("can't open %s", argv[1]);
exit(0);
}
for (i=1; i<argc; i++){
printf("%s:\n", argv[i]);
if((directory=opendir(argv[i])) != NULL){
while ((type = readdir(directory)) != NULL){
switch(type->d_type)
{
case DT_REG:
printf (" Type: Regular file \t \t , Name: %s \n", type->d_name);
break;
case DT_DIR:
printf (" Type: Directory \t \t , Name: %s \n", type->d_name);
break;
case DT_UNKNOWN:
printf (" Type: Unknown file type \t \t , Name: %s \n", type->d_name);
break;
case DT_BLK:
printf (" Type: Block Device \t \t , Name: %s \n", type->d_name);
break;
case DT_LNK:
printf (" Type: Symbolic Link \t \t , Name: %s \n", type->d_name);
break;
case DT_CHR:
printf (" Type: Character Device \t \t , Name: %s \n", type->d_name);
break;
}
}
printf("\n");
}
else{
perror("No such file or directory\n");
}}
closedir(directory);
exit(0);}
