/*
 * java.io.File.c
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2003
 *      Kaffe's team.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <native.h>
#include <jsyscall.h>
#ifdef HAVE_UTIME_H
#include <utime.h>
#endif
#include "defs.h"
#include "files.h"
#include "../../../include/system.h"
#include "java_io_File.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/stringSupport.h"

/*
 * Is named item a file?
 */
jboolean java_io_File_isFileInternal(struct Hjava_io_File* this, 
				     struct Hjava_lang_String* fileName)
{
  struct stat buf;
  char str[MAXPATHLEN];
  int r;
  
  stringJava2CBuf(fileName, str, sizeof(str));
  
  r = KSTAT(str, &buf);
  if (r == 0 && S_ISREG(buf.st_mode)) {
    return (1);
  }
  else {
    return (0);
  }
}

/*
 * Is named item a directory?
 */
jboolean java_io_File_isDirectoryInternal(struct Hjava_io_File* this,
					  struct Hjava_lang_String* fileName)
{
  struct stat buf;
  char str[MAXPATHLEN];
  int r;

  stringJava2CBuf(fileName, str, sizeof(str));
  
  r = KSTAT(str, &buf);
  if (r == 0 && S_ISDIR(buf.st_mode)) {
    return (1);
  }
  else {
    return (0);
  }
}

/*
 * Does named file exist?
 */
jboolean java_io_File_existsInternal(struct Hjava_io_File* this,
				     struct Hjava_lang_String* fileName)
{
  struct stat buf;
  char str[MAXPATHLEN];
  
  stringJava2CBuf(fileName, str, sizeof(str));
  
  /* A file exists if I can stat it */
  return (KSTAT(str, &buf) == 0);
}

/*
 * Last modified time on file.
 */
jlong java_io_File_lastModifiedInternal(struct Hjava_io_File* this,
					struct Hjava_lang_String* fileName)
{
  struct stat buf;
  char str[MAXPATHLEN];
  int r;
  
  stringJava2CBuf(fileName, str, sizeof(str));
  
  r = KSTAT(str, &buf);
  if (r != 0) {
    return ((jlong)0);
  }
  return ((jlong)buf.st_mtime * (jlong)1000);
}

/*
 * Can I write to this file?
 */
jboolean
java_io_File_canWriteInternal(struct Hjava_io_File* this,
			      struct Hjava_lang_String* fileName)
{
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(fileName, str, sizeof(str));
	/* XXX make part of jsyscall interface !? */
	r = access(str, W_OK);
	return (r < 0 ? 0 : 1);
}

/*
 * Can I read from this file.
 */
jboolean java_io_File_canReadInternal(struct Hjava_io_File* this,
				      struct Hjava_lang_String* fileName)
{
  char str[MAXPATHLEN];
  int r;
  
  stringJava2CBuf(fileName, str, sizeof(str));
  /* XXX make part of jsyscall interface !? */
  r = access(str, R_OK);
  return (r < 0 ? 0 : 1);
}

/*
 * Return length of file.
 */
jlong java_io_File_lengthInternal(struct Hjava_io_File* this,
				  struct Hjava_lang_String* fileName)
{
  struct stat buf;
  char str[MAXPATHLEN];
  int r;
  
  stringJava2CBuf(fileName, str, sizeof(str));
  
  r = KSTAT(str, &buf);
  if (r != 0) {
    return ((jlong)0);
  }
  return ((jlong)buf.st_size);
}

/*
 * Create a directory.
 */
jboolean java_io_File_mkdirInternal(struct Hjava_io_File* this,
				    struct Hjava_lang_String* fileName)
{
  char str[MAXPATHLEN];
  int r;
  
  stringJava2CBuf(fileName, str, sizeof(str));
  r = KMKDIR(str, 0777);
  return (r != 0 ? 0 : 1);
}

/*
 * Rename a file.
 */
jboolean java_io_File_renameToInternal(struct Hjava_io_File* this,
				       struct Hjava_lang_String* fromName,
				       struct Hjava_lang_String* toName)
{
  char str[MAXPATHLEN];
  char str2[MAXPATHLEN];
  int r;
  
  stringJava2CBuf(fromName, str, sizeof(str));
  stringJava2CBuf(toName, str2, sizeof(str2));
  
  r = KRENAME(str, str2);
  return (r != 0 ? 0 : 1);
}

/*
 * Delete a file.
 */
jboolean java_io_File_deleteInternal(struct Hjava_io_File* this, 
				    struct Hjava_lang_String* fileName)
{
	char str[MAXPATHLEN];
	int r;
	
