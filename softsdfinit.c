/*
 *  Copyright 2014-2024 The GmSSL Project. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the License); you may
 *  not use this file except in compliance with the License.
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmssl/sm2.h>
#include <gmssl/rand.h>
#include <gmssl/error.h>


static const char *usage = "-kek num -key num -pass str";

static const char *options =
"\n"
"Options\n"
"\n"
"    -kek num            KEK index\n"
"    -key num            Private key index\n"
"    -pass str           Password for accessing the private key\n"
"\n"
"Examples\n"
"\n"
"  $ softsdfinit -kek 1 -key 1 -pass P@ssw0rd\n"
"\n";


static int generate_kek(unsigned int uiKEKIndex)
{
	char filename[256];
	uint8_t kek[16];
	FILE *file;

	if (rand_bytes(kek, sizeof(kek)) != 1) {
		error_print();
		return -1;
	}

	snprintf(filename, sizeof(filename), "kek-%u.key", uiKEKIndex);
	if (!(file = fopen(filename, "wb"))) {
		error_print();
		return -1;
	}
	if (fwrite(kek, 1, sizeof(kek), file) != sizeof(kek)) {
		fclose(file);
		error_print();
		return -1;
	}
	fclose(file);

	printf("KEK #%u generated, saved to kek-%u.key\n", uiKEKIndex, uiKEKIndex);

	return 1;
}

static int generate_sign_key(unsigned int uiKeyIndex, const char *pass)
{
	SM2_KEY sm2_key;
	char filename[256];
	FILE *file;

	if (sm2_key_generate(&sm2_key) != 1) {
		error_print();
		return -1;
	}

	snprintf(filename, sizeof(filename), "sm2sign-%u.pem", uiKeyIndex);
	if ((file = fopen(filename, "wb")) == NULL) {
		fclose(file);
		error_print();
		return -1;
	}
	if (sm2_private_key_info_encrypt_to_pem(&sm2_key, pass, file) != 1) {
		error_print();
		return -1;
	}
	fclose(file);

	snprintf(filename, sizeof(filename), "sm2signpub-%u.pem", uiKeyIndex);
	if ((file = fopen(filename, "wb")) == NULL) {
		fclose(file);
		error_print();
		return -1;
	}
	if (sm2_public_key_info_to_pem(&sm2_key, file) != 1) {
		error_print();
		return -1;
	}
	fclose(file);

	printf("SM2 signing key pair #%u generated, saved to sm2sign-%u.pem and sm2signpub-%u.pem\n", uiKeyIndex, uiKeyIndex, uiKeyIndex);

	return 1;
}

static int generate_enc_key(unsigned int uiKeyIndex, const char *pass)
{
	SM2_KEY sm2_key;
	char filename[256];
	FILE *file;

	if (sm2_key_generate(&sm2_key) != 1) {
		error_print();
		return -1;
	}

	snprintf(filename, sizeof(filename), "sm2enc-%u.pem", uiKeyIndex);
	if ((file = fopen(filename, "wb")) == NULL) {
		fclose(file);
		error_print();
		return -1;
	}
	if (sm2_private_key_info_encrypt_to_pem(&sm2_key, pass, file) != 1) {
		error_print();
		return -1;
	}
	fclose(file);

	snprintf(filename, sizeof(filename), "sm2encpub-%u.pem", uiKeyIndex);
	if ((file = fopen(filename, "wb")) == NULL) {
		fclose(file);
		error_print();
		return -1;
	}
	if (sm2_public_key_info_to_pem(&sm2_key, file) != 1) {
		error_print();
		return -1;
	}
	fclose(file);

	printf("SM2 encryption key pair #%u generated, saved to sm2enc-%u.pem and sm2encpub-%u.pem\n", uiKeyIndex, uiKeyIndex, uiKeyIndex);

	return 1;
}

int main(int argc, char **argv)
{
	int ret = 1;
	char *prog = argv[0];
	int kek = 1;
	int key = 1;
	char *pass = NULL;

	argc--;
	argv++;

	if (argc < 1) {
		fprintf(stderr, "gmssl %s %s\n", prog, usage);
		return 1;
	}

	while (argc > 0) {
		if (!strcmp(*argv, "-help")) {
			printf("usage: %s %s\n", prog, usage);
			printf(options, prog);
			ret = 0;
			goto end;
		} else if (!strcmp(*argv, "-kek")) {
			if (--argc < 1) goto bad;
			kek = atoi(*(++argv));
			if (kek < 1 || kek > 4096) {
				fprintf(stderr, "%s: `-kek` invalid index\n", prog);
				goto end;
			}
		} else if (!strcmp(*argv, "-key")) {
			if (--argc < 1) goto bad;
			key = atoi(*(++argv));
			if (key < 0 || key > 4096) {
				fprintf(stderr, "%s: `-key` invalid index\n", prog);
				goto end;
			}
		} else if (!strcmp(*argv, "-pass")) {
			if (--argc < 1) goto bad;
			pass = *(++argv);
		} else {
			fprintf(stderr, "%s: illegal option `%s`\n", prog, *argv);
			goto end;
bad:
			fprintf(stderr, "%s: `%s` option value missing\n", prog, *argv);
			goto end;
		}

		argc--;
		argv++;
	}

	if (!pass) {
		fprintf(stderr, "%s: option `-pass` missing\n", prog);
		goto end;
	}

	if (generate_kek((unsigned int)kek) != 1) {
		fprintf(stderr, "%s: generate kek failed\n", prog);
		goto end;
	}
	if (generate_sign_key((unsigned int)key, pass) != 1) {
		fprintf(stderr, "%s: generate signing key pair failed\n", prog);
		goto end;
	}
	if (generate_enc_key((unsigned int)key, pass) != 1) {
		fprintf(stderr, "%s: generate signing key pair failed\n", prog);
		goto end;
	}

	ret = 0;
end:
	return ret;
}

