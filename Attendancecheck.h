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

/* �� ����� ������ ���� ����ü */
typedef struct info {
	char id[20];	//���̵�
	char pw[20];	//�н�����
	char name[20];	//�̸�
}Info;

/* ���ӽð�, ����Ƚ�� ���� */
typedef struct _etc {
	char *accessTime;
	int numOfLateness;
}Etc;

/*ȸ������ ������ ������*/
void SignUpQuery(MYSQL *connection, MYSQL conn);

/*�α��� ������ ������*/
void SignInQuery(MYSQL *connection, MYSQL conn);


#endif // !__ATTENDANCE_CHECK_H___