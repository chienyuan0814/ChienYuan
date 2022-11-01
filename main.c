#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define bufsize 120

#define DEFINED_DEBUG
#define DEBUG

double stack[bufsize] = {0.0};
int stkptr=-1;

char queue[15] ={0};
int qptr=-1;

double rvp[bufsize] = {0.0};
int helper[bufsize] = {0};
int rvpptr = 0;

char vars[20][15]= {{'0'}};
int varptr=0;
double varvalue[bufsize]= {0.0};
int varidx = -1;

#ifdef DEFINED_DEBUG
//char buf[bufsize]="a=1;b=3;a*b\0";
//char buf[bufsize]="(1+2)*3\0";
char buf[bufsize]="5^2+(1+2)*3\0";
#else
char buf[bufsize] = {0};
#endif // DEFINED_DEBUG

int bufptr = 0;
char *ptr;

int opval= 0;

void rvprest(void)
{
    stkptr=-1;
    memset(stack,0x30,bufsize);
    rvpptr = 0;
    memset(rvp,0x30,bufsize);
    memset(helper,0x30,bufsize);
    varidx=-1;
}

void queue_reset(void)
{
   memset(queue,0,30);
   qptr=-1;
}

int idxofvar(char *var)
{
    for(int i=0; i<20; i++)
       if (strcmp(vars[i],var)==0)
            return i;
    return -1;
}


//if c is +-*/^
int isop(const char c)
{
    return c=='+'|| c=='-'|| c=='*'|| c=='/'|| c=='^';
}

//+-*/^ to value
int op2val(const char c)
{
    switch(c)
    {
    case '+':
        return 1;
    case '-':
        return 2;
    case '*':
        return 3;
    case '/':
        return 4;
    case '^':
        return 5;
    case '(':
        return 6;
    case ')':
        return 7;
    default:
        return 0;
    }
}

double queue2d(void)
{
    double v;
    if(isdigit(queue[0]))
        v=strtod(queue,NULL);
    else
        v=varvalue[idxofvar(queue)];
    queue_reset();
    return v;
}

//we assumed the test case will always be correct.
int rvp_push(double v, int hId)
{
    int currptr = rvpptr++;
    rvp[currptr]=v;
    helper[currptr]=hId;
    return currptr;
}

double calc(double v1, double v2, int op)
{
    switch (op)
    {
    case 1: return v1+v2;
    case 2: return v1-v2;
    case 3: return v1*v2;
    case 4: return v1/v2;
    case 5: return pow(v1,v2);
    default: return 0;
    }
}

char * ReversePolishExpression(char *rpe, int rpe_len)
{
        char tmp[10] = {0};
        for(int i=0; i<rvpptr; i++)
        {
          if (helper[i]==1)
            snprintf(tmp,10,"%f\x20",rvp[i]);
          else
          {
              if ((int)rvp[i] ==1)
                  snprintf(tmp,10,"%c\x20",'+');
              else if ((int)rvp[i] ==2)
                  snprintf(tmp,10,"%c\x20",'-');
              else if ((int)rvp[i] ==3)
                  snprintf(tmp,10,"%c\x20",'*');
              else if ((int)rvp[i] ==4)
                  snprintf(tmp,10,"%c\x20",'/');
              else if ((int)rvp[i] ==5)
                  snprintf(tmp,10,"%c\x20",'^');
          }
          strcat(rpe,tmp);
        }
        return rpe;
}

double checkout(void)
{
    if(qptr>-1)
        rvp_push(queue2d(),1);

    while(stkptr >-1)
    {
        rvp[rvpptr]=stack[stkptr--];
        helper[rvpptr]=2;
        rvpptr++;
    }
    #ifdef DEBUG
        // printout Reverse Polish expression
        char rpe[120]={0};
        printf("Reverse Polish expression: %s\n",ReversePolishExpression(rpe,120));
    #endif // DEBUG

    for(int i=0; i<rvpptr; i++)
    {
        double v1,v2;
        if (helper[i]==1)
          stack[++stkptr]=rvp[i];
        else
        {
            v2=stack[stkptr--];
            v1=stack[stkptr--];
            stack[++stkptr]=calc(v1,v2,rvp[i]);
        }
    }
    double result = stack[stkptr--];
    //double result = stack_pop();
    if(varidx>-1)
        varvalue[varidx]=result;
    rvprest();
    return result;
}

int main()
{
//FILE* filePointer;
//int bufferLength = 255;
//char buffer[bufferLength]; /* not ISO 90 compatible */
//
//filePointer = fopen("file.txt", "r");
//
//while(fgets(buffer, bufferLength, filePointer)) {
//    printf("%s\n", buffer);
//}
//
//fclose(filePointer);
//

    //helper for string to double. ex: "1.2345" to dobule 1.2345
    #ifndef DEFINED_DEBUG
        printf("Please input your expression:");
        fgets(buf,bufsize,stdin);
    #endif // DEFINED_DEBUG
    #ifdef DEBUG
        printf("Input expression is : %s\n",buf);
    #endif // DEBUG
    ptr=(char *) buf;
    while(*ptr!=0)
    {
        if(isalnum((int)*ptr) || *ptr=='.')
            queue[++qptr]=*ptr;
        else if(isop(*ptr))
        {
            #ifdef DEBUG
               printf("The operator is %c\n",*ptr);
            #endif // DEBUG
            if (strlen(queue) !=0)
                rvp_push(queue2d(),1);
            opval=op2val(*ptr);
            if (stkptr==-1)
                stack[++stkptr]=opval;
            else
            {
                while(stkptr >-1 && stack[stkptr]!=0 && stack[stkptr]>=opval)
                    rvp_push(stack[stkptr--],2);
                stack[++stkptr]=opval;
            }
        }
        else if(*ptr=='(')
        {
            stack[++stkptr]=0; //6 for (
        }
        else if(*ptr==')')
        {
            rvp_push(queue2d(),1);
            do
                rvp_push(stack[stkptr--],2);
            while(stack[stkptr]!=0);
            stkptr--;
        }
        else if(*ptr=='=')
        {
            varidx=idxofvar(queue);
            if (varidx==-1)
            {
                varidx=varptr;
                strcpy(vars[varptr++],queue);
            }
            queue_reset();
        }
        else if(*ptr==';')
            checkout();
        ptr++;
    }
    #ifdef DEBUG
       printf("The result is %f\n",checkout());
    #else
       printf("%f\n",checkout());
    #endif // DEBUG
    return 0;
}
