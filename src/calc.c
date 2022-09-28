#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>
//gcc -lm
const char opr[]={'*','/','+','-',0};
const int opr_sz=5;
#define NO_MEMORY
//que sigue?
//definir operaciones de misma jerarquia.
double sumar(double d1,double d2){
	return d1+d2;
}
double restar(double d1,double d2){
	return d1-d2;
}
double multiplicar(double d1,double d2){
	return d1*d2;
}
double dividir(double d1,double d2){
	return d1/d2;
}
typedef  double (*FUNCION_OPERACION)(double,double);
const FUNCION_OPERACION operaciones[]={&multiplicar,&dividir,&sumar,&restar};
const int nro_funciones=4;
char *opizq(char *str,char *operador)
{
    char *ptr,*opprev;
    opprev=ptr=str;
    while((ptr=strpbrk(ptr,opr))<=operador){
       if(ptr==operador || ptr==NULL)
           break;
       opprev=ptr;
       ptr++;
    }
    if(opprev!=str && *opprev!='-')
       opprev++; 
    
    return opprev;
}

char *opder(char *str,char *op)
{
    char *opsig;
    opsig=op+1;
    while(*opsig=='-' && *opsig!=0)
        opsig++;
    opsig=strpbrk(opsig,opr);
    if(opsig==NULL)
        opsig=str+strlen(str);
    return opsig;
}
typedef struct pardedobles{
    double d1;
    double d2;
}PARDEDOBLES;
PARDEDOBLES getNumbersFromOperation(char *str,char *op)
{
    PARDEDOBLES d;
    char *isolatedoperation,*opi,*opd;
    int isolatedoperationsize;
    opi=opizq(str,op);
    opd=opder(str,op);
    isolatedoperationsize=opd-opi;//tamagno de la operacion aislada
    isolatedoperation=(char*)calloc(isolatedoperationsize+1,sizeof(*isolatedoperation));
    strncpy(isolatedoperation,opi,isolatedoperationsize);
    sscanf(isolatedoperation,"%lf%*c%lf",&d.d1,&d.d2);
    free(isolatedoperation);
    return d;
}
typedef struct operador{
	int opr_idx;
	char* ptr;
}OPERADOR;
OPERADOR encontrarOperador(char *str)
{
///busco los operadores en el orden que estan en opr
    int i;
    char *ptr=NULL;//FIXME: deberia inicializar en NO OP
	OPERADOR operador={0,NULL};
	if(*str=='-')
		str++;
	for(i=0;i<opr_sz;i++){
        ptr=strchr(str,opr[i]);
        if(ptr!=NULL)
            break;
    }
	operador.opr_idx=i;
	operador.ptr=ptr;
	return operador;
}
enum estado{RESUELTO,NO_RESUELTO,DIVISION_POR_CERO};
typedef struct rslt{
    double result;
	char *cadOp;
    enum estado state_id;
}RESULT;
char states[][30]={"RESUELTO","NO_RESUELTO","DIVISION POR CERO"};

RESULT actualizarCadOp(char *operptr,RESULT res){
    char *opi,*opd,*str2;
	int strSize=strlen(res.cadOp)*8;
    opi=opizq(res.cadOp,operptr);
    opd=opder(res.cadOp,operptr);
	str2=malloc(strSize);
	*str2=0;
	strncpy(str2,res.cadOp,opi-res.cadOp);
	snprintf(str2+strlen(str2),strSize,"%s%.3lf%s",str2,res.result,opd);
	free(res.cadOp);
	res.cadOp=str2;
	return res;
}
RESULT resolverUna(char *str)
{
    PARDEDOBLES d;
    RESULT res={0,str,NO_RESUELTO};
	OPERADOR op=encontrarOperador(str);
	if(op.ptr==NULL){//NULL SERIA COMO UN NOOP
		res.result=atof(str);
		res.state_id=RESUELTO;
		return res;
	}
     d=getNumbersFromOperation(str,op.ptr);
     //resolver
    res.result=	operaciones[op.opr_idx](d.d1,d.d2);
	res=actualizarCadOp(op.ptr,res);
    return res;
}

