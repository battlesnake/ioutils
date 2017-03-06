#include <cstd/std.h>
#include <io/io.h>
#include <debug/hexdump.h>

int main(int argc, char *argv[])
{
	prog_name = "tap";

	const char *this = argv[0];

	if (argc != 1) {
		log_error("Syntax: %s", this);
		exit(1);
	}

	struct file_source fi;
	struct file_sink fo;

	struct io_intf io;

	if (!file_source_init(&fi, NULL)) {
		log_error("Failed to open source");
		exit(13);
	}

	if (!file_sink_init(&fo, NULL, false)) {
		log_error("Failed to open sink");
		exit(13);
	}

	if (!io_intf_init(&io, NULL, &fi, &fo, io_relay, io_relay, NULL, NULL)) {
		log_error("Failed to initialise IO adapter");
		exit(13);
	}

	struct relay_packet *packet;
	char type[RELAY_TYPE_LENGTH];
	size_t count = 0;
	while ((packet = io_intf_recv(&io))) {
		if (strcmp(packet->type, type) == 0) {
			count++;
		} else {
			if (count > 0) {
				log_info("Received type '%.*s' %zu times", (int) sizeof(type), type, count);
			}
			strncpy(type, packet->type, sizeof(type));
			count = 1;
		}
		io_intf_forward(&io, packet);
		free(packet);
	}

	if (count > 0) {
		log_info("Received type '%.*s' %zu times", (int) sizeof(type), type, count);
	}

	io_intf_destroy(&io);
	file_source_destroy(&fi);
	file_sink_destroy(&fo);

	return 0;
}
