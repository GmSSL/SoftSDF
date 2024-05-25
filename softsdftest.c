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
#include <gmssl/sm3.h>
#include "sdf.h"

int main(void)
{
	void *hDeviceHandle = NULL;
	void *hSessionHandle = NULL;
	unsigned char ucData[3] = { 0x61, 0x62, 0x63 };
	unsigned int uiDataLength = (unsigned int)sizeof(ucData);
	unsigned char ucHash[32];
	unsigned int uiHashLength;
	int ret;

	SM3_CTX sm3_ctx;
	unsigned char dgst[32];


	ret = SDF_OpenDevice(&hDeviceHandle);
	if (ret != SDR_OK) {
		fprintf(stderr, "Error: SDF_OpenDevice: 0x%X\n", ret);
		return -1;
	}

	ret = SDF_OpenSession(hDeviceHandle, &hSessionHandle);
	if (ret != SDR_OK) {
		fprintf(stderr, "Error: SDF_OpenSession: 0x%X\n", ret);
		return -1;
	}

	ret = SDF_HashInit(hSessionHandle, SGD_SM3, NULL, NULL, 0);
	if (ret != SDR_OK) {
		fprintf(stderr, "Error: SDF_HashInit: 0x%X\n", ret);
		return -1;
	}

	ret = SDF_HashUpdate(hSessionHandle, ucData, uiDataLength);
	if (ret != SDR_OK) {
		fprintf(stderr, "Error: SDF_HashUpdate: 0x%X\n", ret);
		return -1;
	}

	ret = SDF_HashFinal(hSessionHandle, ucHash, &uiHashLength);
	if (ret != SDR_OK) {
		fprintf(stderr, "Error: SDF_HashFinal: 0x%X\n", ret);
		return -1;
	}
	SDF_CloseSession(hSessionHandle);
	SDF_CloseDevice(hDeviceHandle);


	// check with gmssl

	sm3_init(&sm3_ctx);
	sm3_update(&sm3_ctx, ucData, sizeof(ucData));
	sm3_finish(&sm3_ctx, dgst);

	if (uiHashLength != 32) {
		fprintf(stderr, "Error: error hash lenght\n");
		return -1;
	}
	if (memcmp(ucHash, dgst, 32) != 0) {
		fprintf(stderr, "Error: error hash value\n");
		return -1;
	}

	printf("test ok\n");
	return 0;
}
