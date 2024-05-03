#include "SimpleMySQLLibrary.h"
#include "Blueprint/SimpleMysqlObject.h"
#include "MySQL/mysql.h"

//连接测试
/*
void USimpleMySQLLibrary::MySqlMain()
{
	const char * user = "root";	
	const char * host = "127.0.0.1";
	const char * pawd = "root";
	const char * table = "hello";  //数据库名
	const char * db = "Mysql";
	const uint32 port = 3306;

	MYSQL mysql;
	MYSQL_RES * result = NULL;

	int32 result_int = INDEX_NONE;

	mysql_init(&mysql);  //初始化mysql对象
	if(mysql_real_connect(&mysql,host,user,pawd,table,port,0,0))  //最后一个是客户端标记
	{
		//SELECT * FROM W  //查询W表中的所有字段
		//SELECT name FROM W  //仅查询name字段信息
		if((result_int = mysql_query(&mysql,"SELECT * FROM role")) == 0)
		{
			//缓存64MB
			if((result = mysql_store_result(&mysql)) != NULL)
			{
				//MYSQL_ROW是char**类型，相当于一个字符串数组
				while (MYSQL_ROW sql_row = mysql_fetch_row(result))  //通过遍历可以跳到下一行,每次执返回回一行信息，包含每个类型
				{
					GEngine->AddOnScreenDebugMessage(-1,10,FColor::Blue,*FString::Printf(TEXT("name %s"),ANSI_TO_TCHAR(sql_row[0])));  //name
					GEngine->AddOnScreenDebugMessage(-1,10,FColor::Blue,*FString::Printf(TEXT("age %s"),ANSI_TO_TCHAR(sql_row[1])));  //age
					GEngine->AddOnScreenDebugMessage(-1,10,FColor::Blue,*FString::Printf(TEXT("size %s"),ANSI_TO_TCHAR(sql_row[2])));  //size
					GEngine->AddOnScreenDebugMessage(-1,10,FColor::Blue,*FString::Printf(TEXT("hhhhh %s"),ANSI_TO_TCHAR(sql_row[3])));  //hhhhh
					
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"query sql failed!");
			//打印错误信息
			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"connect failed!");
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
	}

	if(result != NULL)
	{
		mysql_free_result(result);  //释放结果结构体数据
	}

	mysql_close(&mysql);  //关闭数据库对象
}
*/

/*mysql_real_connect
void USimpleMySQLLibrary::MySqlMain()
{
	const char * user = "root";	
	const char * host = "127.0.0.1";
	const char * pawd = "root";
	const char * table = "hello";  //数据库名
	const char * db = "Mysql";
	const uint32 port = 3306;

	MYSQL mysql;
	mysql_library_init(NULL,NULL,NULL);  //保证线程安全
	mysql_init(&mysql);  //初始化mysql对象

	
	connect最后一个参数，客户端标记的可选参数
	 CLIENT_COMPRESS  //让客户端进行数据压缩传送
	 CLIENT_FOUND_ROWS  //返回匹配的函数
	 CLIENT_IGNORE_SPACE  //允许运行函数名可以使用空格
	 CLIENT_INTERACTIVE //对超时重连进行增量
	 CLIENT_LOCAL_FILES //可以进行LOCALData的处理，如批量插入
	 CLIENT_MULTI_STATEMENTS //值可以在单个字符串里输入执行多条语句
	 CLIENT_MULTI_RESULTS //使客户端可以对多个函数结果进行处理，同上一个一起
	 CLIENT_NO_SCHEMA //解决ODBC的缺陷
	 CLIENT_ODBC  //让客户端对ODBC更友好
	 CLIENT_SSL  //openSSL,使用加密协议进行数据传输
	
	
	if(mysql_real_connect(&mysql,host,user,pawd,table,port,0,0))  //最后一个是客户端标记
	{
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"connect success!");
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"connect failed!");
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
	}

	int ret = mysql_ping(&mysql);
	if(ret == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"ping success!");
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"ping failed!");
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
	}
	mysql_close(&mysql);  //关闭数据库对象
	
	mysql_library_end();
}
*/

//mysql_options
// void USimpleMySQLLibrary::MySqlMain()
// {
// 	const char * user = "root";	
// 	const char * host = "127.0.0.1";
// 	const char * pawd = "root";
// 	const char * table = "hello";  //数据库名
// 	const char * db = "Mysql";
// 	const uint32 port = 3306;
//
// 	MYSQL mysql;
// 	mysql_library_init(NULL,NULL,NULL);  //保证线程安全
// 	mysql_init(&mysql);  //初始化mysql对象
//
// 	int time = 5;
// 	int ret = mysql_options(&mysql,MYSQL_OPT_RECONNECT,&time);  //在客户端进行链接前可以进行一些MYsql对像属性的配置
// 	
// 	
// 	if(mysql_real_connect(&mysql,host,user,pawd,table,port,0,0))  //最后一个是客户端标记
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"connect success!");
// 	}else
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"connect failed!");
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 	}
//
// 	ret = mysql_ping(&mysql);
// 	if(ret == 0)
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"ping success!");
// 	}else
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"ping failed!");
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 	}
// 	mysql_close(&mysql);  //关闭数据库对象
// 	
// 	mysql_library_end();
// }

