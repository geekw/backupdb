#include <stdio.h>
#include <sqlite3.h>

/*
** This function is used to load the contents of a database file on disk 
** into the "main" database of open database connection pInMemory, or
** to save the current contents of the database opened by pInMemory into
** a database file on disk. pInMemory is probably an in-memory database, 
** but this function will also work fine if it is not.
**
** Parameter zFilename points to a nul-terminated string containing the
** name of the database file on disk to load from or save to. If parameter
** isSave is non-zero, then the contents of the file zFilename are 
** overwritten with the contents of the database opened by pInMemory. If
** parameter isSave is zero, then the contents of the database opened by
** pInMemory are replaced by data loaded from the file zFilename.
**
** If the operation is successful, SQLITE_OK is returned. Otherwise, if
** an error occurs, an SQLite error code is returned.
*/
int sqlite3_copy(const char *srcDBFileName, const char *destDBFileName){
  int rc;                   /* Function return code */
  sqlite3_backup *pBackup;  /* Backup object used to copy data */
  sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */
  sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */


  /* Open the database files identified by srcDBFileName and destDBFileName. 
  ** Exit early if this fails for any reason. */
  rc = sqlite3_open(srcDBFileName, &pFrom);
  if (rc != SQLITE_OK)
   {
      fprintf(stderr, "Opening source database file %s failed\n", srcDBFileName);
      return(1);
   } 

  rc = sqlite3_open(destDBFileName, &pTo);
  if (rc != SQLITE_OK)
  {
      fprintf(stderr, "Opening destination database file %s failed\n", destDBFileName);
      return(1);  
  }
  if( rc==SQLITE_OK ){

    /* Set up the backup procedure to copy from the "main" database of 
    ** connection pFrom to the main database of connection pTo.
    ** If something goes wrong, pBackup will be set to NULL and an error
    ** code and  message left in connection pTo.
    **
    ** If the backup object is successfully created, call backup_step()
    ** to copy data from pFrom to pTo. Then call backup_finish()
    ** to release resources associated with the pBackup object.  If an
    ** error occurred, then  an error code and message will be left in
    ** connection pTo. If no error occurred, then the error code belonging
    ** to pTo is set to SQLITE_OK.
    */
    pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
    if( pBackup ){
      (void)sqlite3_backup_step(pBackup, -1);
      (void)sqlite3_backup_finish(pBackup);
    }
    rc = sqlite3_errcode(pTo);
  }

  /* Close the database connections opened on database file srcDBFileName
  ** and destDBFileName return the result of this function. */
  (void)sqlite3_close(pFrom);
  (void)sqlite3_close(pTo);
  return rc;
}

int main(int argc, char **argv)
{
	int rc;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s SourceDBFileName DestDBFileName\n", argv[0]);
		return 1;
	}

  rc = sqlite3_copy(argv[1], argv[2]);
  if (rc != SQLITE_OK )
    return(1);

	return 0;
}
