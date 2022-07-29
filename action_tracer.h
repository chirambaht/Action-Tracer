#ifndef ACTION_TRACER_H
#define ACTION_TRACER_H
namespace ActionTracer {
	class ActionTracer {
	  private:
		/* data */
	  public:
		ActionTracer( /* args */ );
		~ActionTracer();

		// add copy, move, and assignment constructors
		ActionTracer( const ActionTracer &other );
		ActionTracer( ActionTracer &&other );
		ActionTracer &operator=( const ActionTracer &other );
		ActionTracer &operator=( ActionTracer &&other );

		void start();
		void stop();
		void pause();
		void resume();
		void reset();

		void status();

		void initialize();
		void close();

		void map_device();
		void unmap_device();
		void device_status();

		void set_server_details();

		void set_packet_data();

		void set_fifo_rate();
	};

} // namespace ActionTracer

#endif // ACTION_TRACER_H