// //mysql_query()
// void USimpleMySQLLibrary::MySqlMain()
// {
// 	const char * user = "root";	
// 	const char * host = "127.0.0.1";
// 	const char * pawd = "root";
// 	const char * table = "hello";  //数据库名
// 	const char * db = "Mysql";
// 	const uint32 port = 3306;
//
// 	MYSQL mysql;
// 	mysql_library_init(NULL,NULL,NULL);  //保证线程安全
// 	mysql_init(&mysql);  //初始化mysql对象
//
// 	my_bool reconnect = 5;
// 	int ret = mysql_options(&mysql,MYSQL_OPT_RECONNECT,&reconnect);  //在客户端进行链接前可以进行一些MYsql对像属性的配置
//
// 	int BufSize = 1024 * 1024 * 64; //1MB * 64 = 64MB
// 	ret = mysql_options(&mysql,MYSQL_OPT_MAX_ALLOWED_PACKET,&BufSize);  //设置客户端网络传输缓冲区
// 	
// 	
// 	if(mysql_real_connect(&mysql,host,user,pawd,table,port,0,0))  //最后一个是客户端标记
// 	{
// 		// if((ret = mysql_query(&mysql,"SELECT name FROM role")) == 0)
// 		// {
// 		// 	//缓存64MB,可以用mysql_options里的
// 		// 	if(MYSQL_RES* result = mysql_store_result(&mysql))
// 		// 	{
// 		// 		//MYSQL_ROW是char**类型，相当于一个字符串数组
// 		// 		while (MYSQL_ROW sql_row = mysql_fetch_row(result))  //通过遍历可以跳到下一行,每次执返回回一行信息，包含每个类型
// 		// 		{
// 		// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Blue,*FString::Printf(TEXT("name [%s]"),ANSI_TO_TCHAR(sql_row[0])));  //name
// 		// 		}
// 		// 	}
// 		// }
//
// 		// if((ret = mysql_query(&mysql,"SELECT * FROM role")) == 0)
// 		// {
// 		// 	//直接从服务器读取，服务器会将数据发送给客户端，适合较小的数据，由于在服务器操作，会消耗服务器资源
// 		// 	if(MYSQL_RES* result = mysql_use_result(&mysql))
// 		// 	{
// 		// 		//mysql_num_fields传入一个查询结构体，并输出查询结果的字段数
// 		// 		//GEngine->AddOnScreenDebugMessage(-1,10,FColor::Green,*FString::Printf(TEXT("fields [%i]"),mysql_num_fields(result)));
// 		// 		//对于查询结果的使用是一样的
// 		// 		
// 		// 		while (MYSQL_ROW sql_row = mysql_fetch_row(result))  //通过遍历可以跳到下一行,每次执返回回一行信息，包含每个类型
// 		// 		{
// 		// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Purple,*FString::Printf(TEXT("name [%s]"),ANSI_TO_TCHAR(sql_row[0])));
// 		// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Purple,*FString::Printf(TEXT("age [%s]"),ANSI_TO_TCHAR(sql_row[1])));
// 		//
// 		// 			//获取当前结果行的长度的数组
// 		// 			unsigned long *len = mysql_fetch_lengths(result);
// 		// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Blue,*FString::Printf(TEXT("name_length [%i]"),*len));
// 		// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Blue,*FString::Printf(TEXT("age_length [%i]"),*(len+1)));
// 		// 		}
// 		// 	
// 		// 	}
//
// 		// if((ret = mysql_query(&mysql,"SELECT * FROM role")) == 0)  //必须经过这一句
// 		// {
// 		// 	//也是获取字段数量，和mysql_num_fields一样，结果也一样，但是他接受的是mysql对像，必须经过查询语句才能获取。
// 		// 	GEngine->AddOnScreenDebugMessage(-1,10,FColor::Green,*FString::Printf(TEXT("fields [%i]"),mysql_field_count(&mysql)));
// 		//
// 		// }
//
//
// 		//测试获取字段信息
// 		if((ret = mysql_query(&mysql,"SELECT * FROM role")) == 0)  //必须经过这一句
// 		{
// 			if(MYSQL_RES* result = mysql_store_result(&mysql)) //获取数据是必须的
// 			{
// 				int num = mysql_num_fields(result); //字段数量
// 				
// 				//测试mysql_fetch_field_direct，获取字段信息方法一
// 				 while (MYSQL_ROW sql_row = mysql_fetch_row(result))  //通过遍历可以跳到下一行,每次执返回回一行信息，包含每个类型
// 				 {
// 				 	//GEngine->AddOnScreenDebugMessage(-1,50,FColor::Green,*FString::Printf(TEXT("name [%s]"),ANSI_TO_TCHAR(sql_row[0])));  //name
// 				
// 				 	for (int i = 0;i < num;i++)
// 				     {
// 				 	//MYSQL_FIELD信息
// 				 	// char *name;                 名称(别名)
// 				 	// char *org_name;             源名称
// 				 	// char *table;                表名
// 				 	// char *org_table;            源表名
// 				 	// char *db;                   数据库名
// 				 	// char *catalog;	           目录
// 				 	// char *def;                   默认值
// 				 	// unsigned long length;        字段长度
// 				 	// unsigned long max_length;     字段最大长度
// 				 	// unsigned int name_length;       名字长度
// 				 	// unsigned int org_name_length;    源名字长度
// 				 	// unsigned int table_length;       表大小
// 				 	// unsigned int org_table_length;   源表大小
// 				 	// unsigned int db_length;            库大小
// 				 	// unsigned int catalog_length;       目录大小
// 				 	// unsigned int def_length;            默认大小
// 				 	// unsigned int flags;         /* Div flags */
// 				 	// unsigned int decimals;      /* Number of decimals in field */
// 				 	// unsigned int charsetnr;     /* Character set */
// 				 	// enum enum_field_types type; /* Type of field. See mysql_com.h for types */  字段类型
// 				 	// void *extension;
// 				 	//获取当前result行的第i个字段的具体信息
// 				 	MYSQL_FIELD *find_sql = mysql_fetch_field_direct(result,i);
// 				 	GEngine->AddOnScreenDebugMessage(-1,50,FColor::Cyan,*FString::Printf(TEXT("field_name [%s]"),ANSI_TO_TCHAR(find_sql->name)));
// 				 	GEngine->AddOnScreenDebugMessage(-1,50,FColor::Cyan,*FString::Printf(TEXT("field_org_name [%s]"),ANSI_TO_TCHAR(find_sql->org_name)));
// 				 	GEngine->AddOnScreenDebugMessage(-1,50,FColor::Red,*FString::Printf(TEXT("-------")));
// 				 	}
// 				 }
// 				
// 				//测试mysql_fetch_field，获取字段信息方法二
// 				//和上着的区别是，它不是检测当前result行的字段，
// 					MYSQL_FIELD *find_sql_org = mysql_fetch_field(result);
// 		
// 					for(int i =0;i < num;i++)
// 					{
// 						MYSQL_FIELD *find_sql = &find_sql_org[i];
// 						GEngine->AddOnScreenDebugMessage(-1,50,FColor::Cyan,*FString::Printf(TEXT("field_name [%s]"),ANSI_TO_TCHAR(find_sql->name)));
// 						GEngine->AddOnScreenDebugMessage(-1,50,FColor::Cyan,*FString::Printf(TEXT("field_org_name [%s]"),ANSI_TO_TCHAR(find_sql->org_name)));
// 						GEngine->AddOnScreenDebugMessage(-1,50,FColor::Cyan,*FString::Printf(TEXT("field_table [%s]"),ANSI_TO_TCHAR(find_sql->table)));
// 						GEngine->AddOnScreenDebugMessage(-1,50,FColor::Cyan,*FString::Printf(TEXT("field_org_table [%s]"),ANSI_TO_TCHAR(find_sql->org_table)));
// 						GEngine->AddOnScreenDebugMessage(-1,50,FColor::Cyan,*FString::Printf(TEXT("field_db [%s]"),ANSI_TO_TCHAR(find_sql->db)));
// 						GEngine->AddOnScreenDebugMessage(-1,50,FColor::Red,*FString::Printf(TEXT("-------")));
// 						
// 					}
// 				
// 			}
// 		}
// 	}
// 	else
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"connect failed!");
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 		}
//
// 	ret = mysql_ping(&mysql);
// 	if(ret == 0)
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"ping success!");
// 	}else
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"ping failed!");
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 	}
//
// 	
// 			mysql_close(&mysql);  //关闭数据库对象
// 	
// 			mysql_library_end();
// }
//
// void USimpleMySQLLibrary::MySqlMain()
// {
// 	
// 	const char * user = "root";	
// 	const char * host = "127.0.0.1";
// 	const char * pawd = "root";
// 	const char * table = "hello";  //数据库名
// 	const uint32 port = 3306;
//
// 	MYSQL mysql;
//
// 	//MYSQL_STMT *mysql_stmt_init(MYSQL *mysql)  创建MYSQL_STMT结构体，用于和MySql对象绑定，用于提交SQL函数。
// 	//mysql_stmt_close(MYSQL_STMT*)  释放MYSQL_STMT
// 	
// 	//int mysql_stmt_prepare(MYSQL_STMT* stmt,const char *query,unsigned long length)  准备预处理语句
// 	
// 	//my_bool mysql_stmt_bind_param(MYSQL_STMT *stmt,MYSQL_BING *bind) 绑定参数，用于往SQL语句绑定参数，
// 	
// 	//my_bool mysql_stmt_bind_result(MYSQL_STMT *stmt,MySQL_BIND *bind) 绑定结果
// 	
// 	//int mysql_stmt_execute(MYSQL_STMT *stmt)  //执行
// 	
// 	//int mysql_stmt_store_result(MYSQL_STMT *stmt)  //缓冲查询结果
// 	
// 	//int mysql_stmt_fetch(MYSQL_STMT *stmt)   //返回结果集的下一行
//
// 	
// 	mysql_library_init(NULL,NULL,NULL);  //保证线程安全
// 	mysql_init(&mysql);  //初始化mysql对象
//
// 	if(mysql_real_connect(&mysql,host,user,pawd,table,port,0,0))  //使用预处理发送SQL前必须链接数据库
// 	{
// 		MYSQL_STMT *Ptr = mysql_stmt_init(&mysql);
// 		char SQL[] = "INSERT INTO ppp1 VALUES(?,?)";
// 		int ret = mysql_stmt_prepare(Ptr,SQL,FString(ANSI_TO_TCHAR(SQL)).Len());
// 		if(ret != 0)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_stmt_error(Ptr));
// 			return;
// 		}
// 		
// 		int32 id = 100;
// 		MYSQL_BIND Pram[2];
// 		FMemory::Memset(Pram,0,sizeof(Pram));  //传参前必须清空内存
// 	
// 		Pram[0].buffer_type = MYSQL_TYPE_LONG;  //指定类型
// 		Pram[0].buffer = &id;  //buf内容
// 		//Pram[0].buffer_length = sizeof(id);   //缓冲存储区大小为变量字节，int等数字类型不需要指明，因为它不是可变的类型，会自动推导。
// 		Pram[0].length = 0;  //如果只是int类型可以不指定字符字节大小。
//
// 		char content[] = "Hello";
// 		Pram[1].buffer_type = MYSQL_TYPE_VARCHAR;
// 		Pram[1].buffer = content;
// 		uint32 contentlen = strlen(content);  
// 		Pram[1].buffer_length = contentlen;  //指定存储缓冲区大小
// 		Pram[1].length = (unsigned long*)&contentlen;  //字符串的字符串字节大小，和缓冲区大小应该是一样的。 //长度必须指明，否则会报客户端缓冲区不足，需要更大空间的问题。
// 		
// 		
// 		ret = mysql_stmt_bind_param(Ptr,Pram);
// 		if(ret != 0)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_stmt_error(Ptr));
// 			return;
// 		}
//
// 		ret = mysql_stmt_execute(Ptr);
// 		if(ret != 0)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_stmt_error(Ptr));
// 			return;
// 		}
//
// 		mysql_stmt_close(Ptr);
// 	}	
// 	else
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,TEXT("Connecting failed"));	
// 	}
//
//
// 	mysql_close(&mysql);  //关闭数据库对象
// 	mysql_library_end();
// 	
// 	mysql_close(&mysql);
// }
//