int elFin(RESULT r,char *str)
{
    char dstr[16];
    snprintf(dstr,16,"%.3lf",r.result);
    /* habria que hacer mas cosas pero... */
    return strcmp(dstr,str);
}

int main(int argc,char **argv)
{
    RESULT r;
	r.cadOp=(char*)calloc(8,sizeof(*r.cadOp));
	fprintf(stderr,"ingrese los datos: ");
	fgets(r.cadOp,8,stdin);//	scanf("%8s",r.cadOp);
	while(r.cadOp[strlen(r.cadOp)-1]!='\n'){
		r.cadOp=realloc(r.cadOp,strlen(r.cadOp)*2);
		fgets(r.cadOp+strlen(r.cadOp),strlen(r.cadOp),stdin);
	}

	r=resolverUna(r.cadOp);
    while(r.state_id==NO_RESUELTO && elFin(r,r.cadOp)!=0){
		r=resolverUna(r.cadOp);
    }
	//POR HACER: utilizar punteros a funciones a la hora de evaluar el estado
	//No user if-elses
	if(r.state_id==DIVISION_POR_CERO)
		puts(states[r.state_id]);
	else{
	    printf("%.3lf\n",r.result);
		free(r.cadOp);
	}
    return 0;
}
/**TOP-DOWN
 * 0) HACER UNA CALCULADORA QUE RESUELVA OPERACIONES COMBINADA DE * / + -
 * 1) recivir una cadena por argumento y se duplica en el programa
 * 2) procesar la cadena duplicada para cada una de las operaciones
 *
 * 2.1) para cada caracter de operacion notado como acop
 * 2.2) mientras que queden caracteres de operacion 
 * 2.3) delimitar la operacion dentro de la cadena de caracteres
 * 2.4) Leer/procesar los numeros a ser operados
 * 2.5) Operar con los numeros obteniendo un resultado
 * 2.6) Insertar el resultado dentro de la cadena de caracteres reemplazando la operacion anterior
 * 2.7) Buscaar el siguiente caracter de operacion
 * 2.8)------------
 *  2.3)delimitar la operacion dentro de la cadena de caracteres
 *      //una operacion esta delimitada dentro de una cadena de caracteres por
 *      // [cop o inicioCadena ][ - o \nada ][numero flotante][acop][- o \nada][numero flotante][cop o finCadena]
 *      // cop= Caracter de operacion; acop= Caracter de operacion actual *
 *      .1) Buscar en la cadena de caracteres el acop
 *      .2) Buscar en la cadena de caracteres la operacion anterior al acop, opant, o el inicio de la cadena.
 *      .3) Buscar en la cadena de caracteres la operacion siguiente al acop,opsig, o el fin de la cadena.
 *      .4) La operacion esta delimitada entre opant y opsig
 *      2.3.2)  Buscar en la cadena de caracteres la operacion anterior al acop, opant, o el inicio de la cadena.
 *              Habiendo encontrado un caracter de operacion
 *              .1) Si el acop es '-'entonces
 *              .2) Si el caracter anterior es una operacion entonces elegir
 *              el caracter anterior
 *  2.4)  Leer/procesar los numeros a ser operados
 *      // teniendo los limites de la operacion, opant y opsig
 *      .1) Copiar en otra cadena,sop, la cadena de operacion entre opant y opsig, para hacerla mas simple.
 *      .2) Leer desde la sop los numeros flotantes n1 y n2
 *      .3) Los numeros a operar son n1 y n2
 *  2.5) Operar con los numeros obteniendo un resultado
 *      Se operara con los numero n1 y n2
 *      .1)connmutar acop
 *      .2) caso '*' ....
 *      .x)...
 *      .y) se devuelve el resultado, res, como numero decimal
 * 2.6) Insertar el resultado dentro de la cadena de caracteres reemplazando la operacion anterior.
 *      .1)
 *
 * **/
