// Assembler for slideshow .xml files for the MATE desktop enviroment

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <linux/limits.h> // used for PATH_MAX
#include <errno.h>
#include <sys/stat.h> // used for mkdir(), line 87
void boilerplate();
int checkPrivlages();
int copyPictures(int argc, char **argv);
bool checkArgs(int argc, char **argv);
void compileXML(int argc, char **list);

int main(int argc, char **argv){
  if (checkArgs(argc, argv) == false){
    printf("Invalid argument, see help or type -h as an argument\n");
    exit(0);
      }
  // log current directory
  char currentPath[PATH_MAX];
  getcwd(currentPath, PATH_MAX);
  checkPrivlages();
  chdir(currentPath); // return to original directory
  
  //Processing functions
  // boilerplate();
  if (argv[1][1] == 'd'){
    char userDir[PATH_MAX];
    int dirLength;
    printf("Input directory (from your root directory, not /home) ");
    fgets(userDir, PATH_MAX, stdin);
    dirLength = strlen(userDir);
    dirLength--;
    userDir[dirLength] = '\0';//get rid of newline
    chdir(userDir);
    perror("aa");
    copyPictures(argc, argv);
  }
  if (argv[1][1] == 'n'){
    copyPictures(argc, argv);
  }
  if (argv[1][1] == 'h'){
    system("cat readme.md"); // temporary(?)
    exit(0);
  }  
    return 0;
}

//-------Functions
bool checkArgs (int argc, char **argv){
  switch(argv[1][1]){
  case 100: //-d, or directory input mode
  case 104: //-h, or help (opens readme.md)
  case 110: //-n, or normal mode (works from the directory where the program is located)
  //case 114: //-r, or recursive (TODO)
    return(true);
      }
  return(0);
}
void boilerplate(){
    FILE* outputXML;
    outputXML = fopen("output.xml","a+");
    fprintf(outputXML, "<background>\n  <starttime>\n    <year>2009</year>\n    <month>08</month>\n    <day>04</day>\n    <hour>00</hour>\n    <minute>00</minute>\n    <second>00</second>\n  </starttime>\n<!--This animation will start at midnight. -->\n");
    fclose(outputXML);
  }

//checks privlages as well ass creating the slideshow folder
int checkPrivlages(){ 
  // check for admin privlages
  char *directory = "/usr/share/backgrounds/";
  int checker;
  checker = chdir(directory);
  if (checker == -1){
    //printf("Program does not have administrator privlages, try running with 'sudo (name of program)'\n");
    perror("chdir");
    exit(1);
    }
  FILE * admincheck;
  admincheck = fopen ("testForAdministrator","a+");
  if (admincheck == NULL){
    perror("Could not create slideshow");
    exit(1);
  }
  fclose(admincheck);
  remove("testForAdministrator");
  mkdir("NewSlideshow",0777); // THIS CREATES THE INITAL DIRECTORY FOR THE SLIDESHOW, IT GETS RENAMED IN THE copyPictures() FUNCTION.
  return(0);
}

int copyPictures (int argc, char **argv){
  int buffer;
  int fileCount = argc - 2;
  char* fileList [fileCount]; // array of pointers to strings
  char title[56]; //account for file path, actual file name will not exeed 10 charicters (999 pictures max)
  char currentPath2[PATH_MAX];
  
  getcwd(currentPath2, PATH_MAX);
  printf("Path: %s\n", currentPath2);
  for (int pictureCount = 2; pictureCount < argc; pictureCount++){ // starts at 2 because the 0th position is the binary name and the first are args
    int p2; // to store pictureCount's argv value
    FILE* pictureFile;
    pictureFile = fopen (argv[pictureCount], "r");
    printf("%s file\n", argv[pictureCount]);
    if (pictureFile == NULL){
      printf("File %s not found, please try again\n",argv[pictureCount]);
      fclose(pictureFile);
      exit(1);
  }
    //--Copy filenames to array
    //printf("pictures: %d\n", pictureCount); //debug
    sprintf(title, "/usr/share/backgrounds/NewSlideshow/%s", argv[pictureCount]); //pathname is required to write to that directory
    pictureCount = pictureCount - 2; // set pictureCount to proper place in the array
    p2 = pictureCount + 2;
    fileList[pictureCount] = argv[p2]; // REMINDER: image filenames are stored at and after argv[2],
    pictureCount = pictureCount + 2; // reset to initial value
    //printf("files in array: %s %s %s\n", fileList[0], fileList[1], fileList[2]); // debug
    FILE* pictureCopy;
    pictureCopy = fopen(title,"w");
    perror("fopen");
    while ((buffer = fgetc(pictureFile)) != EOF){
      fputc(buffer, pictureCopy);
    }
    fclose(pictureCopy);
    fclose(pictureFile);
  }
  
  printf("Files copied\n");
  compileXML(argc, fileList);
  return(0);
}

void compileXML(int argc, char **list){ // find another way to measure the length of the list
  //DEBUG
  //printf("Items %s %s %s\n",list[0], list[1], list[2]);
  
  //varibles
  int titleLength;
  int fileNum = argc-2;
  unsigned int transition; // how long it takes to transition from one image to another
  unsigned int stay; // how long the image stays up
  char slideshowTitle[PATH_MAX];
  
  //Naming the slideshow
  chdir("/usr/share/backgrounds/");
  printf("Input name of the slideshow\n");
  fgets(slideshowTitle, PATH_MAX, stdin);
  titleLength = strlen(slideshowTitle);
  titleLength--;
  slideshowTitle[titleLength]= '\0';// Removes newline charicter
  rename("NewSlideshow",slideshowTitle);
  char slideshowTitleNoXML[titleLength];
  strcpy(slideshowTitleNoXML,slideshowTitle);
  chdir(slideshowTitle);
  
  //.xml creation and data compilation 
  strcat(slideshowTitle,".xml");
  FILE* outputXML;
  outputXML = fopen(slideshowTitle,"w");
  
  fprintf(outputXML, "<background>\n  <starttime>\n    <year>2009</year>\n    <month>08</month>\n    <day>04</day>\n    <hour>00</hour>\n    <minute>00</minute>\n    <second>00</second>\n  </starttime>\n<!--This animation will start at midnight. -->\n");
  printf("How long should each image stay up, in minutes? ");
  scanf(" %d", &stay);
  printf("How long should the transition period be, in seconds? ");
  scanf(" %d", &transition); //whitespace to remove any newlines
  stay = stay*60; // the <duration> tag requires the value to be in seconds
   for (int fileNumber = 0; fileNumber < (fileNum); fileNumber++){
    int fileNumberNext = fileNumber++; // for the <to> tag, OR I could start and end the loop at the <to> tag but honestly that would be too disorganized even if it did save some memory
    fprintf(outputXML,"<static>\n  <duration>%d.0</duration>\n  <file>/usr/share/backgrounds/%s/%s</file>\n </static>\n",stay, slideshowTitleNoXML,list[fileNumber]);
    fprintf(outputXML,"<transition>\n  <duration>%d.0</duration>\n  <from>/usr/share/backgrounds/%s/%s</from>\n  <to>/usr/share/backgrounds/%s/%s</to>\n </transition>",transition, slideshowTitle,list[fileNumber], slideshowTitleNoXML,list[fileNumberNext]);
  }
   fprintf(outputXML,"</background>");
   fclose(outputXML);
}
