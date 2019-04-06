// *******************************************************************
// *                                                                 *
// *          êVètèâîÑÇË  ÉIÉZÉç                                     *
// *                                                                 *
// *              Programed by T.Kosugi    [Dec 17.1996]             *
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
int			StackPtr;
int			DirNum[500],DirNumPtr[70];
//


int		RowCol2Adr(int row,int col);
void	DispBan();
void	CalcDiff();
void	PutCheck(int x,int y,int BxW);

//------
int		main(int argc,char *argv[])
{
	int		x,y;

	for (x=0;x<8;x++)
		for(y=0;y<8;y++)
			Ban[RowCol2Adr(x,y)]	= NULL;
	
	Ban[RowCol2Adr(3,3)]	= BLACK;
	Ban[RowCol2Adr(4,4)]	= BLACK;
	Ban[RowCol2Adr(3,4)]	= WHITE;
	Ban[RowCol2Adr(4,3)]	= WHITE;

	StackPtr	= 0;
	DirNumPtr[StackPtr]	= 0;

	DispBan();
	CalcDiff();

	return EXIT_SUCCESS;
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
void	DispBan()
{
	int		x,y;
	
	printf("\nÅ@");
	for (x=0;x<8;x++)
		printf("%s",Col[x]);
	printf("\n");

	for (y=0;y<8;y++){
		printf("%s",Row[y]);
		for (x=0;x<8;x++)
			printf("%s",Koma[Ban[RowCol2Adr(y,x)]]);
		printf("\n");
	}
	
}
//*******************************************************************
//*                              E N D                              *
//*******************************************************************