//事务操作
// void USimpleMySQLLibrary::MySqlMain()
// {
// 	const char * user = "root";	
// 	const char * host = "127.0.0.1";
// 	const char * pawd = "root";
// 	const char * table = "hello";  //数据库名
// 	const uint32 port = 3306;
//
// 	MYSQL mysql;
// 	
// 	mysql_library_init(NULL,NULL,NULL); 
// 	mysql_init(&mysql);  
//
// 	if(mysql_real_connect(&mysql,host,user,pawd,table,port,0,CLIENT_MULTI_STATEMENTS))  
// 	{
// 		
// 		char SQL[] = "START TRANSACTION;";  //开启事务
// 		int ret = mysql_query(&mysql,SQL);
// 		if (ret != 0)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 		}
// 		char SQL1[] = "set autocommit = 0;"; //手动提交
// 		ret = mysql_query(&mysql,SQL1);
// 		if (ret != 0)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 		}
//
// 		char SelectSQL[] = "insert into ppp1 (id,name) values('545','asaas');";  //准备SQL语句
// 		ret = mysql_query(&mysql,SelectSQL);
// 		if(ret == 0)
// 		{
// 			if(MYSQL_RES* result = mysql_store_result(&mysql)) 
// 			{
// 				int num = mysql_num_fields(result); 
// 				 while (MYSQL_ROW sql_row = mysql_fetch_row(result))  
// 				 {
// 				 	 FString RowString;
// 					 for (int i = 0; i < num; ++i)
// 					 {
// 					 	if(sql_row[i] != NULL)
// 					 	{
// 					 		int32 len =(FString(ANSI_TO_TCHAR(sql_row[i])).Len()) / 2;
// 							RowString+="|";
// 					 		int32 Add_len = 20 - len;
// 					 		for (int j = 0;j < Add_len ;j++)
// 					 		{
// 					 			RowString+=" ";
// 					 		}
//
// 					 		RowString+=ANSI_TO_TCHAR(sql_row[i]);
// 					 			
// 					 		for (int j = 0;j < Add_len ;j++)
// 					 		{
// 					 			RowString+=" ";
// 					 		}
// 					 	
// 					 	}
// 					    else
// 					    {
// 					    		int32 len =FString("NULL").Len() / 2;
// 					    		RowString+="|";
// 					    		int32 Add_len = 20 - len;
// 					    		for (int j = 0;j < Add_len ;j++)
// 					    		{
// 					    			RowString+=" ";
// 					    		}
//
// 					    		RowString+="NULL";
// 					 			
// 					    		for (int j = 0;j < Add_len ;j++)
// 					    		{
// 					    			RowString+=" ";
// 					    		}
// 					    	
// 					    }
// 					 }
// 				 	RowString+="|";
// 				 	GEngine->AddOnScreenDebugMessage(-1,100,FColor::Cyan,*RowString);
// 				 }
// 			}
// 			else
// 			{
// 				GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"Store failed!");
// 				GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 			}
// 		}
// 		else
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"Query failed!");
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 		}
//
// 		
// 		char SQL2[] = "ROLLBACK;"; //回滚，撤销所有存储SQL语句
// 		ret = mysql_query(&mysql,SQL2);
// 		if (ret != 0)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 		}
// 		
// 		
// 		char SQL3[] = "insert into ppp1 (id,name) values('56','shared');";  //准备SQL语句，放在内存
// 		ret = mysql_query(&mysql,SQL3);
// 		if(ret == 0)
// 		{
// 			if(MYSQL_RES* result = mysql_store_result(&mysql)) 
// 			{
// 				int num = mysql_num_fields(result); 
// 				while (MYSQL_ROW sql_row = mysql_fetch_row(result))  
// 				{
// 					FString RowString;
// 					for (int i = 0; i < num; ++i)
// 					{
// 						if(sql_row[i] != NULL)
// 						{
// 							int32 len =(FString(ANSI_TO_TCHAR(sql_row[i])).Len()) / 2;
// 							RowString+="|";
// 							int32 Add_len = 20 - len;
// 							for (int j = 0;j < Add_len ;j++)
// 							{
// 								RowString+=" ";
// 							}
// 		
// 							RowString+=ANSI_TO_TCHAR(sql_row[i]);
// 					 			
// 							for (int j = 0;j < Add_len ;j++)
// 							{
// 								RowString+=" ";
// 							}
// 					 	
// 						}
// 						else
// 						{
// 							int32 len =FString("NULL").Len() / 2;
// 							RowString+="|";
// 							int32 Add_len = 20 - len;
// 							for (int j = 0;j < Add_len ;j++)
// 							{
// 								RowString+=" ";
// 							}
// 		
// 							RowString+="NULL";
// 					 			
// 							for (int j = 0;j < Add_len ;j++)
// 							{
// 								RowString+=" ";
// 							}
// 					    	
// 						}
// 					}
// 					RowString+="|";
// 					GEngine->AddOnScreenDebugMessage(-1,100,FColor::Cyan,*RowString);
// 				}
// 			}
// 			else
// 			{
// 				GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"Store failed!");
// 				GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 			}
// 		}
// 		else
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"Query failed!");
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 		}
//
// 		char SQL4[] = "COMMIT;"; //提交所有内存的SQL语句
// 		ret = mysql_query(&mysql,SQL4);
// 		if (ret != 0)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 		}
// 		
// 		char SQL5[] = "set autocommit = 1;"; //设置自动提交
// 		ret = mysql_query(&mysql,SQL5);
// 		if (ret != 0)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 		}
// 		
// 	}
// 	else
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"connect failed!");
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 	}
// 	
// 			mysql_close(&mysql);  //关闭数据库对象
// 	
// 			mysql_library_end();
// }

