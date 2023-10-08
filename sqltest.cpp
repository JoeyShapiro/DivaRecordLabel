#include <stdio.h>
#include <sqlite3.h>
#include <string>

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   printf("\n");
   return 0;
}

int main() {
    printf("hello\n");

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Callback function called";

    // Create a sqlite3 object
    rc = sqlite3_open("DivaRecordLabel.sqlite", &db);

    if ( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
   } else {
        /* Create SQL statement */
        // INSERT INTO scores (title, cool, fine, safe, sad, worst, difficulty, completion, pv_id, total_score, combo, grade)
        // VALUES ('luka', 50, 99, 0, 29, 28, 'hard', 60.7, 5489, 543985, 345, 'great');
        std::string sql = "INSERT INTO scores (pv_id, title, difficulty, total_score, completion, grade, combo, cool, fine, safe, sad, worst)";
        sql += " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        sqlite3_stmt *stmt;
        sqlite3_prepare_v2(
            db,             // the handle to your (opened and ready) database
            sql.c_str(),    // the sql statement, utf-8 encoded
            sql.length(),   // max length of sql statement
            &stmt,          // this is an "out" parameter, the compiled statement goes here
            nullptr
        );

        // bind parameter
        sqlite3_bind_int(stmt, 1, 345);
        sqlite3_bind_text(stmt, 2, "luka", 4, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, "hard", 4, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, 5436);
        sqlite3_bind_double(stmt, 5, 55.6);
        sqlite3_bind_text(stmt, 6, "clear", 5, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 7, 433);
        sqlite3_bind_int(stmt, 8, 333);
        sqlite3_bind_int(stmt, 9, 444);
        sqlite3_bind_int(stmt, 10, 555);
        sqlite3_bind_int(stmt, 11, 111);
        sqlite3_bind_int(stmt, 12, 22);

        // execute the statement
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            printf("SQLite insert error: (%d) %s\n", rc, sqlite3_errmsg(db));
        }

        // close the statement
        rc = sqlite3_finalize(stmt);
        if( rc != SQLITE_OK ) {
            printf("SQLite finalize error: (%d) %s\n", rc, sqlite3_errmsg(db));
        } else {
            printf("Operation done successfully\n");
        }

        const char *sqls = "SELECT * FROM scores";
        sqlite3_exec(db, sqls, callback, (void*)data, &zErrMsg);
   }
   sqlite3_close(db);

    return 0;
}