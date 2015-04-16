#include "mongo_interface.h"
#include <stdio.h>
#include <stdlib.h>
mongoc_client_pool_t *pool;
static void
print_doc(const bson_t *b)
{
	char *str;

	str = bson_as_json(b, NULL);
	MONGOC_DEBUG("%s", str);
	bson_free(str);
}


static void
ping(mongoc_database_t *db,
const bson_t      *cmd)
{
	mongoc_cursor_t *cursor;
	const bson_t *b;
	bson_error_t error;

	cursor = mongoc_database_command(db, MONGOC_QUERY_NONE, 0, 1, 0, cmd, NULL, NULL);
	if (mongoc_cursor_next(cursor, &b)) {
		BSON_ASSERT(b);
		print_doc(b);
	}
	if (mongoc_cursor_error(cursor, &error)) {
		MONGOC_WARNING("Cursor error: %s", error.message);
	}
	mongoc_cursor_destroy(cursor);
}


static void
fetch(mongoc_collection_t *col,
const bson_t        *spec)
{
	mongoc_cursor_t *cursor;
	const bson_t *b;
	bson_error_t error;

	cursor = mongoc_collection_find(col, MONGOC_QUERY_NONE, 0, 0, 0, spec, NULL, NULL);
	while (mongoc_cursor_next(cursor, &b)) {
		BSON_ASSERT(b);
		print_doc(b);
	}
	if (mongoc_cursor_error(cursor, &error)) {
		MONGOC_WARNING("Cursor error: %s", error.message);
	}
	mongoc_cursor_destroy(cursor);
}
static void
test_load(mongoc_client_t *client,
unsigned int        iterations)
{
	mongoc_collection_t *col;
	mongoc_database_t *db;
	bson_error_t error;
	unsigned int i;
	bson_t b;
	bson_t q;

	bson_init(&b);
	bson_append_int32(&b, "ping", 4, 1);

	bson_init(&q);

	db = mongoc_client_get_database(client, "admin");
	col = mongoc_client_get_collection(client, "lmdb", "article.files");

	for (i = 0; i < iterations; i++) {
		printf("%d          ", i);
		ping(db, &b);
		fetch(col, &q);
	}

	if (!mongoc_collection_drop(col, &error)) {
		MONGOC_WARNING("Failed to drop collection: %s", error.message);
	}

	mongoc_database_destroy(db);
	db = mongoc_client_get_database(client, "test");

	if (!mongoc_database_drop(db, &error)) {
		MONGOC_WARNING("Failed to drop database: %s", error.message);
	}

	mongoc_database_destroy(db);
	mongoc_collection_destroy(col);
	bson_destroy(&b);
}
int testgridfs(int argc, char *argv[])
{
	mongoc_gridfs_t *gridfs;
	mongoc_gridfs_file_t *file;
	mongoc_gridfs_file_list_t *list;
	mongoc_gridfs_file_opt_t opt = { 0 };
	mongoc_client_t *client;
	mongoc_stream_t *stream;
	mongoc_client_pool_t *pool;
	mongoc_uri_t *uri;

	bson_t query;
	bson_t child;
	bson_error_t error;
	ssize_t r;
	char buf[4096];
	mongoc_iovec_t iov;
	const char * filename;
	const char * command;

	if (argc < 2) {
		fprintf(stderr, "usage - %s command ...\n", argv[0]);
		return 1;
	}

	mongoc_init();

	uri = mongoc_uri_new("mongodb://dbimport:dbimport@172.16.10.17:27019");///?sockettimeoutms=5000

	pool = mongoc_client_pool_new(uri);
	client = mongoc_client_pool_pop(pool);
	//test_load(client, 10);
	mongoc_client_pool_push(pool, client);
	client = mongoc_client_pool_pop(pool);

	iov.iov_base = (void *)buf;
	iov.iov_len = sizeof buf;

	assert(client);

	/* grab a gridfs handle in test prefixed by fs */
	gridfs = mongoc_client_get_gridfs(client, "voicedb", "fs", &error);
	assert(gridfs);

	command = argv[1];
	filename = argv[2];

	if (strcmp(command, "read") == 0) {
		if (argc != 3) {
			fprintf(stderr, "usage - %s read filename\n", argv[0]);
			return 1;
		}
		file = mongoc_gridfs_find_one_by_filename(gridfs, filename, &error);
		assert(file);

		stream = mongoc_stream_gridfs_new(file);
		assert(stream);

		for (;;) {
			r = mongoc_stream_readv(stream, &iov, 1, -1, 0);

			assert(r >= 0);

			if (r == 0) {
				break;
			}

			if (fwrite(iov.iov_base, 1, r, stdout) != r) {
				MONGOC_ERROR("Failed to write to stdout. Exiting.\n");
				exit(1);
			}
		}

		mongoc_stream_destroy(stream);
		mongoc_gridfs_file_destroy(file);
	}
	else if (strcmp(command, "list") == 0) {
		bson_init(&query);
		bson_append_document_begin(&query, "$orderby", -1, &child);
		bson_append_int32(&child, "filename", -1, 1);
		bson_append_document_end(&query, &child);
		bson_append_document_begin(&query, "$query", -1, &child);
		bson_append_document_end(&query, &child);

		list = mongoc_gridfs_find(gridfs, &query);

		bson_destroy(&query);

		while ((file = mongoc_gridfs_file_list_next(list))) {
			const char * name = mongoc_gridfs_file_get_filename(file);
			printf("%s:%d\n", name ? name : "?", mongoc_gridfs_file_get_length(file));

			if (276850 == mongoc_gridfs_file_get_length(file))
			{

				//printf("%s:%d\n", mongoc_gridfs_file_get_md5(file), mongoc_gridfs_file_get_length(file));


				stream = mongoc_stream_gridfs_new(file);
				assert(stream);
				//FILE *fp = fopen(mongoc_gridfs_file_get_md5(file), "wb");
				iov.iov_len = 1;
				r = mongoc_stream_readv(stream, &iov, 1, 1, 0);
				if (mongoc_gridfs_file_get_length(file) >= 100)
					mongoc_gridfs_file_seek(file, 2, SEEK_SET);
				for (;;) {
					iov.iov_len = 49;
					r = mongoc_stream_readv(stream, &iov, 1, -1, 0);
					if (r == 0) {
						break;
					}
					
					//if (fwrite(iov.iov_base, 1, r, fp) != r) {
						//MONGOC_ERROR("Failed to write to stdout. Exiting.\n");
					//exit(1);
					//}
					//else
					//{
						//fflush(fp);
					//}
				}
				//fclose(fp);
				mongoc_stream_destroy(stream);
			}
			mongoc_gridfs_file_destroy(file);

		}

		mongoc_gridfs_file_list_destroy(list);
	}
	else if (strcmp(command, "write") == 0) {
		if (argc != 4) {
			fprintf(stderr, "usage - %s write filename input_file\n", argv[0]);
			return 1;
		}

		stream = mongoc_stream_file_new_for_path(argv[3], O_RDONLY, 0);
		assert(stream);

		opt.filename = filename;

		file = mongoc_gridfs_create_file_from_stream(gridfs, stream, &opt);
		assert(file);

		mongoc_gridfs_file_save(file);
		mongoc_gridfs_file_destroy(file);
	}
	else {
		fprintf(stderr, "Unknown command");
		return 1;
	}


	mongoc_client_pool_push(pool, client);

	mongoc_client_pool_destroy(pool);
	mongoc_gridfs_destroy(gridfs);
	//mongoc_client_destroy(client);
	mongoc_cleanup();

	return 0;
}