//模板
// void USimpleMySQLLibrary::MySqlMain()
// {
// 	const char * user = "root";	
// 	const char * host = "127.0.0.1";
// 	const char * pawd = "root";
// 	const char * table = "hello";  //数据库名
// 	const uint32 port = 3306;
//
// 	MYSQL mysql;
// 	
// 	mysql_library_init(NULL,NULL,NULL); 
// 	mysql_init(&mysql);  
//
// 	if(mysql_real_connect(&mysql,host,user,pawd,table,port,0,CLIENT_MULTI_STATEMENTS))
// 	{
// 		char SelectSQL[] = "insert into ppp1 (id,name) values('545','asaas');";  //准备SQL语句
// 		int ret = mysql_query(&mysql,SelectSQL);
// 		if(ret == 0)
// 		{
// 			if(MYSQL_RES* result = mysql_store_result(&mysql)) 
// 			{
// 				int num = mysql_num_fields(result); 
// 				while (MYSQL_ROW sql_row = mysql_fetch_row(result))  
// 				{
// 					FString RowString;
// 					for (int i = 0; i < num; ++i)
// 					{
// 						if(sql_row[i] != NULL)
// 						{
// 							int32 len =(FString(ANSI_TO_TCHAR(sql_row[i])).Len()) / 2;
// 							RowString+="|";
// 							int32 Add_len = 20 - len;
// 							for (int j = 0;j < Add_len ;j++)
// 							{
// 								RowString+=" ";
// 							}
//
// 							RowString+=ANSI_TO_TCHAR(sql_row[i]);
// 							
// 							for (int j = 0;j < Add_len ;j++)
// 							{
// 								RowString+=" ";
// 							}
// 					
// 						}
// 						else
// 						{
// 							int32 len =FString("NULL").Len() / 2;
// 							RowString+="|";
// 							int32 Add_len = 20 - len;
// 							for (int j = 0;j < Add_len ;j++)
// 							{
// 								RowString+=" ";
// 							}
//
// 							RowString+="NULL";
// 							
// 							for (int j = 0;j < Add_len ;j++)
// 							{
// 								RowString+=" ";
// 							}
// 					    
// 						}
// 					}
// 					RowString+="|";
// 					GEngine->AddOnScreenDebugMessage(-1,100,FColor::Cyan,*RowString);
// 				}
// 			}
// 			else
// 			{
// 				GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"Store failed!");
// 				GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 			}
// 		}
// 		else
// 		{
// 		    GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"Query failed!");
// 			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 		}
// 	}
// 	else
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"connect failed!");
// 		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
// 	}
// 	mysql_close(&mysql);  //关闭数据库对象
// 	mysql_library_end();
// }

