#ifndef __ATTENDANCE_CHECK_H___
#define __ATTENDANCE_CHECK_H___

#include <cstdio>
#include <iostream>
#include <string>
#include <ctime>
#include <my_global.h>
#include <WinSock2.h>
#include <mysql.h>
#include <cstdlib>

#pragma comment(lib, "libmysql.lib")
#pragma warning(disable:4996)

#define DB_HOST		"127.0.0.1"
#define DB_USER		"root"
#define DB_PASS		"lht1080"
#define DB_NAME		"DEVSIGN"
#define CHOP(x)		x[strlen(x) - 1] = ' '

#define PRINT_LINE()	printf("\n-----------------------------------------------\n\n")

using namespace std;

/* 한 사람의 정보를 담은 구조체 */
typedef struct info {
	char id[20];	//아이디
	char pw[20];	//패스워드
	char name[20];	//이름
}Info;

/* 접속시간, 지각횟수 저장 */
typedef struct _etc {
	char *accessTime;
	int numOfLateness;
}Etc;

/*회원가입 쿼리를 날린다*/
void SignUpQuery(MYSQL *connection, MYSQL conn);

/*로그인 쿼리를 날린다*/
void SignInQuery(MYSQL *connection, MYSQL conn);


#endif // !__ATTENDANCE_CHECK_H___