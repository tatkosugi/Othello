// *******************************************************************
// *                                                                 *
// *          新春初売り  オセロ                                     *
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
int			Ban[120],BanScore[120],BanScoreWk[120];
char		Col[8][3]  = {"Ａ","Ｂ","Ｃ","Ｄ","Ｅ","Ｆ","Ｇ","Ｈ" };
char		Row[8][3]  = {"１","２","３","４","５","６","７","８" };
char		Koma[3][3] = {". ","●","〇"};
char		NextChr[3][10] = {"NULL","Black","White"};
int			Diff[10];
int			StackPtr,NextTurn;
int			NumDir[500],NumDirPtr[80];
int			AddrList[70];
int			PutOKList[30];
int			BlackCtr,WhiteCtr;
int			ScoreTable[4][4] 	= { { 20,0,0,0 } , { -6,-6,0,0} , {4,10,12,0} , {9,6,18,0} };
int			ScoreTableWk[4][4]	= { { 20,0,0,0 } , { -6,-6,0,0} , {4,8,12,0} , {6,10,18,0} };
int			TraceMode;
int			AdrMax;

//


unsigned long 	xor128();
void	InitBScore();
void	InitBScoreWk();
int		RowCol2Adr(int row,int col);
void	DispBan();
void	CalcDiff();
int		PutCheck(int );
void	PutCheckAll();
void	PutExec(int Adr);
void	DispND();
int		GetMove();
char	ToUpper(char cc);
void	MakeAdrList();
void	EraseExec();
int		MakePutOKList();
int		ComThink(int no);
int		ComThink_Random();
int		ComThink_RandWin();
int		ComThink_RSWin();
int		ComThink_RScore();
int		IsLastPass();
void	CountBW();
int		GetWinEval();
void	SortPutOKList(int *AdrList,int nn);
int		GetMaxEval(int Max)	;
int		ExecOneGame();


