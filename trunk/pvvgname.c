// ********************************************************
// pvvgname - reads the name of the VG that a PV belongs to
//            directly from the MDA stored on disk
//
// Copyright (C) 2016 - 2022 rene.reucher@batcom-it.net
// ********************************************************

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define STR(s)		#s
#define XSTR(s)		STR(s)
#define UINT32(x)	(unsigned char)*((x) + 3) * 16777216 + (unsigned char)*((x) + 2) * 65536 + (unsigned char)*((x) + 1) * 256 + (unsigned char)*((x) + 0)
#define SECTOR_SIZE	512
#define FIRST4SECTORS	4 * SECTOR_SIZE
#define BUFFER_SIZE	2048 * SECTOR_SIZE
#define MDA_OFFSET	4096
#define VGN_OFFSET	MDA_OFFSET + 40
#define FMTT_MAGIC 	"\040\114\126\115\062\040\170\133\065\101\045\162\060\116\052\076" // from LVM2's source (lib/format-text/layout.h)
#define LABEL_ONE	"LABELONE"

char long_output = 0;
char **block_device_names = 0;
int number_of_block_devices = 0;

static struct option long_options[] = {
	{"help",	no_argument,	0,	'h'},
	{"version",	no_argument,	0,	'v'},
	{"long",	no_argument,	0,	'l'},
	{0,		0,		0,	0}
};

void show_usage(char **argv)
{
	fprintf(stderr,
		"Usage: %s [-l|--long] <device> [<device> ...]\n"
		"       %s [-v|--version] [-h|--help]\n",
		argv[0],
		argv[0]);
}

void show_version()
{
	printf("pvvgname - v%s\n", XSTR(PVVGNAME_VERSION));
}

int process_arguments(int argc, char **argv)
{
	int c, option_index, help_or_version = 0, retval = 1;
	while ( retval && (c = getopt_long(argc, argv, "hvl", long_options, &option_index)) != -1 ) {
		switch ( c ) {
			case 'h':
				show_usage(argv);
				help_or_version = 1;
				break;

			case 'v':
				show_version();
				help_or_version = 1;
				break;

			case 'l':
				long_output = 1;
				break;

			default:
				break;
		}
	}
	if ( help_or_version )
		return 0;
	number_of_block_devices = argc - optind;
	if ( number_of_block_devices > 0 ) {
		block_device_names = (char **)malloc(sizeof(char *) * number_of_block_devices);
		int index = optind;
		while ( index < argc ) {
			block_device_names[index - optind] = (char *)malloc(sizeof(char) * strlen(argv[index]) + 1);
			strcpy(block_device_names[index - optind], argv[index]);
			index++;
		}
		return 1;
	} else {
		show_usage(argv);
		return 0;
	}
}

void clean_up()
{
	if ( block_device_names ) {
		for (int i = 0; i < number_of_block_devices; i++)
			free(block_device_names[i]);
		free(block_device_names);
	}
}

int main(int argc, char **argv)
{
	if ( !process_arguments(argc, argv) ) {
		clean_up();
		return 1;
	}
	int rc = 0;
	for (int index = 0; index < number_of_block_devices; index++) {
		FILE *f = fopen(block_device_names[index], "r");
		if ( f ) {
			char buffer[BUFFER_SIZE];
			int len = fread(buffer, 1, BUFFER_SIZE, f);
			fclose(f);
			if ( len > 0 ) {
				char *labelone = memmem(buffer, FIRST4SECTORS, LABEL_ONE, strlen(LABEL_ONE));
				char *magic = memmem(buffer, BUFFER_SIZE, FMTT_MAGIC, strlen(FMTT_MAGIC));
				if ( !labelone || !magic ) {
					fprintf(stderr, "ERROR: %s doesn't appear to be an LVM2 physical volume\n", block_device_names[index]);
					rc = 1;
				} else {
					unsigned int offset = UINT32(buffer + VGN_OFFSET);
					if ( offset > 0 ) {
						char *p = buffer + offset + MDA_OFFSET;
						char *q = p;
						while ( *q != ' ' )
							q++;
						int vg_name_len = q - p;
						if ( vg_name_len >= 0 ) {
							char vg_name[vg_name_len + 1];
							strncpy(vg_name, p, vg_name_len);
							vg_name[vg_name_len] = 0;
							if ( long_output ) {
								while ( *q++ != '\"' );
								p = q;
								while ( *q != '\"' )
									q++;
								int vg_uuid_len = q - p;
								char vg_uuid[vg_uuid_len + 1];
								strncpy(vg_uuid, p, vg_uuid_len);
								vg_uuid[vg_uuid_len] = 0;
								printf("%s\t%s\t%s\n", block_device_names[index], vg_name, vg_uuid);
							} else
								printf("%s\n", vg_name);
						}
					}
				}
			} else {
				fprintf(stderr, "ERROR: fread() failed for %s\n", block_device_names[index]);
				rc = 1;
			}
		} else {
			fprintf(stderr, "ERROR: fopen() failed for %s\n", block_device_names[index]);
			rc = 1;
		}
	}
	clean_up();
	return rc;
}