void USimpleMySQLLibrary::MySqlMain()
{
		const char * user = "root";	
		const char * host = "127.0.0.1";
		const char * pawd = "root";
		const char * table = "hello";  //数据库名
		const uint32 port = 3306;
	
		MYSQL mysql;
		
		mysql_library_init(NULL,NULL,NULL); 
		mysql_init(&mysql);  
	
		if(mysql_real_connect(&mysql,host,user,pawd,table,port,0,CLIENT_MULTI_STATEMENTS))
		{
			char CreateTmpTableSQL[] = "SELECT USER()";
			int ret = mysql_query(&mysql,CreateTmpTableSQL);
			if(ret != 0)
			{
				GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
			}
	
			// char insertSQL[] = "insert into tb select * from ppp1 where id < '6'";;
			// ret = mysql_query(&mysql,insertSQL);
			// if(ret != 0)
			// {
			// 	GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
			// }
			//
			// char InsertSQL[] = "insert into TmpTB (name,age,sorce) values('steve','12','100');";
			// ret = mysql_query(&mysql,InsertSQL);
			// if(ret != 0)
			// {
			// 	GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
			// }
	
			char QuerySQL[] = "select * from tb LIMIT 0,10000;";
			ret = mysql_query(&mysql,QuerySQL);
			if(ret == 0)
			{
				if(MYSQL_RES* result = mysql_store_result(&mysql)) 
				{
					int num = mysql_num_fields(result); 
					while (MYSQL_ROW sql_row = mysql_fetch_row(result))  
					{
						FString RowString;
						for (int i = 0; i < num; ++i)
						{
							if(sql_row[i] != NULL)
							{
								int32 len =(FString(ANSI_TO_TCHAR(sql_row[i])).Len()) / 2;
								RowString+="|";
								int32 Add_len = 20 - len;
								for (int j = 0;j < Add_len ;j++)
								{
									RowString+=" ";
								}
	
								RowString+=ANSI_TO_TCHAR(sql_row[i]);
								
								for (int j = 0;j < Add_len ;j++)
								{
									RowString+=" ";
								}
						
							}
							else
							{
								int32 len =FString("NULL").Len() / 2;
								RowString+="|";
								int32 Add_len = 20 - len;
								for (int j = 0;j < Add_len ;j++)
								{
									RowString+=" ";
								}
	
								RowString+="NULL";
								
								for (int j = 0;j < Add_len ;j++)
								{
									RowString+=" ";
								}
						    
							}
						}
						RowString+="|";
						GEngine->AddOnScreenDebugMessage(-1,100,FColor::Cyan,*RowString);
					}
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"Store failed!");
					GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
				}
			}
			else
			{
			    GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"Query failed!");
				GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,"connect failed!");
			GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,mysql_error(&mysql));
		}
		mysql_close(&mysql);  //关闭数据库对象
		mysql_library_end();
	
}

