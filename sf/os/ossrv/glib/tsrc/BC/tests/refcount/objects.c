/* Portion Copyright � 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. */
#include <unistd.h>
#include <glib.h>
#include <glib-object.h>


#ifdef SYMBIAN
#include <glib_global.h>
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/



#define G_TYPE_TEST               (g_test_get_type ())
#define G_TEST(test)              (G_TYPE_CHECK_INSTANCE_CAST ((test), G_TYPE_TEST, GTest))
#define G_IS_TEST(test)           (G_TYPE_CHECK_INSTANCE_TYPE ((test), G_TYPE_TEST))
#define G_TEST_CLASS(tclass)      (G_TYPE_CHECK_CLASS_CAST ((tclass), G_TYPE_TEST, GTestClass))
#define G_IS_TEST_CLASS(tclass)   (G_TYPE_CHECK_CLASS_TYPE ((tclass), G_TYPE_TEST))
#define G_TEST_GET_CLASS(test)    (G_TYPE_INSTANCE_GET_CLASS ((test), G_TYPE_TEST, GTestClass))

typedef struct _GTest GTest;
typedef struct _GTestClass GTestClass;

struct _GTest
{
  GObject object;
};

struct _GTestClass
{
  GObjectClass parent_class;
};

static GType g_test_get_type (void);
static volatile gboolean stopping;

static void g_test_class_init (GTestClass * klass);
static void g_test_init (GTest * test);
static void g_test_dispose (GObject * object);

static GObjectClass *parent_class = NULL;

static GType
g_test_get_type (void)
{
  static GType test_type = 0;

  if (!test_type) {
    static const GTypeInfo test_info = {
      sizeof (GTestClass),
      NULL,
      NULL,
      (GClassInitFunc) g_test_class_init,
      NULL,
      NULL,
      sizeof (GTest),
      0,
      (GInstanceInitFunc) g_test_init,
      NULL
    };

    test_type = g_type_register_static (G_TYPE_OBJECT, "GTest",
        &test_info, 0);
  }
  return test_type;
}

static void
g_test_class_init (GTestClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  gobject_class->dispose = g_test_dispose;
}

static void
g_test_init (GTest * test)
{
  //g_print ("init %p\n", test);
}

static void
g_test_dispose (GObject * object)
{
  GTest *test;

  test = G_TEST (object);

  //g_print ("dispose %p!\n", object);

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
g_test_do_refcount (GTest * test)
{
  g_object_ref (test); 
  g_object_unref (test); 
}

static gpointer
run_thread (GTest * test)
{
  gint i = 1;

  while (!stopping) {
    g_test_do_refcount (test);
    if ((i++ % 10000) == 0) {
      //g_print (".");
      g_thread_yield(); /* force context switch */
    }
  }

  return NULL;
}

int
main (int argc, char **argv)
{
  gint i;
  GTest *test1, *test2;
  GArray *test_threads;
  const guint n_threads = 5;

  #ifdef SYMBIAN
  
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  

  g_thread_init (NULL);
  //g_print ("START: %s\n", argv[0]);
  g_log_set_always_fatal (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL | g_log_set_always_fatal (G_LOG_FATAL_MASK));
  g_type_init ();

  test1 = g_object_new (G_TYPE_TEST, NULL);
  g_assert(test1 != NULL);
  
  test2 = g_object_new (G_TYPE_TEST, NULL);
  g_assert(test2 != NULL);

  test_threads = g_array_new (FALSE, FALSE, sizeof (GThread *));

  stopping = FALSE;

  for (i = 0; i < n_threads; i++) {
    GThread *thread;

    thread = g_thread_create ((GThreadFunc) run_thread, test1, TRUE, NULL);
    g_array_append_val (test_threads, thread);

    thread = g_thread_create ((GThreadFunc) run_thread, test2, TRUE, NULL);
    g_array_append_val (test_threads, thread);
  }
  g_usleep (5000000);

  stopping = TRUE;

  //g_print ("\nstopping\n");

  /* join all threads */
  for (i = 0; i < 2 * n_threads; i++) {
    GThread *thread;

    thread = g_array_index (test_threads, GThread *, i);
    g_thread_join (thread);
  }

  //g_print ("stopped\n");
#ifdef SYMBIAN
  testResultXml("objects");
#endif /* EMULATOR */

  return 0;
}
