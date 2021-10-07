#ifndef ARDUINO_THREADS_SOURCE_HPP_
#define ARDUINO_THREADS_SOURCE_HPP_

/**************************************************************************************
 * FORWARD DECLARATION
 **************************************************************************************/

template<class T>
class Sink;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

template<class T>
class Source
{
  public:
    Source() {};

    void connectTo(Sink<T> &sink) {
      if (destination == nullptr) {
        destination = &sink;
      } else {
        destination->connectTo(sink);
      }
    }

    void send(const T &value) {
      if (destination) destination->inject(value);
    }

  private:
    Sink<T> *destination;
};

#endif /* ARDUINO_THREADS_SOURCE_HPP_ */
