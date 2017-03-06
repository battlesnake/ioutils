#include <cstd/std.h>
#include <identnode/ident_rpc_handler.h>
#include <io/io.h>
#include <debug/hexdump.h>

static enum io_handler_result packet_handle(struct io_intf *io, const struct relay_packet *rp, void *arg)
{
	const char *replace = arg;
	io_intf_send(io, replace, NULL, rp->data, rp->length);
	return iohr_consumed;
}

int main(int argc, char *argv[])
{
	prog_name = "chtype";

	ident_init("Packet type substitution");

	const char *this = argv[0];

	if (argc != 3) {
		log_error("Syntax: %s <find-type> <replace-type>", this);
		exit(1);
	}

	const char *find = argv[1];
	const char *replace = argv[2];

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

	if (!io_intf_init(&io, NULL, &fi, &fo, io_relay, io_relay, find, replace)) {
		log_error("Failed to initialise IO adapter");
		exit(13);
	}

	const struct io_handler handlers[2] = {
		[0] = ident_handler,
		[1] = {
			.type = find,
			.func = packet_handle,
			.arg = (void *) replace
		}
	};

	io_intf_loop(&io, handlers, 2);

	io_intf_destroy(&io);
	file_source_destroy(&fi);
	file_sink_destroy(&fo);

	return 0;
}
