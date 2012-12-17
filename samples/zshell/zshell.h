/*
 * zshell.h
 *
 *  Created on: 24.07.2012
 *      Author: yaroslav
 */

#ifndef ZSHELL_H_
#define ZSHELL_H_

enum { EDbReadOnly, EDbReWritable, EDbWrong };

int run_lua_script (const char* filename);
int run_lua_buffer_script (const char* buffer, size_t buf_size, const char** argv);
int 
run_sql_query_buffer(
		     const char* dbpath, 
		     int open_mode, 
		     const char* sqldump, size_t dump_size );

#endif /* ZSHELL_H_ */
