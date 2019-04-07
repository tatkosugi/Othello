// *******************************************************************
// *                                                                 *
// *          êVètèâîÑÇË  ÉIÉZÉç                                     *
// *                                                                 *
// *              Programed by T.Kosugi    [Mar 07.2019]             *
// *                                                                 *
// *                                                                 *
// *   1.02  1996.12.17     First release                            *
// * version  date ymd                                               *
// *                                                                 *
// *******************************************************************
//
#include	<ctype.h>
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	<time.h>
//
//
//
#define		HIGH			1
#define		LOW				0

#define		BLACK			1
#define		WHITE			2
#define		NULL			0


//
//
int			Ban[100];
char		Col[8][3]  = {"Ç`","Ça","Çb","Çc","Çd","Çe","Çf","Çg" };
char		Row[8][3]  = {"ÇP","ÇQ","ÇR","ÇS","ÇT","ÇU","ÇV","ÇW" };
char		Koma[3][3] = {". ","Åú","ÅZ"};
int			Diff[10];
int			StackPtr,NextTurn;
int			NumDir[500],NumDirPtr[70];
//


int		RowCol2Adr(int row,int col);
void	DispBan();
void	CalcDiff();
int		PutCheck(int );
void	PutCheckAll();
void	PutExec(int Adr);
void	DispND();
int		GetMove();
char	ToUpper(char cc);



