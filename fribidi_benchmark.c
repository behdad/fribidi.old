/* FriBidi - Library of BiDi algorithm
 * Copyright (C) 1999 Dov Grobgeld
 * Copyright (C) 2001 Behdad Esfahbod
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include "fribidi.h"
#include "config.h"

#define appname "fribidi_benchmark"
#define appversion VERSION

extern guchar *fribidi_version_info;

#define MAX_STR_LEN 1000

void
die (gchar * fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);

  fprintf (stderr, "%s: ", appname);
  vfprintf (stderr, fmt, ap);
  fprintf (stderr, "Try `%s --help' for more information.\n", appname);
  exit (-1);
}

#define TEST_STRING \
  "a THE QUICK -123,456 (FOX JUMPS ) DOG the quick !1@7#4&5^ over the dog " \
  "123,456 OVER THE 5%+ 4.0 LAZY"
#define TEST_STRING_EXPLICIT \
  "this is _LJUST_o a _lsimple _Rte%ST_o th_oat  HAS A _LPDF missing" \
  "AnD hOw_L AbOuT, 123,987 tHiS_o a GO_oOD - _L_oTE_oST. " \
  "here_L is_o_o_o _R a good one_o And _r 123,987_LT_oHE_R next_o oNE:" \
  "_R_r and the last _LONE_o IS THE _rbest _lONE and" \
  "A REAL BIG_l_o BUG! _L _l_r_R_L_laslaj siw_o_Rlkj sslk" \
  "a _L_L_L_LL_L_L_L_L_L_L_L_L_Rbug_o_o_o_o_o_o" \
  "here_L is_o_o_o _R a good one_o And _r 123,987_LT_oHE_R next_o oNE:" \
  "_R_r and the last _LONE_o IS THE _rbest _lONE and" \
  "A REAL BIG_l_o BUG! _L _l_r_R_L_laslaj siw_o_Rlkj sslk" \
  "a _L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_L_Rbug" \
  "here_L is_o_o_o _R ab  one_o _r 123,987_LT_oHE_R t_o oNE:" \

double
utime (void)
{
  struct tms tb;
  times (&tb);
  return 0.01 * tb.tms_utime;
}

void
benchmark (guchar * S_, FriBidiCharSet char_set, gint niter)
{
  gint len, i;
  FriBidiChar us[MAX_STR_LEN], out_us[MAX_STR_LEN];
  FriBidiStrIndex positionLtoV[MAX_STR_LEN], positionVtoL[MAX_STR_LEN];
  guint8 embedding_list[MAX_STR_LEN];
  FriBidiCharType base;
  double time0, time1;

  len = fribidi_charset_to_unicode (char_set, S_, us);

  /* Start timer */
  time0 = utime ();

  for (i = 0; i < niter; i++)
    {
      /* Create a bidi string */
      base = FRIBIDI_TYPE_ON;
      fribidi_log2vis (us, len, &base,
		       /* output */
		       out_us, positionVtoL, positionLtoV, embedding_list);
    }

  /* stop timer */
  time1 = utime ();

  /* output result */
  printf ("Length = %d\n", len);
  printf ("Iterations = %d\n", niter);
  printf ("%d len*iterations in %f seconds\n", len * niter, time1 - time0);
  printf ("= %f kilo.len.iterations/second\n",
	  1.0 * len * niter / 1000 / (time1 - time0));

  return;
}

int
main (int argc, char *argv[])
{
  int argp;
  gint niter;

  niter = 1000;

#define CASE(s) if (strcmp (S_, (s)) == 0)
#define CASE2(s1, s2) if (strcmp (S_, (s1)) == 0 || strcmp (S_, (s2)) == 0)

  /* Parse the command line */
  argp = 1;
  while (argp < argc)
    {
      gchar *S_;

      S_ = argv[argp++];
      if (S_[0] == '-' && S_[1])
	{
	  CASE2 ("-?", "--help")
	  {
	    gint i;

	    printf
	      ("Usage: %s [OPTION]...\n"
	       "A program for benchmarking the speed of the %s library.\n"
	       "\n"
	       "  -?, --help            Display this information and exit\n"
	       "  -V, --version         Display version information and exit\n"
	       "  -n, --niter N         Number of iterations. Default is %d.\n"
	       "\nReport bugs online at <http://fribidi.sourceforge.net/bugs.php>.\n",
	       appname, PACKAGE, niter);
	    exit (0);
	  }
	  CASE2 ("-V", "--version")
	  {
	    printf (appname " " appversion "\n%s", fribidi_version_info);
	    exit (0);
	  }
	  CASE2 ("-n", "--niter")
	  {
	    niter = atoi (argv[argp++]);
	    continue;
	  };
	  die ("unrecognized option `%s'\n", S_);
	}
      else
	die ("unrecognized parameter `%s'\n", S_);
    }

  printf ("* Without explicit marks:\n");
  benchmark (TEST_STRING, FRIBIDI_CHARSET_CAP_RTL, niter);
  printf ("\n");
  printf ("* With explicit marks:\n");
  benchmark (TEST_STRING_EXPLICIT, FRIBIDI_CHARSET_CAP_RTL, niter);

  return 0;
}