USimpleMysqlObject* USimpleMySQLLibrary::CreateMysqlObject(UObject* WorldContextObject, const FString& InUser,const FString& InHost, const FString& InPawd, const FString& InDB, const int32 InPort,const TArray<ESimpleClientFlags>& InClientFlag ,const FString& InUnix_Socket)
{
	if(UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))
	{
		if(USimpleMysqlObject *Object = NewObject<USimpleMysqlObject>(World))
		{
			Object->InitMysql(InUser,InHost,InPawd,InDB,InPort,InUnix_Socket,InClientFlag);

			return Object;
		}
	}

	return nullptr;
}

bool USimpleMySQLLibrary::QueryLink(USimpleMysqlObject* Object, const FString& SQL, FString& ErrMesg)
{
	if(Object)
	{
		return Object->GetLink()->QueryLink(SQL,ErrMesg);
	}

		return false;
}



bool USimpleMySQLLibrary::SelectNewDB(USimpleMysqlObject* Object, const FString& NewDB, FString& ErrMesg)
{
	if(Object)
	{
		return Object->GetLink()->SelectNewDB(NewDB,ErrMesg);
	}

	return false;
}


bool USimpleMySQLLibrary::CreateDataBase(USimpleMysqlObject* Object, const FString& DataBaseName,EMysqlCharset Charset, FString& ErrorMeg)
{
	if(Object)
	{
		return Object->GetLink()->CreateDateBase(DataBaseName,Charset,ErrorMeg);
	}
	
	return false;
}

bool USimpleMySQLLibrary::CreateAndSelectDataBase(USimpleMysqlObject* Object, const FString& DataBaseName,EMysqlCharset Charset, FString& ErrorMeg)
{
	if(Object)
	{
		return Object->GetLink()->CreateAndSelectDataBase(DataBaseName,Charset,ErrorMeg);
	}
	
	return false;
}

bool USimpleMySQLLibrary::CreateTable(USimpleMysqlObject* Object, const FString& TableName,const TMap<FString, FMysqlFileType>& InFields, const TArray<FString>& InPrimaryKeys,const FMysqlCreateTableParam& Param, FString& ErrorMeg)
{
	if(Object)
	{
		return Object->GetLink()->CreateTable(TableName,InFields,InPrimaryKeys,Param,ErrorMeg);
	}
	
	return false;
}



bool USimpleMySQLLibrary::DropDataBase(USimpleMysqlObject* Object, const FString& DataBaseName, FString& ErrorMeg)
{
	if(Object)
	{
		return Object->GetLink()->DropDataBase(DataBaseName,ErrorMeg);
	}
	
	return false;
	
}

bool USimpleMySQLLibrary::DropTable(USimpleMysqlObject* Object, const FString& TableName, FString& ErrorMsg)
{
	if(Object)
	{
		return Object->GetLink()->DropTable(TableName,ErrorMsg);
	}
	return false;
}

bool USimpleMySQLLibrary::TruncateTable(USimpleMysqlObject* Object, const FString& TableName, FString& ErrorMsg)
{
	if(Object)
	{
		return Object->GetLink()->TruncateTable(TableName,ErrorMsg);
	}
	return false;
}

bool USimpleMySQLLibrary::DeleteFromTable(USimpleMysqlObject* Object, const FString& TableName, FString& ErrorMsg)
{
	if(Object)
	{
		return Object->GetLink()->DeleteFromTable(TableName,ErrorMsg);
	}
	return false;
}

bool USimpleMySQLLibrary::DeleteFromTableWhereData(USimpleMysqlObject* Object, const FString& TableName,TArray<FSimpleMysqlComparisonOperator>& Condition, FString& ErrorMsg)
{
	if(Object)
	{
		return Object->GetLink()->DeleteFromTableWhereData(TableName, Condition,ErrorMsg);
	}
	return false;
}

bool USimpleMySQLLibrary::OptimiseTable(USimpleMysqlObject* Object, const FString& TableName, FString& ErrorMsg)
{
	if(Object)
	{
		return Object->GetLink()->OptimiseTable(TableName,ErrorMsg);
	}
	return false;
}

bool USimpleMySQLLibrary::PrintResult(USimpleMysqlObject* Object,const TArray<FSimpleMysqlResult>& Results,bool bPrinttToScreen, bool bPrintToLog)
{
	if(Object)
	{
		return Object->GetLink()->PrintResult(Results,bPrinttToScreen,bPrintToLog);
	}
	return false;
}

