// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
/* 
 * Bitcoin cryptography library
 * Copyright (c) Project Nayuki
 * 
 * https://www.nayuki.io/page/bitcoin-cryptography-library
 * https://github.com/nayuki/Bitcoin-Cryptography-Library
 */

#pragma once


void countOps(long n);
	
enum {
	functionOps = 1,
	arithmeticOps = 1,
	loopBodyOps = 3,  // Increment, compare, jump
	uint256CopyOps = 17,
	fieldintCopyOps = 18,  // 1 + uint256CopyOps
	curvepointCopyOps = 55,  // 1 + 3 * fieldintCopyOps
};
