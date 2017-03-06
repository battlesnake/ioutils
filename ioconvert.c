#include <cstd/std.h>
#include <io/io.h>
#include <debug/hexdump.h>

int main(int argc, char *argv[])
{
	prog_name = "convert";

	bool verbose = false;
	const char *this = argv[0];
	if (argc >= 2 && strcmp(argv[1], "-v") == 0) {
		argc--;
		argv++;
		verbose = true;
	}

	if (argc < 3) {
		log_error("Syntax: %s <in-format> <out-format> [<relay-type>]", this);
		exit(1);
	}

	enum io_format fmti = io_format_val(argv[1]);
	enum io_format fmto = io_format_val(argv[2]);
	const char *relay_type = argc == 4 ? argv[3] : NULL;

	if ((int) fmti == -1) {
		log_error("Invalid input format: %s", argv[1]);
		exit(1);
	}

	if ((int) fmto == -1) {
		log_error("Invalid output format: %s", argv[2]);
		exit(1);
	}

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

	if (!io_intf_init(&io, NULL, &fi, &fo, fmti, fmto, relay_type, relay_type)) {
		log_error("Failed to initialise IO adapter");
		exit(13);
	}

	size_t total_chunks = 0;
	size_t total_bytes = 0;
	size_t converted_chunks = 0;
	size_t converted_bytes = 0;

	struct relay_packet *packet;
	while ((packet = io_intf_recv(&io))) {
		total_chunks++;
		total_bytes += packet->length;
		if (relay_type == NULL || strcmp(packet->type, relay_type) == 0) {
			converted_chunks++;
			converted_bytes += packet->length;
			io_intf_send(&io, relay_type, NULL, packet->data, packet->length);
		}
		free(packet);
	}

	io_intf_destroy(&io);
	file_source_destroy(&fi);
	file_sink_destroy(&fo);

	if (verbose) {
		log_info("            |     Bytes | Chunks |");
		log_info("------------+-----------+--------+");
		log_info("      Total |%10zu |%7zu |", total_bytes, total_chunks);
		log_info("------------+-----------+--------+");
		log_info("  Converted |%10zu |%7zu |", converted_bytes, converted_chunks);
		log_info("------------+-----------+--------+");
	}

	return 0;
}
