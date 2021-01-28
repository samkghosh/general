#include <stdlib.h>
#include"database.h"

int main()
{
	_CArray2D arry;
char strSql[200];
DataBase db;
char szVal[32];
int i;
strcpy(strSql,"select i_channelid, v_tappingno from tbl_channels");
printf("Connecting database\n");
fflush(stdout);
if((i=db.Connect("127.0.0.1","root","12345","arcane411"))!= DB_SUCCESS)
{
	printf("faild to connect database");
        fflush(stdout);
        return -1;
}
printf("Conn res=%d\n", i);
printf("Database connected\n");
fflush(stdout);
 int nMaxTotalRec=0;
 if ( db.FetchRecord(strSql,&arry,40,&nMaxTotalRec) != DB_SUCCESS)
 {
	printf("Datafetch error::%s\n", db.szErrorMsg);	
	return 0;
 } 

printf("Max Rec found=%d\n",nMaxTotalRec); 

  for(int i =1; i <= nMaxTotalRec; i++)
  {
	printf("Id=%s, TBN=%s\n", arry.get(i,1, szVal), arry.get(i,2, szVal));
//	printf("Chanid=%s", arry.get(i,1));
//	printf("\tTPN=%s\n", arry.get(i,2));
 } 

  
strcpy(strSql,"INSERT INTO tbl_alarm (d_DateTime) VALUES (NOW())");
db.ExecuteQuery(strSql);
   return 0;
}