//------
int		main(int argc,char *argv[])
{
	time_t	ti;

	CalcDiff();
	MakeAdrList();

	ti	= 0x3ff & time(NULL);
	for (int i=0;i<ti;i++)
		xor128();
//
	InitBScore();
	InitBScoreWk();

	TraceMode	=  1;

	for (int i=0;i<20;i++)
		ExecOneGame();

	return EXIT_SUCCESS;
}
//------
int		ExecOneGame()
{
	int		x,y,mv,mvOld;
	char	GetLoc[20];

	for (x=0;x<9;x++)
		for(y=0;y<9;y++)
			Ban[RowCol2Adr(x,y)]	= NULL;

	Ban[RowCol2Adr(3,3)]	= BLACK;
	Ban[RowCol2Adr(4,4)]	= BLACK;
	Ban[RowCol2Adr(3,4)]	= WHITE;
	Ban[RowCol2Adr(4,3)]	= WHITE;
//
	StackPtr	= 0;
	NumDirPtr[StackPtr]	= 0;

	StackPtr	++;
	NextTurn	= BLACK;

//	printf("\n");
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
//		if (StackPtr == 40)
//			TraceMode	=  0;
		if (NextTurn == BLACK)
			mv	= ComThink(1);
//			mv	= GetMove();
		else
			mv	= ComThink(4);
		
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
			printf("待った　！\n");
//			DispND();
			EraseExec();
			DispBan();
		}

	}

	CountBW();
	int		Result	= BlackCtr - WhiteCtr;
	printf("Game Result : %d\n",Result);

	return		Result;
}
//------
int		ComThink(int no)
{
	int		ret;
	
	switch (no){
	case 1:
		ret	= ComThink_Random();
		break;
	case 2:
		ret	= ComThink_RScore();
		break;
	case 3:
		ret	= ComThink_RandWin();
		break;
	case 4:
		ret	= ComThink_RSWin();
		break;
	}
	
	return	ret;
}
//------
int		ComThink_RandWin()
{
	int		RndAdr,nn,RndX;

	nn	= MakePutOKList();		// 着手可能リスト作成
	
	if (nn == 0)
		RndAdr	= ADR_PASS;
	else {
		if (StackPtr <48){
			RndX	= 15 & xor128();
			while (nn <= RndX)
				RndX -= nn;
			RndAdr	= PutOKList[RndX];
		}
		else{
			int		winval;
			winval	= GetWinEval();
			printf("My score is %d\n",winval);
			RndAdr	= AdrMax;
		}
	}
	
	return RndAdr;
}
//------
int		ComThink_Random()
{
	int		RndAdr,nn,RndX;

	nn	= MakePutOKList();		// 着手可能リスト作成
	
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
int		ComThink_RScore()
{
	int		RetAdr,nn,RndX,Score,MaxScore,i;

	nn	= MakePutOKList();		// 着手可能リスト作成
	
	if (nn == 0)
		RetAdr	= ADR_PASS;
	else {
		MaxScore	= -9999;
		for (i=0;i<nn;i++){
			Score	= BanScore[PutOKList[i]] + ( 3 &xor128());
			if (MaxScore < Score){
				RetAdr		= PutOKList[i];
				MaxScore	= Score;
			}
		}
	}
	
	return RetAdr;
}
//------
int		ComThink_RSWin()
{
	int		RetAdr,nn,RndX,Score,MaxScore,i;
	int		winval;

	nn	= MakePutOKList();		// 着手可能リスト作成
	
	if (nn == 0)
		RetAdr	= ADR_PASS;
	else {
		if (StackPtr <48){
			MaxScore	= -9999;
			for (i=0;i<nn;i++){
				Score	= BanScore[PutOKList[i]] + ( 3 &xor128());
				if (MaxScore < Score){
					RetAdr		= PutOKList[i];
					MaxScore	= Score;
				}
			}
		}
	//	else if (StackPtr <50){
	//		winval	= GetWinEval();
	//		printf ("[%s] My Score is %d\n",NextChr[NextTurn],winval);
	//		RetAdr	= AdrMax;
	//	}
		else{
			winval	= GetMaxEval(-99);
//			printf ("[%s] My Score is %d\n",NextChr[NextTurn],winval);
			RetAdr	= AdrMax;
		}
	}
	
	return RetAdr;
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
void	InitBScoreWk()
{
	int		x,y,Adr;
	
	for (x=0;x<4;x++)
		for (y=0;y<=x;y++){
			Adr	= RowCol2Adr(x,y);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(y,x);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(7 - x ,y);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(7 - y ,x);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(x,7 - y);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(y,7 - x);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(7 - x,7 - y);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(7 - y,7 - x);
			BanScore[Adr]	= ScoreTable[x][y];
		}
		
}
//------
void	InitBScore()
{
	int		x,y,Adr;
	
	for (x=0;x<4;x++)
		for (y=0;y<=x;y++){
			Adr	= RowCol2Adr(x,y);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(y,x);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(7 - x ,y);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(7 - y ,x);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(x,7 - y);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(y,7 - x);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(7 - x,7 - y);
			BanScore[Adr]	= ScoreTable[x][y];
			Adr	= RowCol2Adr(7 - y,7 - x);
			BanScore[Adr]	= ScoreTable[x][y];
		}
		
	for (y=0;y<8;y++){
		for (x=0;x<8;x++)
			printf("%3d ,",BanScore[RowCol2Adr(x,y)]);
		printf("\n");
	}

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
//
//	完全読み
//
//今回着手可能位置なし、かつ前回着手可能位置なしのとき評価値を返す
//
//今回の着手可能リストを作成する。
//
//着手可能リストを並べ替える。
//
//	上位が確保しているスコア(MAX)を超える着手があれば、打ち切り
//  そうでない場合はMAXを最大にする手を探す
//
int			GetMaxEval(int Max)		// 戻り値は評価結果
{
	int		LimitMax,nn,LocalMax,EvalMax,i,Eval;
	int		SortAdrList[20];
	
	LimitMax	= - Max;

	nn	= MakePutOKList();		// 着手可能リスト作成

	if (nn == 0) {
//		printf(" pass\n");
		if (IsLastPass()){
			CountBW();
			if (NextTurn == BLACK)
				EvalMax	= BlackCtr - WhiteCtr;
			else
				EvalMax	= WhiteCtr - BlackCtr;
		}
		else {
			PutExec(ADR_PASS);
			EvalMax	= - GetMaxEval(LimitMax);
			EraseExec();
		}
		LocalMax	= ADR_PASS;
	}
	else{
		SortPutOKList(SortAdrList,nn);
		
		EvalMax	= -99;
		LocalMax	= SortAdrList[0];
		i = 0;
		while (i<nn){
			PutExec(SortAdrList[i]);
			Eval	= - GetMaxEval(EvalMax);
			EraseExec();
			
			if (EvalMax < Eval){
				EvalMax	= Eval;
				LocalMax	= SortAdrList[i];
			}
			if (LimitMax <= EvalMax )
				i = 99;
			else
				i++;

//			printf("select : %3d , ",LocalMax);
//			printf("Score is  : %3d \n",EvalMax);
		}
	}

	AdrMax	= LocalMax;
//	printf("<< stack = %d \n",StackPtr);

	return	EvalMax;
}
//------
//
//	必勝読み
//
//今回着手可能位置なし、かつ前回着手可能位置なしのとき評価値を返す
//
//今回の着手可能リストを作成する。
//
//着手可能リストを並べ替える。
//
//	調べた結果が +1 以上となれば、+1を評価値として返す。
//  すべて調べても 0 以下であれば、0 を評価値として返す
//
//
//
int			GetWinEval()		// 戻り値は評価結果
{
	int		nn,EvalMax,i,Eval,LocalMax;
	int		SortAdrList[20];

	
//	char	GetLoc[10];
//	printf(">>stack = %d :",StackPtr);
//	scanf("%s",GetLoc);

	nn	= MakePutOKList();		// 着手可能リスト作成

	if (nn == 0) {
//		printf(" pass\n");
		if (IsLastPass()){
			CountBW();
			if (NextTurn == BLACK)
				EvalMax	= BlackCtr - WhiteCtr;
			else
				EvalMax	= WhiteCtr - BlackCtr;
		}
		else {
			PutExec(ADR_PASS);
			EvalMax	= - GetWinEval();
			EraseExec();
		}
		LocalMax	= ADR_PASS;
	}
	else {
		SortPutOKList(SortAdrList,nn);

//		for (i=0;i<nn;i++)
//			printf("%3d  ",SortAdrList[i]);
//		printf(" is source and ");

		EvalMax	= -99;
		LocalMax	= SortAdrList[0];
		i = 0;
		while (i<nn){
			PutExec(SortAdrList[i]);

			Eval	= - GetWinEval();
			EraseExec();
			
			if (EvalMax < Eval){
				EvalMax	= Eval;
				LocalMax	= SortAdrList[i];
			}
			if (EvalMax < 1)
				i++;
			else
				i = 99;

//			printf("select : %3d , ",LocalMax);
//			printf("Score is  : %3d \n",EvalMax);
		}
	}

//	for (i=0;i<nn;i++)
//		printf("> %3d  ",SortAdrList[i]);
//	printf("\n(Best adr = %d) \n",LocalMax);
//	printf("<< stack = %d \n",StackPtr);
//	printf("Score is  : %3d \n",EvalMax);
	
	AdrMax	= LocalMax;

	return	EvalMax;
}
//------
int		IsLastPass()
{
	int		Adr,NDPtr;
	
	NDPtr	= NumDirPtr[StackPtr-2];
	Adr		= NumDir[NDPtr];		// location
	if (Adr == ADR_PASS)
		return	1;
	else
		return	0;
}
//------
void	SortPutOKList(int *AdrList,int nn)
{
	int		i;
	
	for (i=0;i<nn;i++)
		AdrList[i]	= PutOKList[i];
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
	
	if (TraceMode == 0){
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
				printf("\tOthello Ver 1.00 [Apr.12.2019]");
				break;
			case 2:
				printf("\tNext turn is ");
				printf("%s",NextChr[NextTurn]);
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