//------
int		main(int argc,char *argv[])
{
	int		x,y,mv;
	char	GetLoc[20];

	CalcDiff();

	for (x=0;x<8;x++)
		for(y=0;y<8;y++)
			Ban[RowCol2Adr(x,y)]	= NULL;
	
	Ban[RowCol2Adr(3,3)]	= BLACK;
	Ban[RowCol2Adr(4,4)]	= BLACK;
	Ban[RowCol2Adr(3,4)]	= WHITE;
	Ban[RowCol2Adr(4,3)]	= WHITE;

	StackPtr	= 0;
	NumDirPtr[StackPtr]	= 0;
	NextTurn	= BLACK;

	printf("\n");
	DispBan();

//	PutCheckAll();
//	PutExec(RowCol2Adr(2,4));
//	DispBan();
//	DispND();
	
	mv = 0;
	while (mv < 90){
		mv	= GetMove();
		if (mv < 90)
			if ( PutCheck(mv) == 1 ){
				PutExec(mv);
				DispBan();
			}
	}

	return EXIT_SUCCESS;
}
//------
char		ToUpper(char cc)
{
	char res;

	if ('a'<=cc && cc<='z')
		res = cc + 'A'- 'a';
	else
		res = cc;

	return res;
}
//------
int		GetMove()
{
	char	GetLoc[20],*ptr,f_wait,f_exit,LocR,LocC,adr;
	
	printf("Input the location (ex. D3) : ");
	scanf("%s",GetLoc);
	printf("\n");
	ptr	= GetLoc;
	f_wait	= 0;
//	f_pass	= 0;
	LocR	= 0;
	LocC	= 0;
	while (*ptr != 0){
		*ptr	= ToUpper(*ptr);
		if (('A' <= *ptr) && (*ptr <= 'H'))
			LocC	= *ptr - 'A';
		if (('1' <= *ptr) && (*ptr <= '8'))
			LocR	= *ptr - '1';
		if (*ptr == 'W')
			f_wait	= 1;
		if (*ptr == 'X')
			f_exit	= 1;
		ptr ++;
	}
	adr = RowCol2Adr(LocR,LocC);
	if (f_wait == 1)
		adr	= 90;
	if (f_exit == 1)
		adr	= 99;	
//	printf ("%d %d %d %d\n",LocC,LocR,f_wait,adr);
//	if (f_pass == 1)
//		adr = 99;

	return	adr;
}
//------
int		RowCol2Adr(int row,int col)
{
	return	row * 10 + col + 12;
}
//------
void	CalcDiff()
{
	int		x,y,bx,by,badr,i;
	
	bx=3;
	by=3;
	badr = RowCol2Adr(bx,by);
	i	= 0;
	for (x=-1;x<=1;x++)
		for(y=-1;y<=1;y++){
			if ((x != 0) || (y != 0)){
				Diff[i]	= RowCol2Adr(bx+x,by+y) - badr;
//				printf ("%2d %2d\n",i,Diff[i]);
				i ++;
			}
		}

}
//------
void	DispND()
{
	int		i;

	for(i=0;i<NumDirPtr[StackPtr-1];i++)
	{
		if(0 < NumDir[i])
			printf("%2d , ",NumDir[i]);
		else
			printf("\n");
	}
}
//------
void	PutCheckAll()
{
	int x,y;
	
	for(x=0;x<8;x++){
		for(y=0;y<8;y++)
			if (PutCheck(RowCol2Adr(x,y)) == 1)
				printf("X");
			else
				printf(".");
		printf("\n");
	}
}
//------
void	PutExec(int Adr)
{
	int		AdrWork,Dir,DirPtr,NDPtr,RvsCtr,i;
	
	DirPtr	= 0;
	NDPtr	= NumDirPtr[StackPtr];
	NumDir[NDPtr ++]	= Adr;		// location
	Ban[Adr]	= NextTurn;
	while(DirPtr < 8)
	{
		Dir	= Diff[DirPtr];
		AdrWork	= Adr + Dir;
		if (Ban[AdrWork] == 3 - NextTurn){
			AdrWork += Dir;
			RvsCtr	= 1;
			while(((Ban[AdrWork] & 3) != 0) && (0 < RvsCtr)){
				if (Ban[AdrWork] == NextTurn){
					NumDir[NDPtr ++]	= RvsCtr;
					NumDir[NDPtr ++]	= Dir;
					AdrWork	= Adr + Dir;
					for (i=0;i<RvsCtr;i++){
						Ban[AdrWork]	= NextTurn;
						AdrWork += Dir;
					}
					RvsCtr	= -1;
				}
				else {
					AdrWork += Dir;
					RvsCtr	++;
				}
			}
		}
		DirPtr	++;
	}
	NumDir[NDPtr++]	= -1;
	NumDirPtr[StackPtr]	= NDPtr;
	
	NextTurn	= 3 - NextTurn;
	StackPtr ++;
}
//------
int		PutCheck(int Adr)
{
	int		IsPutOK,AdrWork,Dir,DirPtr;
	
	IsPutOK		= 0;
	
	if (Ban[Adr] == 0)
	{
		DirPtr	= 0;
		while((IsPutOK == 0) && (DirPtr<8))
		{
			Dir	= Diff[DirPtr];
			AdrWork	= Adr + Dir;
			if (Ban[AdrWork] == 3 - NextTurn){
				AdrWork += Dir;
				while(((Ban[AdrWork] & 3) != 0)  && ! IsPutOK){
					if (Ban[AdrWork] == NextTurn)
						IsPutOK	= 1;
					else
						AdrWork += Dir;
				}
			}
			DirPtr	++;
		}
	}
	
	return		IsPutOK;
}
//------
void	DispBan()
{
	int		x,y;
	
	printf("  ");
	for (x=0;x<8;x++)
		printf("%s",Col[x]);
	printf("\n");

	for (y=0;y<8;y++){
		printf("%s",Row[y]);
		for (x=0;x<8;x++)
			printf("%s",Koma[Ban[RowCol2Adr(y,x)]]);
		switch (y){
		case 0:
			printf("\tOthello Ver 1.00 [Mar.07.2019]");
			break;
		case 2:
			printf("\tNext turn is ");
			if (NextTurn == 1)
				printf("Black");
			else
				printf("White");
			break;
		}
		printf("\n");
	}
	printf("\n");
	
}
//*******************************************************************
//*                              E N D                              *
//*******************************************************************
