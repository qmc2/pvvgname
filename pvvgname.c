// ********************************************************
// pvvgname - reads the name of the VG that a PV belongs to
//            directly from the MDA stored on disk
//
// Copyright (C) 2016 SVA System Vertrieb Alexander GmbH
//
// Author: René Reucher (rene.reucher@sva.de)
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

char *block_device_name = 0;

static struct option long_options[] = {
	{"help",	no_argument,	0,	'h'},
	{"version",	no_argument,	0,	'v'},
	{0,		0,		0,	0}
};

void show_usage(char **argv)
{
	fprintf(stderr, "Usage: %s <device>\n"
			"       %s [-h|--help] [-v|--version]\n", argv[0], argv[0]);
}

void show_version()
{
	printf("pvvgname - v%s\n", XSTR(PVVGNAME_VERSION));
}

int process_arguments(int argc, char **argv)
{
	int c, option_index, retval = 1;
	while ( retval && (c = getopt_long(argc, argv, "hv", long_options, &option_index)) != -1 ) {
		switch ( c ) {
			case 'h':
				show_usage(argv);
				return 0;

			case 'v':
				show_version();
				return 0;

			default:
				break;
		}
	}
	if ( optind < argc ) {
		block_device_name = (char *)malloc(sizeof(char) * strlen(argv[optind]) + 1);
		strcpy(block_device_name, argv[optind]);
		return 1;
	} else {
		show_usage(argv);
		return 0;
	}
}

int clean_up(int rc)
{
	if ( block_device_name )
		free(block_device_name);
	return rc;
}

int main(int argc, char **argv)
{
	if ( !process_arguments(argc, argv) )
		return clean_up(1);
	FILE *f = fopen(block_device_name, "r");
	if ( f ) {
		char buffer[BUFFER_SIZE];
		int len = fread(buffer, 1, BUFFER_SIZE, f);
		fclose(f);
		if ( len > 0 ) {
			char *labelone = memmem(buffer, FIRST4SECTORS, LABEL_ONE, strlen(LABEL_ONE));
			char *magic = memmem(buffer, BUFFER_SIZE, FMTT_MAGIC, strlen(FMTT_MAGIC));
			if ( !labelone || !magic ) {
				fprintf(stderr, "ERROR: %s doesn't appear to be an LVM2 physical volume\n", block_device_name);
				return clean_up(1);
			}
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
					printf("%s\n", vg_name);
				} else
					printf("\n");
			}
			return clean_up(0);
		} else {
			fprintf(stderr, "ERROR: fread() failed\n");
			return clean_up(1);
		}
	} else {
		fprintf(stderr, "ERROR: fopen() failed\n");
		return clean_up(1);
	}
}
