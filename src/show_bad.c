#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
 

 
int main(int argc, char **argv)
{
    FILE *fcraw;
    unsigned char rawrow[10000];
    int rowpix, rows, rowlen;
    int nrow,i,src, value;
    struct stat info;
    int threshold=30;	
    char *err;
    char bits=10;
    int black_level=32;
 
        if ((argc!=2) && (argc!=3)) { printf("usage: show_bad.exe raw_file <threshold>\n"); return 1;}

        if (stat(argv[1],&info)!=0) { printf("file not exist\n"); return 2;}

        switch (info.st_size){
         case (2672*1968*10)/8: rowpix=2672; rows=1968; break; // 5 MP
         case (2664*1968*10)/8: rowpix=2664; rows=1968; break; // another 5 MP
         case (2888*2136*10)/8: rowpix=2888; rows=2136; break; // 6 MP
         case (3152*2340*10)/8: rowpix=3152; rows=2340; break; // 7 MP
         case (3344*2484*10)/8: rowpix=3344; rows=2484; break; // 8 MP
         case (3336*2480*10)/8: rowpix=3336; rows=2480; break; // another 8 MP
         case (3736*2772*10)/8: rowpix=3736; rows=2772; break; // 10 MP
         case (4104*3048*10)/8: rowpix=4104; rows=3048; break; // 12 MP
         case (3720*2772*12)/8: rowpix=3720; rows=2772; bits=12; black_level<<=2; threshold<<=2; break; // another 10 MP with 12 bit RAW

         default: printf("file is unknown\n"); return 3;
        }

        rowlen=(rowpix*bits)/8;

        fcraw=fopen(argv[1],"r+b");

        if (!fcraw) { printf("cannot open file\n"); return 4;}

        if (argc==3){
         int thr=strtol(argv[2],&err,0);
         if ((*err) || (thr>((1<<bits)-1-black_level)) || (thr<=0)) {
          printf("invalid threshold, using default value (%d)\n", threshold);
         }
        else threshold=thr;
        } 

 
        for (nrow=0; nrow<rows; nrow++) {
            fread(rawrow, 1, rowlen, fcraw);
        
            for (i=0; i<rowpix; i++) {
             if (bits==10) {
              src=(i/8)*10;
              switch (i%8){
                case 0: value=((0x3fc&(((unsigned short)rawrow[src+1])<<2)) | (rawrow[src+0] >> 6)); break;
                case 1: value=((0x3f0&(((unsigned short)rawrow[src+0])<<4)) | (rawrow[src+3] >> 4)); break;
                case 2: value=((0x3c0&(((unsigned short)rawrow[src+3])<<6)) | (rawrow[src+2] >> 2)); break;
                case 3: value=((0x300&(((unsigned short)rawrow[src+2])<<8)) | (rawrow[src+5]));      break;
                case 4: value=((0x3fc&(((unsigned short)rawrow[src+4])<<2)) | (rawrow[src+7] >> 6)); break;
                case 5: value=((0x3f0&(((unsigned short)rawrow[src+7])<<4)) | (rawrow[src+6] >> 4)); break;
                case 6: value=((0x3c0&(((unsigned short)rawrow[src+6])<<6)) | (rawrow[src+9] >> 2)); break;
                case 7: value=((0x300&(((unsigned short)rawrow[src+9])<<8)) | (rawrow[src+8]));      break;
              }
             }
             else if (bits==12) {
              src=(i/4)*6;
              switch (i%4){
                case 0: value=((0xFF0&(((unsigned short)rawrow[src+1])<<4)) | (rawrow[src+0] >> 4)); break;
                case 1: value=((0xF00&(((unsigned short)rawrow[src+0])<<8)) | (rawrow[src+3]     )); break;
                case 2: value=((0xFF0&(((unsigned short)rawrow[src+2])<<4)) | (rawrow[src+5] >> 4)); break;
                case 3: value=((0xF00&(((unsigned short)rawrow[src+5])<<8)) | (rawrow[src+4]     )); break;
              }
             }
            if (value>threshold+black_level) printf("%d,%d=%d\n",i,nrow, value);
            }
         }
     fclose(fcraw);
     return 0;
    }
