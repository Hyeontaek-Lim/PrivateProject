#include "Attendancecheck.h"

int _GetCurrentHour(void) {
	time_t timer = time(NULL);		// �ð��� ���� ��ü
	tm * t = localtime(&timer);		// ����ð��� ��´�

	return t->tm_hour;
}

int _GetCurrentMin(void) {
	time_t timer = time(NULL);		// �ð��� ���� ��ü
	tm * t = localtime(&timer);		// ����ð��� ��´�

	return t->tm_min;
}

char* _GetCurrentTime(void) {
	char *getTime = new char[20];	// ���ڿ� ����. ��ȯ�� ���ڿ��̶� �����Ҵ��� �ؾ���.

	/*�ð��� �� ������ ���ڿ��� ���ۿ� ��´�*/
	sprintf(getTime, "%d(h) %d(m)", _GetCurrentHour(), _GetCurrentMin());

	/*���ڿ��� �ѱ��*/
	return getTime;
}

void SignUpQuery(MYSQL *connection, MYSQL conn) {
	char *query = new char[255];	//ȸ������ ���� ���ڿ� ����
	int query_stat;

	Info *info = new Info;

	PRINT_LINE();
	printf("Join Devsign Vacation study \n\n");

	/*������ũ�⸸ŭ ���� ������ �Է¹޴´�.*/
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
		���̵�, �н�����, �̸�, ����ð��� ���� ���ڿ�, ����Ƚ���� �ѱ��.
		����Ƚ���� ����Ʈ�� 0�̴�.
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

	/* ���̵� ���� �̿��ؼ� Ư�� ���̵��� ��� ������ �����Ѵ�. */
	sprintf(queryBuffer, "SELECT * FROM DEVSIGN WHERE ID='%s'", id);	//���̵����� ������ ã�Ƴ���.
	query_stat = mysql_query(connection, queryBuffer);	//������ ������.
	if (query_stat != 0) {	//���� �������
		fprintf(stderr, "ID select query error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);		//����.
	}
	sql_result = mysql_store_result(connection);	//�ش� ������ ����Ѵ�.

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		lateness = atoi(sql_row[4]);	//���� Ƚ���� �����Ѵ�.

	lateness++;			//���� Ƚ�� ������Ŵ

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

	/*���� 10�� ���� ���Ŀ� �����ߴٸ�,*/
	if (_GetCurrentHour() >= 10 && _GetCurrentMin() >= 0) {
		UpdateLateness(id, connection, conn);
	}
	
	/*����ð����� ���ӽð��� ������Ʈ�Ѵ�.*/
	sprintf(queryBuffer, "UPDATE DEVSIGN SET ACCESS_TIME='%s' WHERE ID = '%s'",
		_GetCurrentTime(), id);		//������ ����
	query_stat = mysql_query(connection, queryBuffer); //���� ������
	if (query_stat != 0) {		//������ ���������� �Էµ��� �ʾҴٸ�.
		fprintf(stderr, "Update Connection Time query error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);			//��������.
	}

	return;
}

/*�α��� ������ Ȯ���ϴ� �Լ�.*/
bool LoginCheck(char *id, char *pw, MYSQL *connection, MYSQL conn) {
	MYSQL_ROW sql_row;
	MYSQL_RES *sql_result;
	int pw_check, id_check;

	char query[250];
	sprintf(query, "SELECT * FROM DEVSIGN WHERE ID='%s'", id);	//���̵����� ������ ã�Ƴ���.

	int query_stat = mysql_query(connection, query);	//�ش� ������ ������.
	if (query_stat != 0) {		//�ش� ���̵� �������� �ʴ´ٸ�
		fprintf(stderr, "Not exist ID \n");
		return false;
	}

	/*���� ����� �����Ѵ�.*/
	sql_result = mysql_store_result(connection);

	/*�ش� ���̵� ���� ���� �н����尡 ��ġ�ϴ��� Ȯ��*/
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		pw_check = strcmp(pw, sql_row[1]);
		id_check = strcmp(id, sql_row[0]);
	}

	if ((pw_check == 0) &&(id_check == 0))	//strcmp()�� ���� ���ڿ��̸� 0�� ��ȯ.
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

/*�Լ� ���ǰ� �ؿ� �־*/
void ShowClientInfo(char *id, MYSQL *connection, MYSQL conn);
void ShowAllClientInfo(MYSQL *connection, MYSQL conn);

void SignInQuery(MYSQL *connection, MYSQL conn) {
	Info info;
	bool check;

	PRINT_LINE();
	/*���̵�� ��й�ȣ�� �Է¹޴´�.*/
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

	/*���ӽð��� ������Ʈ �Ѵ�.*/
	UpdateConnectionTime(info.id, connection, conn);

	/*������ ���� �α���*/
	if (!strcmp(info.id, "admin "))
		ShowAllClientInfo(connection, conn);

	/*�Ϲ� �α����� �Ϸ� �Ǿ���.*/
	else
		ShowClientInfo(info.id, connection, conn);

	return;
}

/*Ư�� ȸ�� �������� ���*/
void ShowClientInfo(char *id, MYSQL *connection, MYSQL conn) {
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;

	int query_stat;
	char queryBuffer[250];
	sprintf(queryBuffer, "SELECT * FROM DEVSIGN WHERE ID = '%s'", id);	// �ش� ���̵� ��� ��� ������.

	query_stat = mysql_query(connection, queryBuffer); 
	if (query_stat != 0) {
		fprintf(stderr, "ShowClientInfo query error : %s \n", mysql_error(&conn));
		exit(EXIT_FAILURE);
	}

	sql_result = mysql_store_result(connection);	//���� ����� ����

	/*�ʵ����� ������ ���*/
	printf("%-10s | %-10s | %-10s | %10s | \n\n", "Name", "ID", "Access_time", "numOfLateness");
	/*�� ���� �����ۿ� ������ �ʴ´�.*/
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%-10s | %-10s | %-10s | %10s | \n\n", sql_row[2], sql_row[0],
			sql_row[3], sql_row[4]);
	}
}

/*��� ȸ���� ������ ���*/
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