	stringJava2CBuf(fileName, str, sizeof(str));

	if (java_io_File_isDirectoryInternal(this, fileName))
	  r = KRMDIR(str);
	else
	  r = KREMOVE(str);
	return(r != 0 ? 0 : 1);
}

/*
 * Get a directory listing.
 */
HArrayOfObject* java_io_File_listInternal(struct Hjava_io_File* this,
					  struct Hjava_lang_String* dirName)
{
#if defined(HAVE_DIRENT_H)
  char path[MAXPATHLEN];
  DIR* dir;
  struct dirent* entry;
  struct dentry {
    struct dentry* next;
    char name[1];
  };
  struct dentry* dirlist;
  struct dentry* mentry;
  HArrayOfObject* array;
  int count;
  int i;
  int oom = 0;
  
  stringJava2CBuf(dirName, path, sizeof(path));
  
  /* XXX make part of jsyscall interface !? */
  dir = opendir(path);
  if (dir == 0) {
    return (0);
  }
  
  dirlist = 0;
  count = 0;
  /* XXX make part of jsyscall interface !? */
  while ((entry = readdir(dir)) != 0) {
    /* We skip '.' and '..' */
    if (strcmp(".", entry->d_name) == 0 ||
	strcmp("..", entry->d_name) == 0) {
      continue;
    }
    mentry = KMALLOC(sizeof(struct dentry) + NAMLEN(entry));
    if (!mentry) {
      errorInfo info;
      
      while (dirlist) {
	mentry = dirlist;
	dirlist = dirlist->next;
	KFREE(mentry);
      }
      postOutOfMemory(&info);
      throwError(&info);
    }
    strcpy(mentry->name, entry->d_name);
    mentry->next = dirlist;
    dirlist = mentry;
    count++;
  }
  /* XXX make part of jsyscall interface !? */
  closedir(dir);
  
  array = (HArrayOfObject*)AllocObjectArray(count,
					    "Ljava/lang/String;", 0);
  /* XXX: This assert is a noop.  If AllocObjectArray throws an
     exception, we leak. */
  assert(array != 0);
  for (i = 0; i < count; i++) {
    mentry = dirlist;
    dirlist = mentry->next;
    unhand_array(array)->body[i] =
      (Hjava_lang_Object*)stringC2Java(mentry->name);
    /* if allocation fails, continue freeing mentries in
       this loop. */
    oom |= !unhand_array(array)->body[i];
    KFREE(mentry);
  }
  if (oom) {
    errorInfo info;
    postOutOfMemory(&info);
    throwError(&info);
  }
  
  return (array);
#else
  return (0);
#endif
}

jboolean java_io_File_createInternal(struct Hjava_lang_String* fileName)
{
  char str[MAXPATHLEN];
  int fd;
  int rc;
  
  stringJava2CBuf(fileName, str, sizeof(str));

  rc = KOPEN(str, O_EXCL|O_WRONLY|O_BINARY|O_CREAT, 0600, &fd);
  switch (rc) {
  case 0:
    break;
  case EEXIST:
    return 0;
  default:
    SignalError("java.io.IOException", SYS_ERROR(rc));
  }
  rc = KCLOSE(fd);
  if (rc != 0)
    SignalError("java.io.IOException", SYS_ERROR(rc));
  return 1;
}

jboolean java_io_File_setLastModifiedInternal(struct Hjava_io_File* this,
					      struct Hjava_lang_String* fileName,
					      jlong thetime)
{
#ifdef HAVE_UTIME_H
  char path[MAXPATHLEN];
  struct utimbuf ub;
#endif
  
  if (thetime < 0)
    SignalError("java.lang.IllegalArgumentException", "time < 0");
#ifdef HAVE_UTIME_H
  stringJava2CBuf(fileName, path, sizeof(path));
  ub.actime = (time_t)(thetime / 1000);
  ub.modtime = ub.actime;
  return (utime(path, &ub) >= 0);
#else
  return 0;
#endif
}

jboolean java_io_File_setReadOnlyInternal(struct Hjava_io_File* this, 
					  struct Hjava_lang_String* fileName)
{
  struct stat buf;
  char str[MAXPATHLEN];
  int r;
  
  stringJava2CBuf(fileName, str, sizeof(str));
  
  r = KSTAT(str, &buf);
  if (r != 0)
    return 0;
  
  r = chmod(str, buf.st_mode & ~(S_IWOTH|S_IWGRP|S_IWUSR));
  return (r == 0);
}

