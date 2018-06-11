#include "Attendancecheck.h"

int _GetCurrentHour(void) {
	time_t timer = time(NULL);		// 시간에 대한 객체
	tm * t = localtime(&timer);		// 현재시간을 얻는다

	return t->tm_hour;
}

int _GetCurrentMin(void) {
	time_t timer = time(NULL);		// 시간에 대한 객체
	tm * t = localtime(&timer);		// 현재시간을 얻는다

	return t->tm_min;
}

char* _GetCurrentTime(void) {
	char *getTime = new char[20];	// 문자열 버퍼. 반환할 문자열이라서 동적할당을 해야함.

	/*시간과 분 정보를 문자열로 버퍼에 담는다*/
	sprintf(getTime, "%d(h) %d(m)", _GetCurrentHour(), _GetCurrentMin());

	/*문자열을 넘긴다*/
	return getTime;
}

void SignUpQuery(MYSQL *connection, MYSQL conn) {
	char *query = new char[255];	//회원가입 쿼리 문자열 버퍼
	int query_stat;

	Info *info = new Info;

	PRINT_LINE();
	printf("Join Devsign Vacation study \n\n");

	/*사이즈크기만큼 각각 정보를 입력받는다.*/
	printf("Input User name >> ");
	fgets(info->name, 20, stdin);
	CHOP(info->name);

	printf("Input User ID >> ");
	fgets(info->id, 20, stdin);
	CHOP(info->id);


	printf("Input User P/W >> ");
	fgets(info->pw, 20, stdin);
	CHOP(info->pw);
	PRINT_LINE();

	/*
		아이디, 패스워드, 이름, 현재시간에 대한 문자열, 지각횟수를 넘긴다.
		지각횟수는 디폴트가 0이다.
	*/
	sprintf(query, "INSERT INTO DEVSIGN VALUES ("
		" '%s', '%s', '%s', '%s', '%d' );", info->id, info->pw, info->name, _GetCurrentTime(), 0);

	query_stat = mysql_query(connection, query);
	if (query_stat != 0) {
		fprintf(stderr, "MySQL query error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);
	}

	delete info;
	
	return;
}

void UpdateLateness(char *id, MYSQL *connection, MYSQL conn) {
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;

	int query_stat;

	int lateness = 0;

	char queryBuffer[200];

	/* 아이디 값을 이용해서 특정 아이디의 모든 정보를 저장한다. */
	sprintf(queryBuffer, "SELECT * FROM DEVSIGN WHERE ID='%s'", id);	//아이디값으로 정보를 찾아낸다.
	query_stat = mysql_query(connection, queryBuffer);	//쿼리를 날린다.
	if (query_stat != 0) {	//만약 오류라면
		fprintf(stderr, "ID select query error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);		//종료.
	}
	sql_result = mysql_store_result(connection);	//해당 정보를 기억한다.

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		lateness = atoi(sql_row[4]);	//지각 횟수를 저장한다.

	lateness++;			//지각 횟수 증가시킴

	sprintf(queryBuffer, "UPDATE DEVSIGN SET LATENESS = '%d' WHERE ID = '%s'",
		lateness, id);
	query_stat = mysql_query(connection, queryBuffer);
	if (query_stat != 0) {
		fprintf(stderr, "Update Lateness query error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);
	}

	return;
}


void UpdateConnectionTime(char *id, MYSQL *connection, MYSQL conn) {
	int query_stat;

	char queryBuffer[250];

	/*오전 10시 정각 이후에 접속했다면,*/
	if (_GetCurrentHour() >= 10 && _GetCurrentMin() >= 0) {
		UpdateLateness(id, connection, conn);
	}
	
	/*현재시간으로 접속시간을 업데이트한다.*/
	sprintf(queryBuffer, "UPDATE DEVSIGN SET ACCESS_TIME='%s' WHERE ID = '%s'",
		_GetCurrentTime(), id);		//쿼리문 저장
	query_stat = mysql_query(connection, queryBuffer); //쿼리 날리기
	if (query_stat != 0) {		//쿼리가 정상적으로 입력되지 않았다면.
		fprintf(stderr, "Update Connection Time query error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);			//강제종료.
	}

	return;
}

/*로그인 정보를 확인하는 함수.*/
bool LoginCheck(char *id, char *pw, MYSQL *connection, MYSQL conn) {
	MYSQL_ROW sql_row;
	MYSQL_RES *sql_result;
	int pw_check, id_check;

	char query[250];
	sprintf(query, "SELECT * FROM DEVSIGN WHERE ID='%s'", id);	//아이디값으로 정보를 찾아낸다.

	int query_stat = mysql_query(connection, query);	//해당 쿼리를 날린다.
	if (query_stat != 0) {		//해당 아이디가 존재하지 않는다면
		fprintf(stderr, "Not exist ID \n");
		return false;
	}

	/*현재 결과를 저장한다.*/
	sql_result = mysql_store_result(connection);

	/*해당 아이디 값에 따른 패스워드가 일치하는지 확인*/
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		pw_check = strcmp(pw, sql_row[1]);
		id_check = strcmp(id, sql_row[0]);
	}

	if ((pw_check == 0) &&(id_check == 0))	//strcmp()는 같은 문자열이면 0을 반환.
		return true;
	else if(!id_check){
		fprintf(stderr, "The ID is wrong. \n");
		return false;
	}
	else {
		fprintf(stderr, "The PW is wrong. \n");
		return false;
	}
}

/*함수 정의가 밑에 있어서*/
void ShowClientInfo(char *id, MYSQL *connection, MYSQL conn);
void ShowAllClientInfo(MYSQL *connection, MYSQL conn);

void SignInQuery(MYSQL *connection, MYSQL conn) {
	Info info;
	bool check;

	PRINT_LINE();
	/*아이디와 비밀번호를 입력받는다.*/
	printf("Input Your ID : ");
	fgets(info.id, 20, stdin);
	CHOP(info.id);
	
	printf("Input Your P/W : ");
	fgets(info.pw, 20, stdin);
	CHOP(info.pw);
	PRINT_LINE();

	check = LoginCheck(info.id, info.pw, connection, conn);
	
	if (!check) {
		char join;
		char readFromBuffer;

		PRINT_LINE();
		printf("Do you want to join? (Y/N) >>");
		scanf("%c", &join);
		scanf("%c", &readFromBuffer);

		switch (join) {
		case 'Y':
		case 'y':
			SignUpQuery(connection, conn);
			break;

		case 'N':
		case 'n':
			return;

		default:
			fprintf(stderr, "Not appropriate Input value. press Y or N \n");
			break;
		}
		PRINT_LINE();
	}

	/*접속시간을 업데이트 한다.*/
	UpdateConnectionTime(info.id, connection, conn);

	/*관리자 계정 로그인*/
	if (!strcmp(info.id, "admin "))
		ShowAllClientInfo(connection, conn);

	/*일반 로그인이 완료 되었다.*/
	else
		ShowClientInfo(info.id, connection, conn);

	return;
}

/*특정 회원 정보만을 출력*/
void ShowClientInfo(char *id, MYSQL *connection, MYSQL conn) {
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;

	int query_stat;
	char queryBuffer[250];
	sprintf(queryBuffer, "SELECT * FROM DEVSIGN WHERE ID = '%s'", id);	// 해당 아이디 디비를 얻는 쿼리문.

	query_stat = mysql_query(connection, queryBuffer); 
	if (query_stat != 0) {
		fprintf(stderr, "ShowClientInfo query error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);
	}

	sql_result = mysql_store_result(connection);	//쿼리 결과를 저장

	/*필드폭을 지정한 출력*/
	printf("%-10s | %-10s | %-10s | %10s | \n\n", "Name", "ID", "Access_time", "numOfLateness");
	/*한 행의 정보밖에 나오지 않는다.*/
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%-10s | %-10s | %-10s | %10s | \n\n", sql_row[2], sql_row[0],
			sql_row[3], sql_row[4]);
	}
}

/*모든 회원의 정보를 출력*/
void ShowAllClientInfo(MYSQL *connection, MYSQL conn) {
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;

	int query_stat;

	query_stat = mysql_query(connection, "SELECT * FROM DEVSIGN");
	if (query_stat != 0) {
		fprintf(stderr, "ShowAllClientInfo query error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);
	}

	sql_result = mysql_store_result(connection);

	printf("%-10s | %-10s | %-10s | %-10s | %10s | \n\n", "Name", "ID", "P/W", "Access_time", "numOfLateness");
	printf("----------------------------------------------------------------\n");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%-10s | %-10s | %-10s | %-10s | %10s | \n\n", sql_row[2], sql_row[0], sql_row[1],
			sql_row[3], sql_row[4]);
	}
}