/* Skeleton for test programs.
   Copyright (C) 1998,2000-2004, 2005 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1998.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#define _GLIBCXX_HAVE_SETENV

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <malloc.h>
#include <search.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <time.h>
#include "zrt.h"

#ifdef __native_client__
# define fork(...) (-1)
# define kill(...) (-1)
# define waitpid(...) (-1)
#endif


/* The test function is normally called `do_test' and it is called
   with argc and argv as the arguments.  We nevertheless provide the
   possibility to overwrite this name.  */
#ifndef TEST_FUNCTION
# define TEST_FUNCTION do_test (argc, argv)
#endif

#ifndef TEST_DATA_LIMIT
# define TEST_DATA_LIMIT (64 << 20) /* Data limit (bytes) to run with.  */
#endif

#define OPT_DIRECT 1000
#define OPT_TESTDIR 1001

static struct option options[] =
  {
#ifdef CMDLINE_OPTIONS
    CMDLINE_OPTIONS
#endif
    { "direct", no_argument, NULL, OPT_DIRECT },
    { "test-dir", required_argument, NULL, OPT_TESTDIR },
    { NULL, 0, NULL, 0 }
  };

/* Directory to place temporary files in.  */
static const char *test_dir;

/*List of temporary files.  */
struct temp_name_list
{
  struct qelem q;
  const char *name;
} *s_temp_name_list;

/*Add temporary files in list.  */
static void
__attribute__ ((unused))
add_temp_file (const char *name)
{
  struct temp_name_list *newp
    = (struct temp_name_list *) calloc (sizeof (*newp), 1);
  if (newp != NULL)
    {
      newp->name = name;
      if (s_temp_name_list == NULL)
	s_temp_name_list = (struct temp_name_list *) &newp->q;
      else
	insque (newp, s_temp_name_list);
    }
}

/*Delete all temporary files.  */
static void
__attribute__ ((unused))
delete_temp_files (void)
{
  while (s_temp_name_list != NULL)
    {
      remove (s_temp_name_list->name);
      s_temp_name_list = (struct temp_name_list *) s_temp_name_list->q.q_forw;
    }
}

/*Create a temporary file.  */
static int
__attribute__ ((unused))
create_temp_file (const char *base, char **filename)
{
  char *fname;
  int fd;

  fname = (char *) malloc (strlen (test_dir) + 1 + strlen (base)
			   + sizeof ("XXXXXX"));
  if (fname == NULL)
    {
      puts ("out of memory");
      return -1;
    }
  strcpy (stpcpy (stpcpy (stpcpy (fname, test_dir), "/"), base), "XXXXXX");

  fd = mkstemp (fname);
  if (fd == -1)
    {
      printf ("cannot open temporary file '%s': %m\n", fname);
      free (fname);
      return -1;
    }

  add_temp_file (fname);
  if (filename != NULL)
    *filename = fname;

  return fd;
}
//
//#ifdef CLEANUP_HANDLER
//  CLEANUP_HANDLER;
//#endif
//
//  /* If we expected this signal: good!  */
//#ifdef EXPECTED_SIGNAL
//  if (EXPECTED_SIGNAL == SIGALRM)
//    exit (0);
//#endif
//
//  if (killed == 0 || (WIFSIGNALED (status) && WTERMSIG (status) == SIGKILL))
//    fputs ("Timed out: killed the child process\n", stderr);
//  else if (WIFSTOPPED (status))
//    fprintf (stderr, "Timed out: the child process was %s\n",
//       strsignal (WSTOPSIG (status)));
//  else if (WIFSIGNALED (status))
//    fprintf (stderr, "Timed out: the child process got signal %s\n",
//       strsignal (WTERMSIG (status)));
//  else
//    fprintf (stderr, "Timed out: killed the child process but it exited %d\n",
//       WEXITSTATUS (status));
//
//  /* Exit with an error.  */
//  exit (1);
//}

