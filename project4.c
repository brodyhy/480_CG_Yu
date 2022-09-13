/*
 * author: Haoran Yu
 *
 * This program read info about an existing TLB, Virtual Page Table, and Physical Memory Direct Memory caching. Based on this info, the program will read a  virtual address from user and output the byte at that location, or "Can not be determined" if the data is not available.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * This method translates the user input virtual address (hex) to binary virtual address. Based on size of virtual address (XXXX, XXX, XX or X), it adds different amount of 0 ahead to unify the format of output binary virtual address.
 * parameter: hex[] char array that stores user input virtual address;
              bin[] empty char array that stores binary virtual address after translation;
              n size_t that stores the size of virtual address to determine number of zero to be added.
 */
void hex2bin(char hex[],char bin[], size_t n){
    if (n<2){
        strcat(bin, "000000000000");
    }else if (n<3){
        strcat(bin, "00000000");
    }else if (n<4){
        strcat(bin, "0000");
    }

    int i = 0;
    while(hex[i]){
        switch(hex[i]){
            case '0': strcat(bin, "0000");break;
            case '1': strcat(bin, "0001");break;
            case '2': strcat(bin, "0010");break;
            case '3': strcat(bin, "0011");break;
            case '4': strcat(bin, "0100");break;
            case '5': strcat(bin, "0101");break;
            case '6': strcat(bin, "0110");break;
            case '7': strcat(bin, "0111");break;
            case '8': strcat(bin, "1000");break;
            case '9': strcat(bin, "1001");break;
            case 'A': strcat(bin, "1010");break;
            case 'B': strcat(bin, "1011");break;
            case 'C': strcat(bin, "1100");break;
            case 'D': strcat(bin, "1101");break;
            case 'E': strcat(bin, "1110");break;
            case 'F': strcat(bin, "1111");break;
        }
        i++;
        
    }

 
}

/*
 * Main method that stores info about TLB, Page Table and Cache first in integer arrays.
 * Then read virtual address from user and call hex2bin() to convert it to binary form.
 * Then calculate TLBT and TLBI to get PPN, if failed, use VPN to get PPN.
 * If no valid PPN is retrieved, print msg and terminate.
 * Else, print the byte found.
 */
int main(void){
    char * line = NULL;
    FILE * fp;
    size_t len=0;
    ssize_t read;
    int TLB[10][20]={0};
    int Page[100]={0};
    int Cache[20][10]={0};
    int i;
    int TLBindex=0;
    int TLBcount=0;
    int Pagevpn=0;
    int Cachenum=0;
    /*
     * read info about TLB, Page Table, and Cache, store in arrays.
     */
    fp = fopen("Project4Input.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while((read = getline(&line, &len, fp))!=-1){
        char * splitted = strtok(line, ",");
        //printf("%s",splitted);
        if (strcmp(splitted,"TLB")==0) {
            splitted = strtok(NULL, ",");
            //printf("%s\n", splitted);
            TLBindex = atoi(splitted);
            //printf("%d\n",TLBindex);
            splitted = strtok(NULL, ",");
            TLB[TLBindex][TLBcount] = (int)strtol(splitted, NULL, 16);
            TLBcount++;
            splitted = strtok(NULL, ",");
            TLB[TLBindex][TLBcount] = (int)strtol(splitted, NULL, 16);
            TLBcount++;
        }else if(strcmp(splitted,"Page")==0){
            splitted = strtok(NULL, ",");
            Pagevpn = (int)strtol(splitted, NULL, 16);
            //printf("%d\n", Pagevpn);
            splitted = strtok(NULL, ",");
            Page[Pagevpn] = (int)strtol(splitted, NULL, 16);
        }else if(strcmp(splitted, "Cache")==0){
            splitted = strtok(NULL, ",");
            Cachenum = (int)strtol(splitted, NULL, 16);
            for (i=0; i<5; i++){
                splitted = strtok(NULL, ",");
                Cache[Cachenum][i] = (int) strtol(splitted, NULL, 16);
            }
        }else{
            printf("Done\n");
        }
    }
    fclose(fp);
    /*
     * read user input
     */
    char va[1];
    printf("Enter Virtual Addresses:\n");
    scanf("%s", va);
    //printf("length of va: %lu\n", strlen(va));
    /*
     * convert virtual address to binary
     */
    char va_bin[16];
    hex2bin(va, va_bin, strlen(va));
    //printf("Virtual Address in Binary: %s\n", va_bin);
    //printf("length of va_bin: %lu\n", strlen(va_bin));
    /*
     * get TLBI and TLBT, use them to get PPN
     */
    int TLBI=(va_bin[13]-48)*2+(va_bin[14]-48);
    //printf("va_bin index 0 and 1: %c %c\n", va_bin[5], va_bin[6]);
    //printf("va_bin index 8 and 9: %c %c\n", va_bin[8], va_bin[9]);
    //printf("try casting: %d %d\n", (int)va_bin[8], (int)va_bin[9]);
    //printf("TLBI: %d\n", TLBI);
    int TLBT = (va_bin[5]-48)*pow(2,7)+(va_bin[6]-48)*pow(2,6)+(va_bin[7]-48)*pow(2,5)+
    (va_bin[8]-48)*pow(2,4)+(va_bin[9]-48)*pow(2,3)+(va_bin[10]-48)*pow(2,2)+(va_bin[11]-48)*2+
    (va_bin[12]-48);
    //printf("TLBI: %d\n", TLBT);
    int useTLB=0;
    int PPN=0;
    int j=0;
    while (TLB[TLBI][j]){
        if (TLB[TLBI][j]==TLBT){
            PPN = TLB[TLBI][j+1];
            useTLB = 1;
            break;
        }
        if (j<18)
            j+=2;
        else
            break;
    }
    /*
     * If TLB cannot lead to PPN, then use VPN to find PPN.
     */
    int VPN;
    if(useTLB==0){
        VPN=TLBT*4+TLBI;
        PPN=Page[VPN];
    }
    /*
     * If both methods fail, print out msg and terminate.
     */
    if(PPN==0){
        printf("Cannot be determined.");
        return 0;
    }
    /*
     * print byte founded.
     */
    int result = 0;
    int CI = (va_bin[15]-48)*8+(va_bin[16]-48)*4+(va_bin[17]-48)*2+(va_bin[18]-48);
    int CO = (va_bin[19]-48)*2+(va_bin[20]-48);
    if (PPN==Cache[CI][0]){
        result = Cache[CI][CO+1];
        printf("%x", result);
    }else{
        printf("Cannot be determined.");
    }
    return 0;
}
