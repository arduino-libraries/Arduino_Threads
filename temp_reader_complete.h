class temp_reader_class {

    Shared<float> temperature;
    void setup() {
      //initialize
    }

    void loop() {
      temperature = random() * 1.0f;
      delay(1000);
    }

    void temp_reader_execute() {
      setup();
      while (1) {
        loop();
      }
    }

    rtos::Thread t;
  public:
    void start() {
      t.start(mbed::callback(this, &temp_reader_class::temp_reader_execute));
    }
};

temp_reader_class temp_reader_obj;
