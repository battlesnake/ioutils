#include <cstd/std.h>
#include <io/io.h>
#include <debug/hexdump.h>

int main(int argc, char *argv[])
{
	prog_name = "readdr";

	const char *this = argv[0];

	if (argc != 2) {
		log_error("Syntax: %s <address>", this);
		exit(1);
	}

	const char *addr = argv[1];

	struct file_source fi;
	struct file_sink fo;

	struct io_intf io;

	if (!file_source_init(&fi, NULL)) {
		log_error("Failed to open source");
		exit(13);
	}

	if (!file_sink_init(&fo, NULL, fsm_create)) {
		log_error("Failed to open sink");
		exit(13);
	}

	if (!io_intf_init(&io, NULL, &fi, &fo, io_relay, io_relay, NULL, NULL)) {
		log_error("Failed to initialise IO adapter");
		exit(13);
	}

	struct relay_packet *packet;
	while (io_intf_recv(&io, &packet) && packet) {
		snprintf(packet->local, sizeof(packet->local), "%s", addr);
		io_intf_forward(&io, packet);
		free(packet);
	}

	io_intf_destroy(&io);
	file_source_destroy(&fi);
	file_sink_destroy(&fo);

	return 0;
}
