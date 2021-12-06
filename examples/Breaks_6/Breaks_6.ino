/* This example is in fact expected to break, since we try
 * to access a thread-private variable.
 */

#include <Arduino_Threads.h>

void setup()
{
  Thread.start();
}

void loop()
{
  int const my_var = var;
}
