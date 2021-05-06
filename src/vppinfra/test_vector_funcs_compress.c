/* SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2021 Cisco Systems, Inc.
 */

#include <vppinfra/format.h>
#include <vppinfra/vector_funcs.h>
#include <vppinfra/test_vector_funcs.h>

__clib_test_fn u32
clib_compress_u32_wrapper (u32 *dst, u32 *src, u64 *mask, u32 n_elts)
{
  return clib_compress_u32 (dst, src, mask, n_elts);
}

typedef struct
{
  u64 mask[10];
  u32 n_elts;
} compress_test_t;

static compress_test_t tests[] = {
  { .mask = { 1 }, .n_elts = 1 },
  { .mask = { 2 }, .n_elts = 2 },
  { .mask = { 3 }, .n_elts = 2 },
  { .mask = { 0, 1 }, .n_elts = 66 },
  { .mask = { 0, 2 }, .n_elts = 69 },
  { .mask = { 0, 3 }, .n_elts = 66 },
  { .mask = { ~0ULL, ~0ULL, ~0ULL, ~0ULL }, .n_elts = 62 },
  { .mask = { ~0ULL, ~0ULL, ~0ULL, ~0ULL }, .n_elts = 255 },
  { .mask = { ~0ULL, 1, 1, ~0ULL }, .n_elts = 256 },
};

static clib_error_t *
test_clib_compress_u32 (clib_error_t *err)
{
  u32 src[513];
  u32 dst[513];
  u32 i, j;

  for (i = 0; i < ARRAY_LEN (src); i++)
    src[i] = i;

  for (i = 0; i < ARRAY_LEN (tests); i++)
    {
      compress_test_t *t = tests + i;
      u32 *dp = dst;
      u32 r;

      for (j = 0; j < ARRAY_LEN (dst); j++)
	dst[j] = 0xa5a5a5a5;

      r = clib_compress_u32_wrapper (dst, src, t->mask, t->n_elts);

      for (j = 0; j < t->n_elts; j++)
	{
	  if ((t->mask[j >> 6] & (1ULL << (j & 0x3f))) == 0)
	    continue;

	  if (dp[0] != src[j])
	    return clib_error_return (err,
				      "wrong data in testcase %u at "
				      "(dst[%u] = 0x%x, src[%u] = 0x%x)",
				      i, dp - dst, dp[0], j, src[j]);
	  dp++;
	}

      if (dst[dp - dst + 1] != 0xa5a5a5a5)
	return clib_error_return (err, "buffer overrun in testcase %u", i);

      if (dp - dst != r)
	return clib_error_return (err, "wrong number of elts in testcase %u",
				  i);
    }

  return err;
}

REGISTER_TEST (clib_compress_u32) = {
  .name = "clib_compress_u32",
  .fn = test_clib_compress_u32,
};
