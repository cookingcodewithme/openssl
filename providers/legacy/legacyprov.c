/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <string.h>
#include <stdio.h>
#include <openssl/core.h>
#include <openssl/core_numbers.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include "internal/provider_algs.h"

/* Functions provided by the core */
static OSSL_core_get_param_types_fn *c_get_param_types = NULL;
static OSSL_core_get_params_fn *c_get_params = NULL;

/* Parameters we provide to the core */
static const OSSL_ITEM legacy_param_types[] = {
    { OSSL_PARAM_UTF8_PTR, OSSL_PROV_PARAM_NAME },
    { OSSL_PARAM_UTF8_PTR, OSSL_PROV_PARAM_VERSION },
    { OSSL_PARAM_UTF8_PTR, OSSL_PROV_PARAM_BUILDINFO },
    { 0, NULL }
};

static const OSSL_ITEM *legacy_get_param_types(const OSSL_PROVIDER *prov)
{
    return legacy_param_types;
}

static int legacy_get_params(const OSSL_PROVIDER *prov,
                            const OSSL_PARAM params[])
{
    const OSSL_PARAM *p;

    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_NAME);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, "OpenSSL Legacy Provider"))
        return 0;
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_VERSION);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, OPENSSL_VERSION_STR))
        return 0;
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_BUILDINFO);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, OPENSSL_FULL_VERSION_STR))
        return 0;

    return 1;
}

static const OSSL_ALGORITHM legacy_digests[] = {
#ifndef OPENSSL_NO_MD2
    { "MD2", "legacy=yes", md2_functions },
#endif

#ifndef OPENSSL_NO_MD4
    { "MD4", "legacy=yes", md4_functions },
#endif

#ifndef OPENSSL_NO_MDC2
    { "MDC2", "legacy=yes", mdc2_functions },
#endif /* OPENSSL_NO_MDC2 */

#ifndef OPENSSL_NO_WHIRLPOOL
    { "whirlpool", "legacy=yes", wp_functions },
#endif /* OPENSSL_NO_WHIRLPOOL */

#ifndef OPENSSL_NO_RMD160
    { "RIPEMD160", "legacy=yes", ripemd160_functions },
#endif /* OPENSSL_NO_RMD160 */

    { NULL, NULL, NULL }
};

static const OSSL_ALGORITHM *legacy_query(OSSL_PROVIDER *prov,
                                          int operation_id,
                                          int *no_cache)
{
    *no_cache = 0;
    switch (operation_id) {
    case OSSL_OP_DIGEST:
        return legacy_digests;
    }
    return NULL;
}

/* Functions we provide to the core */
static const OSSL_DISPATCH legacy_dispatch_table[] = {
    { OSSL_FUNC_PROVIDER_GET_PARAM_TYPES, (void (*)(void))legacy_get_param_types },
    { OSSL_FUNC_PROVIDER_GET_PARAMS, (void (*)(void))legacy_get_params },
    { OSSL_FUNC_PROVIDER_QUERY_OPERATION, (void (*)(void))legacy_query },
    { 0, NULL }
};

int OSSL_provider_init(const OSSL_PROVIDER *provider,
                       const OSSL_DISPATCH *in,
                       const OSSL_DISPATCH **out,
                       void **provctx)
{
    for (; in->function_id != 0; in++) {
        switch (in->function_id) {
        case OSSL_FUNC_CORE_GET_PARAM_TYPES:
            c_get_param_types = OSSL_get_core_get_param_types(in);
            break;
        case OSSL_FUNC_CORE_GET_PARAMS:
            c_get_params = OSSL_get_core_get_params(in);
            break;
        /* Just ignore anything we don't understand */
        default:
            break;
        }
    }

    *out = legacy_dispatch_table;
    return 1;
}
