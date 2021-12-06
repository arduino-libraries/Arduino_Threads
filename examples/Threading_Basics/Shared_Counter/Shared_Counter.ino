/* This example demonstrates data exchange between
 * threads using a shared counter variable defined
 * within 'SharedVariables.h'.
 */

void setup()
{
  Producer.start();
  Consumer.start();
}

void loop()
{

}