/* We provide the entry point here.  */
int
main (int argc, char **argv)
{
  int direct = 1;   /* In Native Client fork is not implemented.  */
  int status;
  int opt;
  unsigned int timeoutfactor = 1;

  setenv("","",0);

  /* Make uses of freed and uninitialized memory known.  */
  mallopt (M_PERTURB, 42);

#ifdef STDOUT_UNBUFFERED
  setbuf (stdout, NULL);
#endif

  while ((opt = getopt_long (argc, argv, "+", options, NULL)) != -1)
    switch (opt)
      {
      case '?':
	exit (1);
      case OPT_DIRECT:
	direct = 1;
	break;
      case OPT_TESTDIR:
	test_dir = optarg;
	break;
#ifdef CMDLINE_PROCESS
	CMDLINE_PROCESS
#endif
	  }

  /* If set, read the test TIMEOUTFACTOR value from the environment.
     This value is used to scale the default test timeout values. */
  char *envstr_timeoutfactor = getenv ("TIMEOUTFACTOR");
  if (envstr_timeoutfactor != NULL)
    {
      char *envstr_conv = envstr_timeoutfactor;
      unsigned long int env_fact;

      env_fact = strtoul (envstr_timeoutfactor, &envstr_conv, 0);
      if (*envstr_conv == '\0' && envstr_conv != envstr_timeoutfactor)
	timeoutfactor = MAX (env_fact, 1);
    }

  /* Set TMPDIR to specified test directory.  */
  if (test_dir != NULL)
    {
      setenv ("TMPDIR", test_dir, 1);

      if (chdir (test_dir) < 0)
	{
	  perror ("chdir");
	  exit (1);
	}
    }
  else
    {
      test_dir = getenv ("TMPDIR");
      if (test_dir == NULL || test_dir[0] == '\0')
	test_dir = "/tmp";
    }

  /* Make sure we see all message, even those on stdout.  */
  setvbuf (stdout, NULL, _IONBF, 0);

  /* make sure temporary files are deleted.  */
  //atexit (delete_temp_files);

  /* Correct for the possible parameters.  */
  argv[optind - 1] = argv[0];
  argv += optind - 1;
  argc -= optind - 1;

  /* Call the initializing function, if one is available.  */
#ifdef PREPARE
  PREPARE (argc, argv);
#endif

  /* Launch test always directly under zerovm, because threads are not supported*/
  if (direct){
    return TEST_FUNCTION;
  }

  /* Set up the test environment:
     - prevent core dumps
     - set up the timer
     - fork and execute the function.  */

  {
    /* This is the child.  */
#ifdef RLIMIT_CORE
    /* Try to avoid dumping core.  */
    struct rlimit core_limit;
    core_limit.rlim_cur = 0;
    core_limit.rlim_max = 0;
    setrlimit (RLIMIT_CORE, &core_limit);
#endif

#ifdef RLIMIT_DATA
    /* Try to avoid eating all memory if a test leaks.  */
    struct rlimit data_limit;
    if (getrlimit (RLIMIT_DATA, &data_limit) == 0)
      {
	if (TEST_DATA_LIMIT == RLIM_INFINITY)
	  data_limit.rlim_cur = data_limit.rlim_max;
	else if (data_limit.rlim_cur > (rlim_t) TEST_DATA_LIMIT)
	  data_limit.rlim_cur = MIN ((rlim_t) TEST_DATA_LIMIT,
				     data_limit.rlim_max);
	if (setrlimit (RLIMIT_DATA, &data_limit) < 0)
	  perror ("setrlimit: RLIMIT_DATA");
      }
    else
      perror ("getrlimit: RLIMIT_DATA");
#endif

    /* Execute the test function and exit with the return value.   */
    exit (TEST_FUNCTION);
  }

  /* Simply exit with the return value of the test.  */
#ifndef EXPECTED_STATUS
  return WEXITSTATUS (status);
#else
  if (WEXITSTATUS (status) != EXPECTED_STATUS)
    {
      fprintf (stderr, "Expected status %d, got %d\n",
	       EXPECTED_STATUS, WEXITSTATUS (status));
      exit (1);
    }

  return 0;
#endif
}