bool USimpleMySQLLibrary::UpdataTableData(USimpleMysqlObject* Object,const FString& TableName, const TArray<FSimpleMysqlAssignment>& InFields,
	const TArray<FSimpleMysqlComparisonOperator>& Condition, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->UpdataTableData(TableName,InFields,Condition,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::ReplaceTableData(USimpleMysqlObject* Object, const FString& TableName,
	const TArray<FSimpleMysqlReplaceParamters>& InReplaces, const TArray<FSimpleMysqlComparisonOperator>& Condition,
	FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->ReplaceTableData(TableName,InReplaces,Condition,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::ShowTables(USimpleMysqlObject* Object, const FString& TableName,EMysqlQuerySaveType SaveType,
	TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, const FSimpleMysqlDebugResult& Debug)
{
	if(Object)
	{
		return Object->GetLink()->ShowTables(TableName,SaveType,Results,ErrorMesg,Debug);
	}
	return false;
}

bool USimpleMySQLLibrary::InsertTableData(USimpleMysqlObject* Object,const FString& TableNmae, const TMap<FString, FString>& InsertKeyValueData,const FSimpleMysqlQueryParameters &QueryParam,FString& ErrorMesg,bool bIgnore)
{
	if(Object)
	{
		return Object->GetLink()->InsertTableData(TableNmae,InsertKeyValueData,QueryParam,ErrorMesg,bIgnore);
	}
	return false;
}

bool USimpleMySQLLibrary::SimpleInsertTableData(USimpleMysqlObject* Object,const FString& TableNmae, const TArray<FString>& InsertValueData,FString& ErrorMesg,bool bIgnore)
{
	if(Object)
	{
		return Object->GetLink()->SimpleInsertTableData(TableNmae,InsertValueData,ErrorMesg,bIgnore);
	}
	return false;
}

bool USimpleMySQLLibrary::ReplaceIfExistence(USimpleMysqlObject* Object, const FString& TableNmae,
	const TMap<FString, FString>& InsertKeyValueData, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->ReplaceIfExistence(TableNmae,InsertKeyValueData,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::GetSelectVersion(USimpleMysqlObject* Object, TArray<FSimpleMysqlResult>& Results,
                                           FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if(Object)
	{
		return Object->GetLink()->GetSelectVersion(Results,ErrorMesg,SaveType,Debug);
	}
	return false;
}

bool USimpleMySQLLibrary::GetSelectDatabase(USimpleMysqlObject* Object, TArray<FSimpleMysqlResult>& Results,
	FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if(Object)
	{
		return Object->GetLink()->GetSelectDatabase(Results,ErrorMesg,SaveType,Debug);
	}
	return false;
}

bool USimpleMySQLLibrary::GetSelectUser(USimpleMysqlObject* Object, TArray<FSimpleMysqlResult>& Results,
	FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if(Object)
	{
		return Object->GetLink()->GetSelectUser(Results,ErrorMesg,SaveType,Debug);
	}
	return false;
}

bool USimpleMySQLLibrary::GetShowStatus(USimpleMysqlObject* Object, TArray<FSimpleMysqlResult>& Results,
	FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if(Object)
	{
		return Object->GetLink()->GetShowStatus(Results,ErrorMesg,SaveType,Debug);
	}
	return false;
}

bool USimpleMySQLLibrary::GetShowVariables(USimpleMysqlObject* Object, TArray<FSimpleMysqlResult>& Results,
	FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if(Object)
	{
		return Object->GetLink()->GetShowVariables(Results,ErrorMesg,SaveType,Debug);
	}
	return false;
}

bool USimpleMySQLLibrary::CreateFunction(USimpleMysqlObject *Object, const FString &InFunctionName, const TMap<FString, FMysqlFileType> &InParameter, const FString &InFunctionBody, FString& ErrMesg, EMysqlVariableType InReturnsType /*= EMysqlVariableType::INT*/, const FString &InComment /*= TEXT("")*/)
{
	if (Object)
	{
		return Object->GetLink()->CreateFunction(InFunctionName, InParameter, InFunctionBody, ErrMesg, InReturnsType, InComment);
	}

	return false;
}

bool USimpleMySQLLibrary::ExecuteFunction(USimpleMysqlObject *Object, const FString &InFunctionName, const TArray<FString> &InParameter, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if (Object)
	{
		return Object->GetLink()->ExecuteFunction(InFunctionName, InParameter, Results, ErrMesg, SaveType, Debug);
	}

	return false;
}

bool USimpleMySQLLibrary::DropFunction(USimpleMysqlObject *Object, const FString &InFunctionName, FString& ErrMesg)
{
	if (Object)
	{
		return Object->GetLink()->DropFunction(InFunctionName, ErrMesg);
	}

	return false;
}

bool USimpleMySQLLibrary::ShowCreateFunction(USimpleMysqlObject *Object, const FString &InFunctionName, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if (Object)
	{
		return Object->GetLink()->ShowCreateFunction(InFunctionName, Results, ErrMesg, SaveType, Debug);
	}

	return false;
}

bool USimpleMySQLLibrary::ShowFunctionsStatus(USimpleMysqlObject *Object, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if (Object)
	{
		return Object->GetLink()->ShowFunctionsStatus(Results, ErrMesg, SaveType, Debug);
	}

	return false;
}

bool USimpleMySQLLibrary::DefiningVariables(USimpleMysqlObject *Object, const FString &InVariablesName, const FString &InValue, FString &ErrMesg)
{
	if (Object)
	{
		return Object->GetLink()->DefiningVariables(InVariablesName, InValue, ErrMesg);
	}

	return false;
}

bool USimpleMySQLLibrary::CreateProcedure(USimpleMysqlObject *Object, const FString &InProcedureName, const TMap<FString, FMysqlFileType> &InParameter, const FString &InProcedureBody, FString& ErrMesg, const FString &InComment /*= TEXT("")*/)
{
	if (Object)
	{
		return Object->GetLink()->CreateProcedure(InProcedureName, InParameter, InProcedureBody, ErrMesg, InComment);
	}

	return false;
}

bool USimpleMySQLLibrary::ExecuteProcedure(USimpleMysqlObject *Object, const FString &InProcedureName, const TArray<FString> &InParameter, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if (Object)
	{
		return Object->GetLink()->ExecuteProcedure(InProcedureName, InParameter, Results, ErrMesg, SaveType, Debug);
	}

	return false;
}

bool USimpleMySQLLibrary::DropProcedure(USimpleMysqlObject *Object, const FString &InProcedureName, FString& ErrMesg)
{
	if (Object)
	{
		return Object->GetLink()->DropProcedure(InProcedureName, ErrMesg);
	}

	return false;
}

bool USimpleMySQLLibrary::ShowCreateProcedure(USimpleMysqlObject *Object, const FString &InProcedureName, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if (Object)
	{
		return Object->GetLink()->ShowCreateProcedure(InProcedureName, Results, ErrMesg, SaveType, Debug);
	}

	return false;
}

bool USimpleMySQLLibrary::ShowProceduresStatus(USimpleMysqlObject *Object, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if (Object)
	{
		return Object->GetLink()->ShowProceduresStatus(Results, ErrMesg, SaveType, Debug);
	}

	return false;
}

bool USimpleMySQLLibrary::StartTransaction(USimpleMysqlObject* Object, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->StartTransaction(ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::SetAutocommit(USimpleMysqlObject* Object, bool bAuto, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->SetAutocommit(bAuto,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::Commit(USimpleMysqlObject* Object, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->Commit(ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::SetSavePointName(USimpleMysqlObject* Object, const FString& SaveName, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->SetSavePointName(SaveName,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::Rollback(USimpleMysqlObject* Object, const FString& SaveName, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->Rollback(SaveName,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::ReleaseSavepoint(USimpleMysqlObject* Object, const FString& SaveName, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->ReleaseSavepoint(SaveName,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::CreateTemporaryTable(USimpleMysqlObject* Object, const FString& NewTableName,
	const TMap<FString, FMysqlFileType>& InFields, const TArray<FString>& InPrimaryKeys,
	const FMysqlCreateTableParam& Param, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->CreateTemporaryTable(NewTableName,InFields,InPrimaryKeys,Param,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::CreateTemporaryTableFromOther(USimpleMysqlObject* Object, const FString& NewTableName,const TMap<FString, FMysqlFileType>& InFields, const TArray<FString>& InPrimaryKeys,
	const FSimpleMysqlQueryParameters& QueryParam, TArray<FSimpleMysqlComparisonOperator>& Condition, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->CreateTemporaryTableFromOther(NewTableName,InFields,InPrimaryKeys,QueryParam,Condition,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::DropTemporaryTable(USimpleMysqlObject* Object,const FString& TemporaryTable, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->DropTemporaryTable(TemporaryTable,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::CreateTemporaryTableAsOther(USimpleMysqlObject* Object,const FString& TemporaryTable,
	const TMap<FString, FMysqlFileType>& NewInFields,const TArray<FString>& InPrimaryKeys, const FSimpleMysqlQueryParameters& QueryParam, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->CreateTemporaryTableAsOther(TemporaryTable,NewInFields,InPrimaryKeys,QueryParam,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::CopyTableAToBFast(USimpleMysqlObject* Object, const FString& CopyTargetTable,
	const TMap<FString, FMysqlFileType>& InsertFields, const TArray<FString>& InPrimaryKeys,
	const FSimpleMysqlQueryParameters& QueryParam, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->CopyTableAToBFast(CopyTargetTable,InsertFields,InPrimaryKeys,QueryParam,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::CopyTableAToTemporaryBFast(USimpleMysqlObject* Object, const FString& CopyTargetTable,
	const TMap<FString, FMysqlFileType>& InsertFields, const TArray<FString>& InPrimaryKeys,
	const FSimpleMysqlQueryParameters& QueryParam, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->CopyTableAToTemporaryBFast(CopyTargetTable,InsertFields,InPrimaryKeys,QueryParam,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::CopyTableAToB(USimpleMysqlObject* Object, const FString& A, const FString& B,const TArray<FString>& Fields, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->CopyTableAToB(A,B,Fields,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::CopyTableAToTemporaryB(USimpleMysqlObject* Object, const FString& Table,
	const FString& Temporary, const TArray<FString>& Fields, FString& ErrorMesg)
{
	if(Object)
	{
		return Object->GetLink()->CopyTableAToTemporaryB(Table,Temporary,Fields,ErrorMesg);
	}
	return false;
}

bool USimpleMySQLLibrary::GetSelectTableDataSQL(USimpleMysqlObject* Object, FString& SQL, const FSimpleMysqlQueryParameters& QueryParam)
{
	if(Object)
	{
		return Object->GetLink()->GetSelectTableDataSQL(SQL,QueryParam);
	}
	return false;
}

bool USimpleMySQLLibrary::GetSelectTableData(USimpleMysqlObject* Object, EMysqlQuerySaveType SaveType,const FSimpleMysqlQueryParameters& QueryParam,
	TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, const FSimpleMysqlDebugResult& Debug)
{
	if(Object)
    	{
    		return Object->GetLink()->GetSelectTableData(SaveType,QueryParam,Results,ErrorMesg,Debug);
    	}
    	return false;
}


TArray<FString> USimpleMySQLLibrary::GetStringNULLArray()
{
	return TArray<FString>();
}

TMap<FString, FString> USimpleMySQLLibrary::GetTMPStringNULLArray()
{
	return TMap<FString, FString>();
}

TArray<FSimpleMysqlQueryOrderBy> USimpleMySQLLibrary::GetOrderByNULLArray()
{
	return TArray<FSimpleMysqlQueryOrderBy>();
}

FSimpleMysqlJoinParameters USimpleMySQLLibrary::GetJoinParametersNULL()
{
	return FSimpleMysqlJoinParameters();
}

TArray<FSimpleMysqlAssignment> USimpleMySQLLibrary::GetAssignmentNULLArray()
{
	return TArray<FSimpleMysqlAssignment>();
}

TArray<FSimpleMysqlReplaceParamters> USimpleMySQLLibrary::GeReplaceParamtersNULLArray()
{
	return TArray<FSimpleMysqlReplaceParamters>();
}

TMap<FString, FMysqlFileType> USimpleMySQLLibrary::GetInSertFieldsNULL()
{
	return TMap<FString, FMysqlFileType>();
}

TMap<FString, FMysqlFileType> USimpleMySQLLibrary::GetInParameterNULL()
{
	return TMap<FString, FMysqlFileType>();
}

FSimpleMysqlModifyField USimpleMySQLLibrary::GetlModifyFieldNULL()
{
	return FSimpleMysqlModifyField();
}

TArray<FSimpleMysqlComparisonOperator> USimpleMySQLLibrary::GetComparisonOperatorNULLArray()
{
	return TArray<FSimpleMysqlComparisonOperator>();
}

FSimpleMysqlQueryParameters USimpleMySQLLibrary::GetNULLQueryParameters()
{
	return FSimpleMysqlQueryParameters();
}
