// *******************************************************************
// *                                                                 *
// *          �V�t������  �I�Z��                                     *
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

#define		CTRL_REQ_WAIT	90
#define		CTRL_REQ_EXIT	99

#define		ADR_PASS		98

//
//
int			Ban[120];
char		Col[8][3]  = {"�`","�a","�b","�c","�d","�e","�f","�g" };
char		Row[8][3]  = {"�P","�Q","�R","�S","�T","�U","�V","�W" };
char		Koma[3][3] = {". ","��","�Z"};
int			Diff[10];
int			StackPtr,NextTurn;
int			NumDir[500],NumDirPtr[70];
int			AddrList[70];
int			PutOKList[30];
int			BlackCtr,WhiteCtr;


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
unsigned long 	xor128();
void	MakeAdrList();
void	EraseExec();
int		MakePutOKList();
int		ComThink();



//------
int		main(int argc,char *argv[])
{
	int		x,y,mv,mvOld;
	char	GetLoc[20];

	CalcDiff();
	MakeAdrList();

	for (x=0;x<9;x++)
		for(y=0;y<9;y++)
			Ban[RowCol2Adr(x,y)]	= NULL;

	StackPtr	= 0;
	NumDirPtr[StackPtr]	= 0;
	
	Ban[RowCol2Adr(3,3)]	= BLACK;
	Ban[RowCol2Adr(4,4)]	= BLACK;
	Ban[RowCol2Adr(3,4)]	= WHITE;
	Ban[RowCol2Adr(4,3)]	= WHITE;
//
//	Ban[RowCol2Adr(0,3)]	= WHITE;
//	Ban[RowCol2Adr(0,4)]	= WHITE;
//	Ban[RowCol2Adr(0,5)]	= WHITE;
//
//	Ban[RowCol2Adr(1,4)]	= WHITE;
//	Ban[RowCol2Adr(1,5)]	= BLACK;
//
//	Ban[RowCol2Adr(2,3)]	= WHITE;
//	Ban[RowCol2Adr(2,4)]	= WHITE;
//	Ban[RowCol2Adr(2,5)]	= WHITE;
//	Ban[RowCol2Adr(2,6)]	= WHITE;
//
//	Ban[RowCol2Adr(3,3)]	= WHITE;
//	Ban[RowCol2Adr(3,4)]	= BLACK;
//	Ban[RowCol2Adr(3,5)]	= BLACK;
//
//	Ban[RowCol2Adr(4,3)]	= WHITE;
//	Ban[RowCol2Adr(4,4)]	= BLACK;

//
	StackPtr	++;
	NextTurn	= BLACK;

	printf("\n");
	DispBan();

//	PutCheckAll();
//	PutExec(RowCol2Adr(2,4));
//	DispBan();
//	DispND();
	
	mv 		= 0;
	mvOld	= 0;
	while (mv < 99){
		//PutCheckAll();
		mvOld	= mv;
		if (NextTurn == BLACK)
			mv	= GetMove();
		else
			mv	= ComThink();
		
		if (mvOld == ADR_PASS && mv == ADR_PASS){
			mv	= CTRL_REQ_EXIT;
		}
			
		if (mv < 90 ){
			if ( PutCheck(mv) == 1 ){
				PutExec(mv);
				//DispND();
				DispBan();
			}
		}
		else if(mv == ADR_PASS){
			PutExec(mv);
			DispBan();
		}
		else if((mv == CTRL_REQ_WAIT) && (1<StackPtr)){
			printf("�҂����@�I\n");
//			DispND();
			EraseExec();
			DispBan();
		}

	}

	return EXIT_SUCCESS;
}
//------
int		ComThink()
{
	int		RndAdr,nn,RndX;

	nn	= MakePutOKList();		// ����\���X�g�쐬
	
	if (nn == 0)
		RndAdr	= ADR_PASS;
	else {
		RndX	= 15 & xor128();
		while (nn <= RndX)
			RndX -= nn;
		RndAdr	= PutOKList[RndX];
	}
	
	return RndAdr;
}
//------
int		MakePutOKList()
{
	int		nn,i;
	
	nn	= 0;
	for (i=0;i<60;i++)
		if (PutCheck(AddrList[i]) == 1){
			PutOKList[nn]	= AddrList[i];
			nn ++;
		}

	return nn;
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
	char	GetLoc[20],*ptr,f_wait,f_exit,LocR,LocC,adr,f_pass;
	
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
		if (*ptr == 'P')
			f_pass	= 1;
		ptr ++;
	}
	adr = RowCol2Adr(LocR,LocC);
	if (f_wait == 1)
		adr	= CTRL_REQ_WAIT;
	if (f_exit == 1)
		adr	= CTRL_REQ_EXIT;	
	if (f_pass == 1)
		adr	= ADR_PASS;	
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

	printf("Stack : %d  NDPtr : %d\n",StackPtr,NumDirPtr[StackPtr-1]);
	for(i=0;i<NumDirPtr[StackPtr-1];i++) {
		if( NumDir[i] != -99)
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
void	EraseExec()
{
	int		AdrWork,Dir,Adr,NDPtr,RvsCtr,i;

	StackPtr --;
	
	NDPtr	= NumDirPtr[StackPtr-1];
	Adr		= NumDir[NDPtr ++];		// location
	if (Adr != ADR_PASS){
		Ban[Adr]	= 0;

		RvsCtr	= NumDir[NDPtr ++];		// 
		while(0<RvsCtr){
			Dir	= NumDir[NDPtr ++];
			AdrWork	= Adr + Dir;
			for(i=0;i<RvsCtr;i++){
				Ban[AdrWork]	= NextTurn;
				AdrWork += Dir;
			}
			RvsCtr	= NumDir[NDPtr ++];		// 
		}
	}

	NextTurn	= 3 - NextTurn;
}
//------
void	PutExec(int Adr)
{
	int		AdrWork,Dir,DirPtr,NDPtr,RvsCtr,i;
	
	DirPtr	= 0;
	NDPtr	= NumDirPtr[StackPtr-1];
	NumDir[NDPtr ++]	= Adr;		// location
	if (Adr != ADR_PASS){
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
	}
	NumDir[NDPtr++]	= -99;
	NumDirPtr[StackPtr]	= NDPtr;
	
	NextTurn	= 3 - NextTurn;
	StackPtr ++;
}
//------
int		PutCheck(int Adr)
{
	int		IsPutOK,AdrWork,Dir,DirPtr;
//	int		DbgFlag;
	
	IsPutOK		= 0;
	
//	DbgFlag	= 0;
//	if(Adr == RowCol2Adr(3,7)){
//		printf("spe\n");
//		DbgFlag	 =1;
//	}
	
	if (Ban[Adr] == 0)
	{
		DirPtr	= 0;
		while((IsPutOK == 0) && (DirPtr<8))
		{
			Dir	= Diff[DirPtr];
			AdrWork	= Adr + Dir;
//			if (DbgFlag ==1)
//				printf("%3d  %3d  %3d  \n",Dir,AdrWork,DirPtr);
			if (Ban[AdrWork] == (3 - NextTurn)){
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
void	CountBW()
{
	int 	i;
	
	BlackCtr	= 0;
	WhiteCtr	= 0;
	for (i=12;i <= AddrList[59];i++){
		if (Ban[i] == BLACK)
			BlackCtr	++;
		if (Ban[i] == WHITE)
			WhiteCtr	++;
	}
}
//------
void	DispBan()
{
	int		x,y;
	
	CountBW();
	
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
		case 4:
			printf("\tBlack : %d",BlackCtr);
			break;
		case 5:
			printf("\tWhite : %d",WhiteCtr);
			break;
		}
		printf("\n");
	}
	printf("\n");
	
}

//------
void	MakeAdrList()
{
	int		x,y,i;
	
	i=0;
	for (x=0;x<8;x++)
		for(y=0;y<8;y++){
			if ((x<3 || 4<x) || (y<3 || 4<y)){
				AddrList[i]	= RowCol2Adr(x,y);
				i++;
			}
		}
	
//	for(i=0;i<60;i++)
//		printf("%3d,",AddrList[i]);
//	printf("\n");
}
//------

unsigned long xor128(){ 
    static unsigned long x=123456789,y=362436069,z=521288629,w=88675123; 
    unsigned long t; 
    t=(x^(x<<11));
    x=y;
    y=z;
    z=w;
    return( w=(w^(w>>19))^(t^(t>>8)) ); 
} 
//*******************************************************************
//*                              E N D                              *
//*******************************************************************
