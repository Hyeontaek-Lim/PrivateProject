#include "Attendancecheck.h"

typedef enum{
	SIGN_UP = 1,
	SIGN_IN = 2
}SIGN_CONTROL;

int main(void) {
	MYSQL *connection = NULL, conn;
	MYSQL_RES *sql_result;

	int opt;

	/*데이터베이스 초기화*/
	mysql_init(&conn);

	/*출석용 데이터베이스와 연결한다.*/
	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);
	if (connection == NULL) {
		fprintf(stderr, "MySQL connection error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);
	}

	sql_result = mysql_store_result(connection);

	PRINT_LINE();
	printf("********************************************** \n");
	printf("         방학 상주 출석용 프로그램 \n");
	printf("---------- Made by Hyuntaek Lim ---------- \n");
	printf("********************************************** \n\n");

	printf("%s \t\t %s\n", " Pesident", "임현택(3, 24) 010-3073-5573");
	printf("%s \t %s\n", " Semi-president", "윤아선(4, 26) 010-7704-6863");
	printf("%s \t %s\n", " General Affairs", "배동규(3, 25) 010-4920-1380");
	PRINT_LINE();

	while (1) {
		char readFromBuffer;
		PRINT_LINE();
		printf("Option Input \n");
		printf("Sign up(1), Sign in(2) : ");
		scanf("%d", &opt);
		scanf("%c", &readFromBuffer);
		PRINT_LINE();

		switch (opt) {
		case SIGN_UP:
			SignUpQuery(connection, conn);
			break;

		case SIGN_IN:
			SignInQuery(connection, conn);
			break;

		default:
			printf("Not appropriate Input value.press 1 or 2 \n");
		}
		break;
	}
	/*데이터베이스를 이용하기 위한 모든 변수 메모리를 해제한다.*/
	mysql_free_result(sql_result);

	return 0;
}